cmake_minimum_required(VERSION 3.5)         # Specify the minimum required cmake version for this project.
project(api_demo)                           # Specify the project's name. Append `.exe` if needed.

include(FetchContent)                       # Include the FetchContent CMake module.
FetchContent_Declare(                       # Define the content to be fetched, and from where.
  ixwebsocket                               # Content's name.
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
)
FetchContent_MakeAvailable(ixwebsocket)     # Fetch the content.

include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
) 
FetchContent_MakeAvailable(json)

add_executable(api_demo src/api_demo.cpp)       # add_executable(<project>, <source>)

target_link_libraries(api_demo PRIVATE nlohmann_json::nlohmann_json)  # link <project> with <library>
target_link_libraries(api_demo PRIVATE ixwebsocket)                   # link <project> with <library>
