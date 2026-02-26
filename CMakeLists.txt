###################################################################################################################
# References:
# CMake's Website:
# https://cmake.org/cmake/help/book/mastering-cmake/index.html
# https://cmake.org/cmake/help/latest/module/GoogleTest.html
# 
# GoogleTest's GitHub page:
# https://google.github.io/googletest/quickstart-cmake.html
###################################################################################################################

cmake_minimum_required(VERSION 3.5)         # Specify the minimum required cmake version for this project.
project(Binance_Websockets)                 # Specify the project's name. Append `.exe` if needed.

enable_testing()

set(CMAKE_CXX_STANDARD 17)                  # Set C++17 as minimum required C++ version.
set(CMAKE_CXX_STANDARD_REQUIRED ON)         # Enforce this minimum C++17 requirement.
set(CMAKE_EXPORT_COMPILE_COMMANDS on)       # Export a list of compile commands to make clang shut up about errors.

include(FetchContent)                       # Include the FetchContent CMake module.
include(GoogleTest)                         # Include GoogleTest

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
  DOWNLOAD_EXTRACT_TIMESTAMP NEW
)

FetchContent_MakeAvailable(ixwebsocket)     # Fetch ixwebsocket.
FetchContent_MakeAvailable(json)            # Fetch nlohmann_json.
FetchContent_MakeAvailable(googletest)      # Fetch GoogleTest.

#####################################################################################################################

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
