# Software Architecture

## 1. Purpose and scope

`virtual-bms-ecu` is the application software of a battery management system
(BMS) ECU for a 12-cell (12S) lithium-ion pack. It runs as a normal Linux
process and talks to the outside world only through CAN frames, using
SocketCAN. On a laptop the bus is a virtual CAN interface (`vcan0`), so the
whole system can be developed and tested without hardware. On a real target
only the platform layer would change.

The structure follows AUTOSAR Classic layering, deliberately simplified. There
is no code generator, configuration lives in plain C tables, and only the BSW
modules a BMS needs are present.

## 2. System context

```
        +---------------------+          vcan0           +---------------------+
        |  Battery plant sim  |  CMC_* , PackSensor  ->  |                     |
        |  (cell monitoring   | -----------------------> |                     |
        |   controller, pack  |                          |   virtual-bms-ecu   |
        |   current sensor)   |                          |                     |
        +---------------------+                          |                     |
        +---------------------+  VCU_Command  ->         |                     |
        |  Vehicle control    | -----------------------> |                     |
        |  unit (VCU) sim     | <----------------------- |                     |
        +---------------------+  <- BMS_Status/Limits    |                     |
        +---------------------+  UDS 0x7E0 / 0x7E8       |                     |
        |  Diagnostic tester  | <----------------------> |                     |
        |  (uds-diag-tester)  |                          +---------------------+
        +---------------------+
```

A production BMS reads its cells through an analog front end over SPI or
isoSPI. Here the cell monitoring controller (CMC) sends its measurements as CAN
frames, which is also a common topology for distributed BMS designs with
satellite boards.

## 3. Layered architecture

```
+--------------------------------------------------------------------------+
| Application SWCs (src/swc)                                               |
|   CellMonitor   SocEstimator   Protection   ContactorCtrl                |
+--------------------------------------------------------------------------+
| RTE (src/rte)        typed read/write access to signals, no logic       |
+--------------------------------------------------------------------------+
| BSW services (src/bsw)                                                   |
|   Com (signal packing)   E2E (CRC + alive counter)   Dem (DTC memory)    |
|   CanTp (ISO 15765-2)    Dcm (UDS, ISO 14229)        Crc                 |
|   CanIf (routes frames by CAN ID to Com or CanTp)                        |
+--------------------------------------------------------------------------+
| Platform (src/platform)   replaces MCAL + OS on Linux                    |
|   CanDrv (SocketCAN)      Os (cyclic scheduler)      Tim (monotonic ms)  |
+--------------------------------------------------------------------------+
```

Dependency rule: a layer only calls the layer directly below it, or the RTE
for SWCs. SWCs never include BSW or platform headers, and BSW never includes
SWC headers. `tools/check_layers.py` checks every `#include` against these
rules in CI.

## 4. Scheduling

The `Os` module runs a single-threaded, time-triggered loop, as most body and
powertrain ECUs do.

| Task | Period | Runnables, in order |
|------|--------|---------------------|
| `Task_10ms` | 10 ms | CanIf_MainFunctionRx, CanTp_MainFunction, Dcm_MainFunction, Protection_Run10ms, ContactorCtrl_Run10ms, Com_MainFunctionTx |
| `Task_100ms` | 100 ms | CellMonitor_Run100ms, SocEstimator_Run100ms, Dem_MainFunction |

The order inside `Task_10ms` is fixed so that one cycle goes receive, decide,
transmit. That keeps reaction time to a fault within one 10 ms cycle plus the
debounce time.

Overruns are counted (`Os_GetOverrunCount`) and reported through UDS.

## 5. Communication

The CAN matrix is defined in [`dbc/bms.dbc`](../dbc/bms.dbc).

| ID | Name | Sender | Period | Content |
|----|------|--------|--------|---------|
| 0x100 | VCU_Command | VCU | 100 ms | Contactor request, E2E (CRC + alive counter) |
| 0x200..0x202 | CMC_CellVoltages_1..3 | CMC | 100 ms | 4 cells each, 1 mV/bit |
| 0x210 | CMC_Temperatures | CMC | 100 ms | 4 sensors, 1 °C/bit, offset −40 |
| 0x220 | PackSensor | CMC | 10 ms | Pack current (0.1 A/bit, signed), pack voltage (0.01 V/bit) |
| 0x300 | BMS_Status | BMS | 100 ms | State, contactor, SOC, fault flags, E2E |
| 0x301 | BMS_Limits | BMS | 100 ms | Allowed charge and discharge current |
| 0x7E0 / 0x7E8 | Diag request / response | Tester / BMS | on event | UDS over ISO-TP |

Safety-relevant frames (`VCU_Command`, `BMS_Status`) are protected with an
E2E scheme modelled on AUTOSAR E2E Profile 1: a CRC-8 SAE J1850 over the data
ID and payload, plus a 4-bit alive counter.

## 6. Error handling

- **Measurement plausibility:** values outside the sensor range are marked invalid by CellMonitor and treated as a fault.
- **Timeouts:** missing CMC or PackSensor frames for 3 periods raise a communication fault.
- **Protection:** over- and under-voltage, over- and under-temperature, and over-current are debounced and then latched. Any latched fault opens the contactors.
- **Diagnostics:** every fault maps to a DTC in Dem, with status byte handling per ISO 14229-1.

## 7. Directory layout

```
include/bms/<layer>/   public headers per layer
src/<layer>/           implementation per layer
src/ecu/               main(), task bodies, configuration tables
test/unit/             GoogleTest unit tests (@verifies REQ-...)
test/stubs/            link-time stubs (e.g. CanDrv) for host tests
dbc/                   CAN database
docs/                  architecture, requirements, safety concept
```
