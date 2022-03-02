# Example KNX Application

This repository contains a barebones example of what you need to do in order to
build an application on top of the open-source KNX-IoT stack.

## Prerequisites

In order to build this example project, you will need the following to be installed
on your system:
- C compiler (GCC on Linux, Visual Studio on Windows)
- CMake
- A generator that CMake can use - make on Linux, Visual Studio on Windows.

## Build instructions

### Linux

```bash
# Download this repository
git clone https://github.com/KNX-IOT/Example-Application.git
# Change directory to the newly created dir
cd Example-Application
# Create build directory & go there
mkdir build
cd build/
# Generate CMake build system. "../" is the path to the previous
# directory, which contains the CMakeLists.txt script.
cmake ../
# Build & run the example application
make example_sensor
./example_sensor
```

### Windows

Building the example application with Visual Studio is identical to building
the KNX-IoT stack, the only difference being that you must perform the steps
on a fresh clone of the Example Application repository instead.

Refer to [the KNX-IoT guide on building for Windows]() for detailed steps.
During Step 1, make sure you do `git clone https://github.com/KNX-IOT/Example-Application.git`
instead of cloning the KNX-IoT stack, and then follow the rest of the guide.

## Extending the example application

The CMake build system is made aware of the example application thanks to the
call to `add_executable` inside CMakeLists.txt. If you want to add further
executables, you must add new calls to `add_executable` and point them to a
different .c file.

The call to `target_link_libraries` is what brings in the KNX-IoT stack in.
It instructs the build system to link against a static library containing
all KNX-IoT functionality, and also lets you `#include` KNX headers within
the executable that links to this library.

