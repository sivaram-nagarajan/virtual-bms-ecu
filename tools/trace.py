#!/usr/bin/env python3
"""Build a requirement-to-test traceability matrix.

Reads requirement IDs from docs/requirements.md and `@verifies` tags from the
test sources, writes a Markdown matrix, and exits non-zero when a requirement
is untested or a test refers to an unknown requirement.
"""
from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

REQ_ROW = re.compile(r"^\|\s*(REQ-[A-Z0-9-]+)\s*\|\s*([^|]+?)\s*\|(.*)$")
VERIFIES = re.compile(r"@verifies\s+([A-Z0-9 ,-]+)")
TEST_NAME = re.compile(r"^\s*TEST(?:_F|_P)?\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)")


def read_requirements(path: Path) -> tuple[dict[str, str], set[str]]:
    """Return (id -> text, ids whose status is Planned)."""
    reqs: dict[str, str] = {}
    planned: set[str] = set()
    for line in path.read_text(encoding="utf-8").splitlines():
        match = REQ_ROW.match(line)
        if match:
            reqs[match.group(1)] = match.group(2)
            cells = [c.strip().lower() for c in match.group(3).split("|")]
            if "planned" in cells:
                planned.add(match.group(1))
    return reqs, planned


def read_test_links(test_dir: Path) -> dict[str, list[str]]:
    """Map requirement ID -> list of 'Suite.Test (file)' that verify it."""
    links: dict[str, list[str]] = defaultdict(list)
    for src in sorted(test_dir.rglob("*.c*")):
        pending: list[str] = []
        for line in src.read_text(encoding="utf-8").splitlines():
            tag = VERIFIES.search(line)
            if tag:
                pending += [r.strip() for r in tag.group(1).split(",") if r.strip()]
                continue
            test = TEST_NAME.match(line)
            if test and pending:
                name = f"{test.group(1)}.{test.group(2)} ({src.name})"
                for req in pending:
                    links[req].append(name)
                pending = []
    return links


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--requirements", type=Path, default=Path("docs/requirements.md"))
    parser.add_argument("--tests", type=Path, default=Path("test"))
    parser.add_argument("--output", type=Path, default=Path("build/traceability.md"))
    args = parser.parse_args()

    reqs, planned = read_requirements(args.requirements)
    links = read_test_links(args.tests)

    untested = [r for r in reqs if not links.get(r) and r not in planned]
    unknown = sorted(set(links) - set(reqs))

    rows = ["| Requirement | Description | Verified by |", "|---|---|---|"]
    for req_id, text in reqs.items():
        default = "_planned_" if req_id in planned else "**NOT VERIFIED**"
        tests = "<br>".join(links.get(req_id, [])) or default
        rows.append(f"| {req_id} | {text} | {tests} |")
    active = len(reqs) - len(planned)
    covered = active - len(untested)
    summary = (f"**Coverage: {covered}/{active} implemented requirements verified "
               f"({len(planned)} planned).**")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("# Traceability Matrix\n\n" + summary + "\n\n" + "\n".join(rows) + "\n",
                           encoding="utf-8")
    print(summary.strip("*"))

    for req in untested:
        print(f"ERROR: {req} has no verifying test", file=sys.stderr)
    for req in unknown:
        print(f"ERROR: tests reference unknown requirement {req}", file=sys.stderr)
    return 1 if (untested or unknown) else 0


if __name__ == "__main__":
    sys.exit(main())
