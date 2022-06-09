# Example KNX Application
<!-- TOC -->

- [Example KNX Application](#example-knx-application)
  - [Introduction](#introduction)
  - [Prerequisites](#prerequisites)
  - [Build instructions](#build-instructions)
    - [Linux](#linux)
    - [Windows](#windows)
  - [Extending the example application](#extending-the-example-application)

<!-- /TOC -->

## Introduction

This repository contains a barebones example of what you need to do in order to
build an application on top of the open-source [KNX IoT Point API stack](https://github.com/KNX-IOT/KNX-IOT-STACK).

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

Please make sure that you have a working Visual Studio build system before
proceeding with the following steps.


In Powershell, type the following commands:
```powershell
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
```

This will generate several `.vcxproj`. You should then open `ALL_BUILD.vcxproj`
within Visual Studio and build the targets as usual, by selecting Build -> Build Solution
within the Visual Studio menu bar.

Note: more info at [the KNX-IoT guide on building for Windows](https://knx-iot.github.io/building_windows/).


## Extending the example application

The CMake build system is made aware of the example application thanks to the
call to `add_executable` inside CMakeLists.txt. If you want to add further
executables, you must add new calls to `add_executable` and point them to a
different .c file.

The call to `target_link_libraries` is what brings in the KNX-IoT stack in.
It instructs the build system to link against a static library containing
all KNX-IoT functionality, and also lets you `#include` KNX headers within
the executable that links to this library.
