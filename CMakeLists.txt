project(example-application)
cmake_minimum_required(VERSION 3.13)

include(FetchContent)
FetchContent_Declare(
    knx-iot-stack
    GIT_REPOSITORY https://github.com/KNX-IOT/KNX-IOT-STACK.git
    GIT_TAG master
)

FetchContent_Declare(
    knx-iot-stack-gitlab
    GIT_REPOSITORY https://gitlab.knx.org/shared-projects/knx-iot-point-api-public-stack.git
    GIT_TAG master
)
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()


set(USE_GITLAB OFF CACHE BOOL "use gitlab as source for KNX IoT Stack")

if(USE_GITLAB)
    if(NOT knx-iot-stack_POPULATED)
        message(STATUS "Fetching KNX-IoT Source Code, please wait...")
        FetchContent_Populate(knx-iot-stack-gitlab)
        add_subdirectory(${knx-iot-stack-gitlab_SOURCE_DIR} ${knx-iot-stack-gitlab_BINARY_DIR})
    endif()
else()
    if(NOT knx-iot-stack_POPULATED)
        message(STATUS "Fetching KNX-IoT Source Code, please wait...")
        FetchContent_Populate(knx-iot-stack)
        add_subdirectory(${knx-iot-stack_SOURCE_DIR} ${knx-iot-stack_BINARY_DIR})
    endif()
endif()

add_executable(example_sensor
	${PROJECT_SOURCE_DIR}/example_sensor.c
)

target_link_libraries(example_sensor kisClientServer)

