cmake_minimum_required(VERSION 3.20)
project(virtual_bms_ecu VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BMS_BUILD_TESTS "Build unit tests" ON)
option(BMS_COVERAGE "Instrument for code coverage (GCC/Clang)" OFF)
option(BMS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

# The warning helper is shared with automotive-cpp-template.
set(AUTOLIB_WARNINGS_AS_ERRORS ${BMS_WARNINGS_AS_ERRORS})
include(cmake/CompilerWarnings.cmake)

function(bms_component target)
  target_include_directories(${target} PUBLIC ${PROJECT_SOURCE_DIR}/include)
  autolib_set_warnings(${target})
  if(BMS_COVERAGE)
    target_compile_options(${target} PRIVATE --coverage -O0)
    target_link_options(${target} PUBLIC --coverage)
  endif()
endfunction()

# Platform-independent code: everything except the Linux drivers and main().
add_library(bms_core STATIC
  src/platform/os.c
  src/bsw/canif.c
  src/bsw/com.c
  src/bsw/cantp.c
  src/bsw/crc8_j1850.c
  src/ecu/ecu_cfg.c
  src/ecu/bms_tasks.c
)
bms_component(bms_core)
target_include_directories(bms_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ecu)

# Linux platform: SocketCAN driver and the real-time loop.
add_library(bms_platform_linux STATIC
  src/platform/can_socketcan.c
  src/platform/os_linux.c
)
bms_component(bms_platform_linux)
# POSIX/Linux APIs (clock_nanosleep, SocketCAN ioctls) for the integration code only.
target_compile_definitions(bms_platform_linux PRIVATE _DEFAULT_SOURCE)

add_executable(bms_ecu src/ecu/main.c)
bms_component(bms_ecu)
target_compile_definitions(bms_ecu PRIVATE _DEFAULT_SOURCE)
target_link_libraries(bms_ecu PRIVATE bms_core bms_platform_linux)

if(BMS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
