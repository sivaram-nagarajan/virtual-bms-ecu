# Functional Safety Concept (ISO 26262, simplified)

> This is a portfolio project. The analysis below follows the structure of
> ISO 26262-3 (concept phase) so the safety requirements in
> [requirements.md](requirements.md) have a visible source. It is not a
> complete safety case.

## 1. Item definition

**Item:** battery management function of a 12S lithium-ion (NMC) traction or
auxiliary battery, made up of cell monitoring, state estimation, protection and
main contactor control.

| Parameter | Value |
|-----------|-------|
| Cell chemistry | NMC, 2.5 V to 4.2 V operating window |
| Configuration | 12 cells in series, 4 temperature sensors |
| Nominal capacity | 50 Ah |
| Max continuous current | 150 A discharge, 50 A charge |
| Operating temperature | −20 °C to 60 °C (discharge), 0 °C to 45 °C (charge) |
| Actuators | Main positive and negative contactors, precharge relay |

**Interfaces:** CMC (cell voltages, temperatures, current), VCU (contactor
requests, receives status and limits), diagnostic tester (UDS).

## 2. Hazard analysis and risk assessment (excerpt)

| ID | Malfunction | Hazardous event | S | E | C | ASIL |
|----|-------------|-----------------|---|---|---|------|
| H-01 | Cell overcharged, no disconnection | Thermal runaway while vehicle parked or charging, fire | S3 | E4 | C3 | **C** (see note) |
| H-02 | Cell over-discharged, no disconnection | Internal damage, later thermal event on charge | S3 | E3 | C3 | **B** |
| H-03 | Over-temperature not detected | Thermal runaway during driving | S3 | E3 | C3 | **B** |
| H-04 | Over-current not interrupted | Cable or cell overheating, fire | S3 | E3 | C2 | **A** |
| H-05 | Contactors open unexpectedly while driving | Loss of propulsion at speed | S2 | E4 | C2 | **B** |

S = severity, E = exposure, C = controllability. Ratings are illustrative.
Note: real projects often rate H-01 as ASIL C or D depending on the vehicle; C
is used here.

## 3. Safety goals

| ID | Safety goal | ASIL | Safe state | FTTI |
|----|-------------|------|------------|------|
| SG-01 | Prevent cell voltage above the maximum limit | C | Contactors open | 1 s |
| SG-02 | Prevent cell voltage below the minimum limit | B | Contactors open | 1 s |
| SG-03 | Prevent cell temperature outside the allowed range | B | Contactors open | 2 s |
| SG-04 | Prevent pack current above the allowed limit | A | Contactors open | 500 ms |
| SG-05 | Prevent unintended opening of the contactors while driving | B | Contactors stay closed, warning to VCU | n/a |

SG-05 conflicts with SG-01 to SG-04. It is resolved by giving the VCU a
warning (`BMS_Limits` derated to zero) before opening, except for faults where
the FTTI does not allow it.

## 4. Functional safety requirements

| ID | Requirement | Derived from | ASIL |
|----|-------------|--------------|------|
| FSR-01 | The BMS shall detect cell over-voltage and open the contactors within the FTTI. | SG-01 | C |
| FSR-02 | The BMS shall detect cell under-voltage and open the contactors within the FTTI. | SG-02 | B |
| FSR-03 | The BMS shall detect cell over- and under-temperature and open the contactors within the FTTI. | SG-03 | B |
| FSR-04 | The BMS shall detect pack over-current and open the contactors within the FTTI. | SG-04 | A |
| FSR-05 | The BMS shall detect loss or corruption of measurement data and treat it as a fault. | SG-01..04 | C |
| FSR-06 | The BMS shall only accept contactor commands that pass the E2E check. | SG-05 | B |
| FSR-07 | The BMS shall report every detected fault to the VCU and store it as a DTC. | SG-01..05 | QM |

The software requirements in [requirements.md](requirements.md) refine these
FSRs. Each one names the FSR it comes from in the *Parent* column.

## 5. Assumptions of use

- The CMC measures cell voltages with ±5 mV accuracy and temperatures with ±2 °C.
- Contactor hardware has an independent hardware over-current trip (fuse). This software path is not the only protection.
- A hardware watchdog supervises the BMS on a real target. On Linux, overruns are counted and reported.
