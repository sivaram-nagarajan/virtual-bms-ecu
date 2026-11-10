# Changelog

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and
the project uses [Semantic Versioning](https://semver.org/).

## [Unreleased]
### Added
- Software architecture, functional safety concept (HARA, safety goals, FSRs) and software requirements.
- CAN database `dbc/bms.dbc`.
- Cyclic scheduler (10 ms and 100 ms tasks) with overrun detection.
- SocketCAN driver and table-driven CanIf routing.
- Skeletons for Com and CanTp.
- CI: GCC and Clang builds, unit tests, MISRA C:2012, clang-tidy, layer check, coverage, traceability, vcan smoke test.
