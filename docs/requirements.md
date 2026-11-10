# Software Requirements

IDs are stable. Tests reference them with `// @verifies <ID>`, and
`tools/trace.py` builds the traceability matrix. CI fails if a requirement with
status **Implemented** has no test, or if a test names an unknown ID.

Status values: **Implemented** (code and tests exist) or **Planned** (scheduled
for a later pull request).

## Platform and scheduling

| ID | Requirement | Parent | ASIL | Status |
|----|-------------|--------|------|--------|
| REQ-OS-001 | The scheduler shall call the 10 ms task on every tick and the 100 ms task on every tenth tick. | Arch §4 | QM | Implemented |
| REQ-OS-002 | The scheduler shall count a task overrun when a tick starts later than its period. | Arch §4 | QM | Implemented |
| REQ-CANIF-001 | CanIf shall route each received frame to the upper layer configured for its CAN ID. | Arch §5 | QM | Implemented |
| REQ-CANIF-002 | CanIf shall discard frames with an unconfigured CAN ID or a DLC shorter than configured, and count them. | FSR-05 | B | Implemented |
| REQ-CRC-001 | The CRC module shall compute CRC-8 SAE J1850 (polynomial 0x1D, initial value 0xFF, final XOR 0xFF). | FSR-05, FSR-06 | B | Implemented |
| REQ-CRC-002 | The CRC module shall return the CRC of an empty block, without dereferencing, when the data pointer is NULL. | FSR-05 | B | Implemented |
| REQ-CRC-003 | The CRC module shall support chained calculation, so that splitting a block across calls gives the same result as one call. | FSR-05 | B | Implemented |

## Measurement and state estimation

| ID | Requirement | Parent | ASIL | Status |
|----|-------------|--------|------|--------|
| REQ-CELL-001 | CellMonitor shall mark a cell voltage outside 0 V to 5 V as invalid. | FSR-05 | C | Planned |
| REQ-CELL-002 | CellMonitor shall provide minimum, maximum and average cell voltage over valid cells. | FSR-01, FSR-02 | C | Planned |
| REQ-CELL-003 | CellMonitor shall raise a communication fault when no CMC frame was received for 300 ms. | FSR-05 | C | Planned |
| REQ-SOC-001 | SocEstimator shall initialise SOC from the open-circuit voltage table at start-up. | — | QM | Planned |
| REQ-SOC-002 | SocEstimator shall update SOC by coulomb counting every 100 ms and limit it to 0 % to 100 %. | — | QM | Planned |

## Protection and contactor control

| ID | Requirement | Parent | ASIL | Status |
|----|-------------|--------|------|--------|
| REQ-PROT-001 | Protection shall latch an over-voltage fault when any cell exceeds 4.25 V for 500 ms. | FSR-01 | C | Planned |
| REQ-PROT-002 | Protection shall latch an under-voltage fault when any cell is below 2.50 V for 500 ms. | FSR-02 | B | Planned |
| REQ-PROT-003 | Protection shall latch an over-temperature fault above 60 °C and an under-temperature fault below −20 °C, each for 1 s. | FSR-03 | B | Planned |
| REQ-PROT-004 | Protection shall latch an over-current fault when discharge current exceeds 150 A or charge current exceeds 50 A for 200 ms. | FSR-04 | A | Planned |
| REQ-CONT-001 | ContactorCtrl shall close the contactors only through precharge, and only on a valid VCU close request with no latched fault. | FSR-06 | B | Planned |
| REQ-CONT-002 | ContactorCtrl shall open the contactors within one 10 ms cycle of a latched fault. | FSR-01..04 | C | Planned |
| REQ-E2E-001 | The BMS shall discard a VCU_Command whose CRC or alive counter is wrong, and raise a fault after 3 consecutive failures. | FSR-06 | B | Planned |

## Diagnostics

| ID | Requirement | Parent | ASIL | Status |
|----|-------------|--------|------|--------|
| REQ-DEM-001 | Dem shall store one DTC per fault with an ISO 14229-1 status byte. | FSR-07 | QM | Planned |
| REQ-DCM-001 | Dcm shall support UDS services 0x10, 0x11, 0x14, 0x19, 0x22 and 0x3E over ISO-TP on 0x7E0/0x7E8. | FSR-07 | QM | Planned |
