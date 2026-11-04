#!/usr/bin/env python3
"""Expand the TRD's Markdown/code includes, optionally building its PDF."""

import argparse
from pathlib import Path
import re
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
DIRECTIVE = re.compile(r"^\{\{\s*(include|code):\s*(.+?)\s*\}\}\s*$")


def expand(source: Path, ancestors: tuple = ()) -> str:
    source = source.resolve()
    if source in ancestors:
        chain = " -> ".join(str(path) for path in (*ancestors, source))
        raise ValueError(f"Cyclic document include: {chain}")

    output = []
    for line in source.read_text(encoding="utf-8").splitlines(keepends=True):
        directive = DIRECTIVE.fullmatch(line)
        if directive is None:
            output.append(line)
            continue

        kind, relative_path = directive.groups()
        target = source.parent / relative_path
        if kind == "include":
            output.append(expand(target, (*ancestors, source)).rstrip() + "\n\n")
        else:
            code = target.read_text(encoding="utf-8").rstrip()
            # Keep the fence valid even if the source itself contains backticks.
            longest_run = max((len(run) for run in re.findall(r"`+", code)), default=0)
            fence = "`" * max(3, longest_run + 1)
            output.append(f"{fence}cpp\n{code}\n{fence}\n\n")
    return "".join(output)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=ROOT / "trd.md")
    parser.add_argument("--output-dir", type=Path, default=ROOT / "build/document")
    parser.add_argument(
        "--pdf", action="store_true", help="also build PDF with Pandoc and XeLaTeX"
    )
    args = parser.parse_args()

    try:
        markdown = expand(args.source)
        output_dir = args.output_dir.resolve()
        output_dir.mkdir(parents=True, exist_ok=True)
        output = output_dir / "trd.md"
        output.write_text(markdown, encoding="utf-8")
        print(f"Wrote {output}")
        if args.pdf:
            pdf = output_dir / "trd.pdf"
            subprocess.run(
                [
                    "pandoc",
                    str(output),
                    "--defaults",
                    "document/pandoc.yaml",
                    "--output",
                    str(pdf),
                ],
                cwd=ROOT,
                check=True,
            )
            print(f"Wrote {pdf}")
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(f"Document build failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
