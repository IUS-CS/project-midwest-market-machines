# Rationale
The api_demo.cpp application requires a few dependencies. These dependencies in turn require the use of CMake.

CMake is an open-source build system generator. It handles cross-platform builds, system introspection, and user customized builds. The goal of CMake is to allow multiple developers to build the application from the source code on multiple different computers.

You may find more information on CMake at [CMake's website](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Why%20CMake.html)

# Building with CMake
Once you get used to using CMake, it's as easy as pie.
It's much easier to use than calling `g++ ...` for every build, but does require a *little* fiddling every now and then.

## Ensure CMake is installed
Check if you have CMake by running `cmake --version`. If you have CMake, you will receive a message similar to the following:

```
Midwest Market Machines/src prototyping ? ❯ cmake --version
cmake version 4.2.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

Else, you will not receive this message.
Follow the directions on [CMake's Downloads Page](https://cmake.org/download/). Otherwise... it's very popular, so you may find it in your package manager under `cmake`. At least it's in the package manager for Arch.

## How to build the appliction using CMake
1. Navigate to the `build/` directory for the project.
2. Run the following commands:

```
cmake -DUSE_TLS=1 ..  // Enable TLS support.
cmake ..              // Build the application using ../CMakeLists.txt
./api_demo            // Run the application.
```

> [!TIP]
> When changing the source code, you may run `make` from the `build/` directory to rebuild and recompile the application. No need to do all of these steps each time 

More information is available via [CMake's Getting Started Page](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html)
## Maintaining CMake
An important callout is that CMake requires maintenance. Unlike Java, where we may call `import Java.utils.scanner;`, wherein Java handles getting that package/library and linking it to the source code, C++ does not do that for us.
Rather, this role is fulfilled by CMake.

CMake knows what you want it to do (link what to what?) via the `CMakeLists.txt` document. This .txt file is *required* to use CMake. Without it, CMake will fail out.

CMake uses *its own langauge* in its configuration file. More information is available on the CMake website via the links above.
Please check out `CMakeLists.txt` when you get a chance. If for any reason a package or library fails to be included when building the application, it's most likely that the `CMakeLists.txt` file has fallen into a state of disrepair.

[Writing CMakeLists Files](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Writing%20CMakeLists%20Files.html)
