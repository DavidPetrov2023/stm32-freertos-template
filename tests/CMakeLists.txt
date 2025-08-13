cmake_minimum_required(VERSION 3.20)
project(unit_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)

# Download GoogleTest automatically into build-tests/ directory
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Force shared CRT for MSVC (no effect on Linux)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Unit test executable
add_executable(unit_tests
    test_main.cpp
)

target_link_libraries(unit_tests
    gtest_main
    pthread
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# ---- Optional coverage instrumentation ----
option(ENABLE_COVERAGE "Build tests with coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "Enabling coverage instrumentation (GNU) for unit_tests")
        # Apply coverage flags only to unit test binary
        target_compile_options(unit_tests PRIVATE -O0 -g --coverage)
        target_link_options(unit_tests PRIVATE --coverage)
    else()
        message(WARNING "ENABLE_COVERAGE is ON, but compiler is not GNU -> ignoring")
    endif()
endif()

# ---- LCov/GenHTML target ----
find_program(LCOV_EXEC lcov)
find_program(GENHTML_EXEC genhtml)

add_custom_target(coverage
    # Reset counters
    COMMAND ${CMAKE_COMMAND} -E env LC_ALL=C
            ${LCOV_EXEC} --directory ${CMAKE_BINARY_DIR} --zerocounters

    # Run all tests
    COMMAND ctest --output-on-failure

    # Capture results (ignore GTest macro mismatches)
    COMMAND ${LCOV_EXEC}
            --directory ${CMAKE_BINARY_DIR}
            --capture
            --rc geninfo_unexecuted_blocks=1
            --ignore-errors mismatch
            --output-file ${CMAKE_BINARY_DIR}/coverage.info

    # Remove system headers and dependencies from report
    COMMAND ${LCOV_EXEC}
            --remove ${CMAKE_BINARY_DIR}/coverage.info
            /usr/* */_deps/*               # Keep tests for now until real code is added
            --ignore-errors unused,empty
            -o ${CMAKE_BINARY_DIR}/coverage.info

    # Generate HTML report
    COMMAND ${GENHTML_EXEC} ${CMAKE_BINARY_DIR}/coverage.info
            --legend --title "Unit Tests Coverage"
            --output-directory ${CMAKE_BINARY_DIR}/coverage_html

    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Run tests and generate HTML coverage report"
    VERBATIM
)
