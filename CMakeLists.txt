###################################################################################################################
# References:
# CMake's Website:
# https://cmake.org/cmake/help/book/mastering-cmake/index.html
# https://cmake.org/cmake/help/latest/module/GoogleTest.html
# 
# GoogleTest's GitHub page:
# https://google.github.io/googletest/quickstart-cmake.html
###################################################################################################################

cmake_minimum_required(VERSION 3.24)       # 3.24 req'd for find and fetch logic.

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

#if(WIN32)
#  # Auto vcpkg setup: clones, bootstraps, needs to load before project() call
#  set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")
#  if(NOT EXISTS "${VCPKG_ROOT}")
#    execute_process(COMMAND git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}")
#    execute_process(COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.bat" WORKING_DIRECTORY "${VCPKG_ROOT}")
#  endif()

#  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
#  execute_process(COMMAND "${VCPKG_ROOT}/vcpkg" install zlib mbedtls)
#endif()

project(Binance_Websockets)                # Specify the project's name. Append `.exe` if needed.

###################################################################################################################
set(CMAKE_CXX_STANDARD 17)                          # Set C++17 as minimum required C++ version.
set(CMAKE_CXX_STANDARD_REQUIRED ON)                 # Enforce this minimum C++17 requirement.
set(CMAKE_EXPORT_COMPILE_COMMANDS on)               # Export a list of compile commands to make clang shut up about errors.
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build examples for zlib.
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)         # Don't build tests included in dependency repositories.
set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)        # Don't build programs.
set(USE_TLS ON CACHE BOOL "" FORCE)                 # TLS required for WebSockets.
set(USE_MBEDTLS ON CACHE BOOL "" FORCE)             # 
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  #

include(FetchContent)
include(GoogleTest)

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
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

FetchContent_Declare(
  webview
  GIT_REPOSITORY https://github.com/webview/webview.git
)

FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG v1.3.2
  FIND_PACKAGE_ARGS NAMES ZLIB              # Try to find ZLIB before fetching it.
)

FetchContent_Declare(
  mbedtls
  GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git
  GIT_TAG mbedtls-3.6.5
  FIND_PACKAGE_ARGS NAMES MBedTLS           # Try to find MBedTLS before fetching it.
)

FetchContent_MakeAvailable(ixwebsocket)     # Fetch ixwebsocket.
FetchContent_MakeAvailable(json)            # Fetch nlohmann_json.
FetchContent_MakeAvailable(googletest)      # Fetch googletestt.
FetchContent_MakeAvailable(webview)         # Fetch webview.
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
  webview::core
)                                           # link <project> with <library> <library>

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
