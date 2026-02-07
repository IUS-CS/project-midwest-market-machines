cmake_minimum_required(VERSION 3.23)
project(api_demo)

include(FetchContent)
FetchContent_Declare(
  ixwebsocket
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
)
FetchContent_MakeAvailable(ixwebsocket)

add_executable(api_demo api_demo.cpp)

target_link_libraries(api_demo PRIVATE ixwebsocket)
