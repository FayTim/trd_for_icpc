"""Exercise real stdin/stdout, including interactive flush and error termination."""
import os
import selectors
import subprocess
import sys
import time


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def run(executable, args=(), data=b""):
    return subprocess.run([executable, *args], input=data, capture_output=True,
                          timeout=3, check=False)


def play(executable, n, secret=None, error=None):
    with subprocess.Popen([executable], stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE) as process:
        try:
            process.stdin.write(f"{n}\n".encode())
            process.stdin.flush()
            buffer = b""
            queries = 0
            with selectors.DefaultSelector() as selector:
                selector.register(process.stdout, selectors.EVENT_READ)

                def read_line():
                    nonlocal buffer
                    deadline = time.monotonic() + 3
                    while b"\n" not in buffer:
                        remaining = deadline - time.monotonic()
                        require(remaining > 0 and selector.select(remaining),
                                "no complete query: missing flush or deadlock")
                        chunk = os.read(process.stdout.fileno(), 4096)
                        require(chunk, "contestant exited before the answer")
                        buffer += chunk
                    line, buffer = buffer.split(b"\n", 1)
                    return line.decode()

                while True:
                    parts = read_line().split()
                    require(len(parts) == 2, "malformed protocol line")
                    marker, value = parts[0], int(parts[1])
                    if marker == "!":
                        require(error is None and value == secret, "wrong final answer")
                        expected_code = 0
                        break
                    require(marker == "?" and 1 <= value <= n, "illegal query")
                    queries += 1
                    require(queries <= n.bit_length(), "query budget exceeded")
                    if error is not None:
                        if error != "EOF":
                            process.stdin.write((error + "\n").encode())
                            process.stdin.flush()
                        expected_code = 1
                        break
                    reply = "LT" if secret < value else "GT" if secret > value else "EQ"
                    process.stdin.write((reply + "\n").encode())
                    process.stdin.flush()
            process.stdin.close()
            process.stdin = None
            tail, diagnostic = process.communicate(timeout=3)
            require(not buffer and not tail, "unexpected output after answer/error")
            require(process.returncode == expected_code,
                    f"unexpected exit status: {process.returncode}, {diagnostic!r}")
        except BaseException:
            process.kill()
            process.wait()
            raise


def main():
    ordinary, interactive = sys.argv[1:]
    result = run(ordinary, data=b"-7 4\n")
    require(result.returncode == 0 and result.stdout == b"-3\n" and not result.stderr,
            "ordinary judge mode is incorrect")
    for executable in (ordinary, interactive):
        result = run(executable, ("--test",))
        require(result.returncode == 0 and not result.stdout,
                "self-tests must succeed without printing to stdout")
        for args in (("--unknown",), ("--test", "extra")):
            result = run(executable, args)
            require(result.returncode == 2 and not result.stdout and result.stderr,
                    "unknown arguments must print usage to stderr and fail")
    for n in (1, 2, 3, 7, 8, 9, 31):
        for secret in range(1, n + 1):
            play(interactive, n, secret)
    for secret in (1, (2**63 - 1) // 2, 2**63 - 1):
        play(interactive, 2**63 - 1, secret)
    for error in ("ERROR", "unknown", "EOF"):
        play(interactive, 7, error=error)
    for n in (0, -1):
        result = run(interactive, data=f"{n}\n".encode())
        require(result.returncode == 1 and not result.stdout, "invalid n must be rejected")
    print("contest template process tests passed")


if __name__ == "__main__":
    main()
