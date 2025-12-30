############################################################
# Child CMake for `apps/blink`
############################################################

# Standalone fallback: compute paths and initialize SDK if the
# parent build hasn't already configured them.
if(NOT DEFINED PICO_SDK_PATH)
  # from this directory (blink) go up to vip: ../../..
  get_filename_component(VIP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
  set(PICO_SDK_PATH ${VIP_DIR}/turing/pico-sdk)
  set(FREERTOS_KERNEL_PATH ${VIP_DIR}/turing/FreeRTOS-Kernel)
  set(RTOS_CONFIG_PATH ${VIP_DIR}/programs/bsw_cfg/RTOS_config)

  include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
  include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

  project(blink C CXX ASM)
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_CXX_STANDARD 17)

  pico_sdk_init()
endif()

## --- Application executable (minimal child) -----------------------
# The parent CMake will provide a `freertos` target when building from
# the `buildall` parent. When building this directory standalone the
# fallback above creates `freertos` itself.

add_executable(blink src/blink.c)

## Outputs and linking
# Link against the common `freertos` library and `pico_stdlib`.
target_link_libraries(blink pico_stdlib freertos)

pico_add_extra_outputs(blink)

# End of CMakeLists
