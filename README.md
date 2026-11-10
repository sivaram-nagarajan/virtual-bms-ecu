# virtual-bms-ecu

[![CI](https://github.com/sivaram-nagarajan/virtual-bms-ecu/actions/workflows/ci.yml/badge.svg)](https://github.com/sivaram-nagarajan/virtual-bms-ecu/actions/workflows/ci.yml)

Battery management ECU software for a 12-cell lithium-ion pack, written in
MISRA C and running on Linux over a virtual CAN bus. The whole ECU, with its
plant model and diagnostic tester, runs on a laptop with no hardware.

It is built the way production ECU software is built: an AUTOSAR-style layered
architecture, requirements traced to tests, an ISO 26262 safety concept, MISRA
C:2012 checks, and CI on every change.

## Status

| Increment | Content | State |
|-----------|---------|-------|
| PR 1 | Architecture, requirements, safety concept, scheduler, CAN driver and routing | ✅ |
| PR 2 | Cell monitoring, SOC estimation, protections, contactor state machine, E2E | ⏳ |
| PR 3 | UDS diagnostics (ISO 14229) over ISO-TP, DTC memory | ⏳ |
| PR 4 | Battery plant simulator and end-to-end demo | ⏳ |

## Documents

- [Software architecture](docs/architecture.md): layers, scheduling, CAN matrix, error handling
- [Functional safety concept](docs/safety-concept.md): item definition, HARA, safety goals, FSRs
- [Software requirements](docs/requirements.md): traced to tests in CI
- [MISRA C:2012 compliance](docs/misra.md): scope and deviations
- [CAN database](dbc/bms.dbc)

## Build and test

```bash
sudo apt-get install -y cmake gcc g++ libgtest-dev cppcheck clang-tidy can-utils
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure

tools/static_analysis.sh build   # MISRA C, clang-tidy, layer rules
python3 tools/trace.py           # build/traceability.md
```

## Run on a virtual CAN bus

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

./build/bms_ecu -i vcan0
```

In a second terminal, watch the bus or send frames:

```bash
candump vcan0
cansend vcan0 200#A00FA00FA00FA00F   # CMC_CellVoltages_1: 4 x 4.000 V
```

Stop the ECU with Ctrl+C. It prints scheduler and CAN routing statistics on
exit.

## Repository layout

```
include/bms/<layer>/   public headers (platform, bsw, rte, swc)
src/<layer>/           implementation
src/ecu/               main(), task bodies, configuration tables
test/unit/             GoogleTest unit tests, tagged @verifies REQ-...
test/stubs/            link-time test doubles
dbc/                   CAN database
docs/                  architecture, safety concept, requirements, MISRA
tools/                 static analysis, traceability, layer checks
```

## License

MIT
