#!/usr/bin/env bash
# Static analysis: MISRA C:2012 (cppcheck addon), general cppcheck, clang-tidy.
# Usage: tools/static_analysis.sh [build-dir]
set -euo pipefail
BUILD_DIR="${1:-build}"
cd "$(dirname "$0")/.."

# MISRA scope: ECU software that would run on the target (BSW, RTE, SWCs and
# the platform-independent scheduler). Linux integration code (SocketCAN,
# POSIX timing, main) is out of scope and documented as such in docs/misra.md.
MISRA_FILES=$(find src/bsw src/rte src/swc src/ecu -name '*.c' ! -name 'main.c' 2>/dev/null; echo src/platform/os.c)
OTHER_FILES="src/platform/can_socketcan.c src/platform/os_linux.c src/ecu/main.c"

MISRA_ADDON="misra"
# Rule texts are copyrighted by MISRA and not shipped; add tools/misra_rules.txt
# locally (from your licensed copy) to get rule descriptions in the output.
if [[ -f tools/misra_rules.txt ]]; then MISRA_ADDON="tools/misra.json"; fi

echo "== cppcheck + MISRA C:2012 (target code) =="
# shellcheck disable=SC2086
cppcheck --std=c11 --enable=warning,style,performance,portability \
  --addon="${MISRA_ADDON}" --inline-suppr --error-exitcode=1 \
  --suppressions-list=tools/misra_suppressions.txt \
  --suppress=missingIncludeSystem --suppress=checkersReport --suppress=unusedFunction \
  -I include -I src/ecu ${MISRA_FILES}

echo "== cppcheck (Linux integration code) =="
# shellcheck disable=SC2086
cppcheck --std=c11 --enable=warning,style,performance,portability \
  --inline-suppr --error-exitcode=1 \
  --suppress=missingIncludeSystem --suppress=checkersReport --suppress=unusedFunction \
  -I include -I src/ecu ${OTHER_FILES}

echo "== clang-tidy =="
# shellcheck disable=SC2046
clang-tidy -p "${BUILD_DIR}" --quiet $(find src test -name '*.c' -o -name '*.cpp')

echo "== layer dependencies =="
python3 tools/check_layers.py
