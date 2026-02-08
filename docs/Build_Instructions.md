# Rationale
The api_demo.cpp application requires a few dependencies. These dependencies in turn require the use of CMake.

CMake is an open-source build system generator. It handles cross-platform builds, system introspection, and user customized builds. The goal of CMake is to allow multiple developers to build the application from the source code on multiple different computers.

It was not my intention nor my goal to include CMake in this project, as I had never heard of it. As noted earlier, the prototype dependencies rely on CMake as a build system. In an effort to complete the prototype as fast as possible, I opted to just stick to their documentation and use CMake.

You may find more information on CMake at [CMake's website](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Why%20CMake.html)

# Building with CMake
The inclusion of CMake in this project does complicate the build process slightly. Once you get use to using CMake, it's as easy as pie.

So frett not, this will all be over soon:

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
1. Navigate to the `/src` directory.
2. Run the following commands:

```
mkdir build           // Create a directory for build artifacts.
cmake -DUSE_TLS=1 ..  // Enable TLS support.
cmake -B /src/build   // Specify *where* to build to.
cmake -B build        // Build the application.
./api_demo            // Run the application.
```

![TIP] When changing the source code, you may run `make` to rebuild and recompile the appliction. No neeed to do all of these steps each time 

More information is available via [CMake's Getting Started Page](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html)
## Maintaining CMake
An important callout is that CMake requires maintenance. Unlike Java, where we may call `import Java.utils.scanner;`, wherein Java handles getting that package/library and linking it to the source code, C++ does not do that for us.
Rather, this role is fulfilled by CMake.

CMake knows what you want it to do (link what to what?) via the `CMakeLists.txt` document. This .txt file is *required* to use CMake. Without it, CMake will fail out.

CMake uses *its own langauge* in its configuration file. More information is available on the CMake website via the links above.
Please check out `CMakeLists.txt` when you get a chance. If for any reason a package or library fails to be included when building the application, it's most likely that the `CMakeLists.txt` file has fallen into a state of disrepair.

[Writing CMakeLists Files](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Writing%20CMakeLists%20Files.html)
