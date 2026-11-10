#!/usr/bin/env python3
"""Check that #include directives respect the layer rules in docs/architecture.md §3.

Allowed dependencies (a layer may include itself and the layers listed):
  swc      -> rte
  rte      -> bsw          (RTE maps signals onto Com)
  bsw      -> platform
  platform -> (nothing)
  ecu      -> everything   (integration code: main, tasks, configuration)
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ALLOWED = {
    "swc": {"swc", "rte"},
    "rte": {"rte", "bsw"},
    "bsw": {"bsw", "platform"},
    "platform": {"platform"},
}
INCLUDE = re.compile(r'^\s*#\s*include\s+"bms/([a-z]+)/')


def layer_of(path: Path) -> str | None:
    parts = path.parts
    for root in ("src", "include"):
        if root in parts:
            idx = parts.index(root)
            sub = parts[idx + 1 : idx + 3]
            layer = sub[1] if root == "include" and len(sub) > 1 else sub[0]
            return layer if layer in ALLOWED else None
    return None


def main() -> int:
    errors = []
    for path in sorted(list(Path("src").rglob("*.[ch]")) + list(Path("include").rglob("*.h"))):
        layer = layer_of(path)
        if layer is None:
            continue
        for lineno, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
            match = INCLUDE.match(line)
            if match and match.group(1) not in ALLOWED[layer]:
                errors.append(f"{path}:{lineno}: {layer} must not include bms/{match.group(1)}/")
    for err in errors:
        print(err, file=sys.stderr)
    print(f"Layer check: {len(errors)} violation(s).")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
