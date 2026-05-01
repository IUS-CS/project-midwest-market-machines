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

if(WIN32)
  add_compile_definitions(_HAS_STD_BYTE=0)
  link_libraries(bcrypt ws2_32 crypt32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}>")
  set(X_VCPKG_APPLOCAL_DEPS ON)

  # Auto vcpkg setup: clones, bootstraps, needs to load before project() call
  set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")
  if(NOT EXISTS "${VCPKG_ROOT}")
    execute_process(COMMAND git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}")
    execute_process(COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.bat" WORKING_DIRECTORY "${VCPKG_ROOT}")
  endif()

  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
  execute_process(COMMAND "${VCPKG_ROOT}/vcpkg" install zlib mbedtls)

  set(NPM_CMD npm.cmd)                      # Alias `npm.cmd` to `NPM_CMD`. `npm` needs `.cmd` on WIN32.
else()
  set(NPM_CMD npm)                          # Other systems do not need `npm.cmd`, where `npm` will suffice.
endif()

project(Binance_Websockets)                # Specify the project's name. Append `.exe` if needed.

###################################################################################################################
set(CMAKE_CXX_STANDARD 17)                          # Set C++17 as minimum required C++ version.
set(CMAKE_CXX_STANDARD_REQUIRED ON)                 # Enforce this minimum C++17 requirement.
set(CMAKE_EXPORT_COMPILE_COMMANDS on)               # Export a list of compile commands to make clang shut up about errors.
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build examples for zlib.
set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)        # Don't build programs.
set(USE_TLS ON CACHE BOOL "" FORCE)                 # TLS required for WebSockets.
set(USE_MBEDTLS ON CACHE BOOL "" FORCE)             # Tell IXWebSocket we WILL use MBedTLS.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # Tell GTest to use a shared version of runtime libraries.
set(USE_TEST ON CACHE BOOL "" FORCE)                # Enable tests for IXWebSocket.
set(WEBVIEW_BUILD_TESTS ON CACHE BOOL "" FORCE)     # Enable tests for webview.
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)        # Do *not* build nlohmann_json's test suite (buggy - tried it already).

ENABLE_TESTING()
include(FetchContent)
include(GoogleTest)

#####################################################################################################################

FetchContent_Declare(                       # Define the content to be fetched, and from where.
  ixwebsocket                               # Content's name.
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
  FIND_PACKAGE_ARGS NAMES ixwebsocket IXWebSocket
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  FIND_PACKAGE_ARGS NAMES nlohmann_json nlohmann::json nlohmann_json::nlohmann_json
)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  FIND_PACKAGE_ARGS NAMES gtest GoogleTest gmock GMock
)

FetchContent_Declare(
  webview
  GIT_REPOSITORY https://github.com/webview/webview.git
  FIND_PACKAGE_ARGS NAMES webview WebView
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

FetchContent_MakeAvailable(zlib)            # Fetch zlib.
FetchContent_MakeAvailable(mbedtls)         # Fetch mbedtls.
FetchContent_MakeAvailable(ixwebsocket)     # Fetch ixwebsocket.
FetchContent_MakeAvailable(json)            # Fetch nlohmann_json.
FetchContent_MakeAvailable(googletest)      # Fetch googletestt.
FetchContent_MakeAvailable(webview)         # Fetch webview.
#####################################################################################################################

add_custom_target(
  npm_build
  COMMAND ${NPM_CMD} run build
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/MarketUI"
  VERBATIM
)

add_executable(
  Binance_Websockets
  src/Binance_Websockets.cpp
)                                           # add_executable(<project>, <source>)

add_dependencies(
  Binance_Websockets
  npm_build
)

target_link_libraries(
  Binance_Websockets
  PRIVATE nlohmann_json::nlohmann_json
  ixwebsocket
  webview::core
)                                           # link <project> with <library> <library>

######################################################################################################################

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

add_executable(
  ExchangeClientTest
  test/ExchangeClientTest.cpp
)

add_executable(
  DatabaseHandlerTest
  test/DatabaseHandlerTest.cpp
)

target_link_libraries(
  DatabaseHandlerTest
  PRIVATE
  gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  nlohmann_json::nlohmann_json
  ixwebsocket
)

if(WIN32)
  target_link_libraries(
  ExchangeClientTest
  PRIVATE
  gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  nlohmann_json::nlohmann_json
  ixwebsocket
  bcrypt
)
else()
target_link_libraries(
  ExchangeClientTest
  PRIVATE
  gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  nlohmann_json::nlohmann_json
  ixwebsocket
)
endif()

target_include_directories(
  ExchangeClientTest
  PRIVATE
  src
)

target_include_directories(
  DatabaseHandlerTest
  PRIVATE
  src
)

gtest_discover_tests(ExchangeClientTest)
gtest_discover_tests(SmokeTest)
gtest_discover_tests(DatabaseHandlerTest)
