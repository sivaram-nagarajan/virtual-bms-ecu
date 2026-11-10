# Prefer the system GoogleTest (apt: libgtest-dev); fall back to downloading it.
find_package(GTest QUIET)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

# Host test double for the CAN driver (link-time substitution).
add_library(bms_test_stubs STATIC stubs/can_drv_stub.c)
target_include_directories(bms_test_stubs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stubs)
bms_component(bms_test_stubs)

add_executable(bms_unit_tests
  unit/test_crc8_j1850.cpp
  unit/test_os.cpp
  unit/test_canif.cpp
  unit/test_ecu_integration.cpp
)
target_link_libraries(bms_unit_tests PRIVATE bms_core bms_test_stubs GTest::gtest_main)
autolib_set_warnings(bms_unit_tests)

include(GoogleTest)
gtest_discover_tests(bms_unit_tests)
