# MISRA C:2012 Compliance

## Scope

| Code | MISRA checked | Reason |
|------|---------------|--------|
| `src/bsw`, `src/rte`, `src/swc`, `src/ecu` (except `main.c`), `src/platform/os.c` | Yes | Code that would run on the target ECU |
| `src/platform/can_socketcan.c`, `src/platform/os_linux.c`, `src/ecu/main.c` | No (cppcheck and clang-tidy only) | Linux integration that a real target replaces with MCAL drivers and an AUTOSAR OS. It uses POSIX APIs that are outside MISRA by nature |
| `test/` | No | Host test code (C++) |

Checks run with the cppcheck MISRA addon in CI (`tools/static_analysis.sh`).
The addon covers most decidable rules. It is not a qualified tool, so on a real
project its results would be confirmed with a qualified checker.

## Deviations

| ID | Rule | Location | Justification |
|----|------|----------|---------------|
| — | — | — | No deviations so far. New entries need a reason here and a line in `tools/misra_suppressions.txt`. |
