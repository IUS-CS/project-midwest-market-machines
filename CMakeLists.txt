cmake_minimum_required(VERSION 3.5)
project(api_demo)

include(FetchContent)
FetchContent_Declare(
  ixwebsocket
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
)
FetchContent_MakeAvailable(ixwebsocket)

include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
) 
FetchContent_MakeAvailable(json)

add_executable(api_demo api_demo.cpp)

target_link_libraries(api_demo PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(api_demo PRIVATE ixwebsocket)
