cmake_minimum_required(VERSION 3.12)

############################################################
# Parent CMake for programs/apps
#
# This file centralises common path setup for all `programs/apps/*`
# children. Configure and build from `code/vip/programs/buildall`:
#
# mkdir -p build && cd build
# cmake ..
# make -j4
############################################################

## --- Compute vip directory (two levels up from buildall) ----
get_filename_component(VIP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

set(PICO_SDK_PATH ${VIP_DIR}/turing/pico-sdk)
set(FREERTOS_KERNEL_PATH ${VIP_DIR}/turing/FreeRTOS-Kernel)
set(RTOS_CONFIG_PATH ${VIP_DIR}/programs/bsw_cfg/RTOS_config)

## --- Import SDK and FreeRTOS helpers -----------------------
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(vip_apps C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

## --- Parent-level targets and common configuration -------------
# Create a single FreeRTOS static library target for all apps to link
# against. This keeps child CMakeLists minimal and focused on the
# application sources. If a child build is used standalone it will
# create its own `freertos` target (child fallback).

add_library(freertos STATIC
	${FREERTOS_KERNEL_PATH}
	${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/port.c
)

target_include_directories(freertos PUBLIC
	${FREERTOS_KERNEL_PATH}/include
	${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/include
	${RTOS_CONFIG_PATH}
)

target_link_libraries(freertos
	FreeRTOS-Kernel
	FreeRTOS-Kernel-Heap3
	pico_stdlib
)

## --- Add apps (children) ------------------------------------
# Each child CMakeLists.txt should be minimal and include only
# the app sources and a `target_link_libraries(... freertos)` call.
# To add another app: add_subdirectory(../apps/<name>/src <binary-dir>)
# update other sw child cmake: add_subdirectory(../apps/<name>/src <binary-dir>)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../apps/blink ${CMAKE_CURRENT_BINARY_DIR}/blink_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../bsw_cfg ${CMAKE_CURRENT_BINARY_DIR}/bsw_cfg_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../cdd/servo ${CMAKE_CURRENT_BINARY_DIR}/cdd_servo_build)

## --- System executable using parent main and linked libraries -----
# Build a single integrated system binary that uses the parent
# `main.c` entry point and links against the bsw_cfg, blink, and
# cdd_servo libraries. All component sources are compiled in their
# respective subdirectories and linked here.

add_executable(vip_system
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/ecum.c
)

target_include_directories(vip_system PRIVATE
	${VIP_DIR}/programs/bsw_cfg
)

target_link_libraries(vip_system PRIVATE
	freertos
	pico_stdlib
	bsw_cfg
	blink
	cdd_servo
)

# Generate UF2, hex, and other output formats from the ELF file
pico_add_extra_outputs(vip_system)

install(TARGETS vip_system DESTINATION .)

## End of parent CMake
