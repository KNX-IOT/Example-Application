project(example-application)
cmake_minimum_required(VERSION 3.13)

include(FetchContent)

FetchContent_Declare(
    knx-iot-stack
    GIT_REPOSITORY https://github.com/KNX-IOT/KNX-IOT-STACK.git
    GIT_TAG master
)

if(NOT knx-iot-stack_POPULATED)
	FetchContent_Populate(knx-iot-stack)
	add_subdirectory(${knx-iot-stack_SOURCE_DIR} ${knx-iot-stack_BINARY_DIR})
endif()

