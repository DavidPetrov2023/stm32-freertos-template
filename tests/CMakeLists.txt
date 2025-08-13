cmake_minimum_required(VERSION 3.20)
project(unit_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)
# Stažením GTestu vyřešíme závislosti automaticky (do build-tests/)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Pro MSVC (nevadí na Linuxu, jen držíme tutorial-ově)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
  test_main.cpp
)

target_link_libraries(unit_tests
  gtest_main
  pthread
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

# ---- Coverage volitelně ---------------------------------
option(ENABLE_COVERAGE "Build tests with coverage instrumentation" OFF)

# ... (FetchContent + add_executable(unit_tests ...) + target_link_libraries ...)

if(ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Enabling coverage instrumentation (GNU) for unit_tests")
    # jen na náš test binary
    target_compile_options(unit_tests PRIVATE -O0 -g --coverage)
    target_link_options(unit_tests PRIVATE --coverage)
  else()
    message(WARNING "ENABLE_COVERAGE is ON, but compiler is not GNU -> ignoring")
  endif()
endif()

# ---- LCov/GenHTML target --------------------------------
find_program(LCOV_EXEC lcov)
find_program(GENHTML_EXEC genhtml)

add_custom_target(coverage
  COMMAND ${CMAKE_COMMAND} -E env LC_ALL=C
          ${LCOV_EXEC} --directory ${CMAKE_BINARY_DIR}
                       --zerocounters

  COMMAND ctest --output-on-failure

  # capture + tolerance na drobné neshody řádků z GTest makro-kódu
  COMMAND ${LCOV_EXEC}
          --directory ${CMAKE_BINARY_DIR}
          --capture
          --rc geninfo_unexecuted_blocks=1
          --ignore-errors mismatch
          --output-file ${CMAKE_BINARY_DIR}/coverage.info


   # odfiltruj systém a závislosti, testy zatím ponech
  COMMAND ${LCOV_EXEC}
        --remove ${CMAKE_BINARY_DIR}/coverage.info
        /usr/* */_deps/*  # POZOR: NEzahazujeme */tests/* dokud nemáme testovaný kód mimo testy
        --ignore-errors unused,empty
        -o ${CMAKE_BINARY_DIR}/coverage.info

  COMMAND ${GENHTML_EXEC} ${CMAKE_BINARY_DIR}/coverage.info
          --legend --title "Unit Tests Coverage"
          --output-directory ${CMAKE_BINARY_DIR}/coverage_html

  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Run tests and generate HTML coverage report"
  VERBATIM
)

