###################################################################################################################
# References:
# CMake's Website:
# https://cmake.org/cmake/help/book/mastering-cmake/index.html
# https://cmake.org/cmake/help/latest/module/GoogleTest.html
# 
# GoogleTest's GitHub page:
# https://google.github.io/googletest/quickstart-cmake.html
###################################################################################################################

cmake_minimum_required(VERSION 3.10)       # Updated to 3.10 for vcpkg toolchain support

# Auto vcpkg setup: clones, bootstraps, needs to load before project() call
set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")
if(NOT EXISTS "${VCPKG_ROOT}")
    execute_process(COMMAND git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}")
    if(WIN32)
        execute_process(COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.bat" WORKING_DIRECTORY "${VCPKG_ROOT}")
    else()
        execute_process(COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.sh" WORKING_DIRECTORY "${VCPKG_ROOT}")
    endif()
endif()
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
execute_process(COMMAND "${VCPKG_ROOT}/vcpkg" install zlib mbedtls)

enable_testing()

project(Binance_Websockets)                # Specify the project's name. Append `.exe` if needed.

###################################################################################################################
set(CMAKE_CXX_STANDARD 17)                  # Set C++17 as minimum required C++ version.
set(CMAKE_CXX_STANDARD_REQUIRED ON)         # Enforce this minimum C++17 requirement.
set(CMAKE_EXPORT_COMPILE_COMMANDS on)       # Export a list of compile commands to make clang shut up about errors.

# Find the vcpkg-installed libraries.
find_package(ZLIB REQUIRED)
find_package(MbedTLS REQUIRED)

include(FetchContent)                       # Include the FetchContent CMake module.
include(GoogleTest)                         # Include GoogleTest.

# Configure IXWebSocket for TLS (required for Binance connection).
set(USE_TLS ON CACHE BOOL "" FORCE)
set(USE_MBEDTLS ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

#####################################################################################################################

FetchContent_Declare(                       # Define the content to be fetched, and from where.
  ixwebsocket                               # Content's name.
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/73a63ea05dc8ca29ec1d2c1d66481dd0de1950f1.zip
)

FetchContent_MakeAvailable(ixwebsocket)     # Fetch ixwebsocket.
FetchContent_MakeAvailable(json)            # Fetch nlohmann_json.
FetchContent_MakeAvailable(googletest)      # Fetch GoogleTest.

#####################################################################################################################

if(WIN32)
  link_libraries(bcrypt)                    # Required for Windows Cryptography/SSL support.
endif()

add_executable(
  Binance_Websockets
  src/Binance_Websockets.cpp
)                                           # add_executable(<project>, <source>)

target_link_libraries(
  Binance_Websockets
  PRIVATE nlohmann_json::nlohmann_json
  ixwebsocket
)                                          # link <project> with <library> <library>

add_executable(
  Risk_Analyzer
  src/Risk_Analyzer.cpp
)

add_executable(
  SmokeTest
  test/SmokeTest.cpp
)

target_link_libraries(
  SmokeTest
  PRIVATE gtest
  GTest::gtest_main
  nlohmann_json::nlohmann_json
  ixwebsocket
)

gtest_discover_tests(SmokeTest)
