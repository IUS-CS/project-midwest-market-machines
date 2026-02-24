[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
# Simple Trade

Simple Trade is an AI-assisted trading application for major cryptocurrencies and stocks. This application runs entirely on-device. 

## Installation


### Dependencies
Simple Trade requires that you have [CMake](https://cmake.org/download/) installed.

To check if CMake is installed, open your operating system's terminal and type `cmake --version`. If the command isn't recognized, you can download it [here](https://cmake.org/download/).

### Building
*A more in-depth verison of the building section can be found in `docs/Build_Instructions.md`*
1. Navigate to the `build/` directory for the project.
2. Run the following commands:

```
cmake -DUSE_TLS=1 ..  // Enable TLS support.
cmake ..              // Build the application using ../CMakeLists.txt
./api_demo            // Run the application.
```

> [!TIP]
> When changing the source code, you may run `make` from the `build/` directory to rebuild and recompile the application. No need to do all of these steps each time 

More information is available via [CMake's Getting Started Page](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html).



## Example Run


![Terminal_With_Text](https://raw.githubusercontent.com/IUS-CS/project-midwest-market-machines/refs/heads/main/docs/Pasted%20image.png)


