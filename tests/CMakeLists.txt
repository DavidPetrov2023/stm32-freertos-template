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
