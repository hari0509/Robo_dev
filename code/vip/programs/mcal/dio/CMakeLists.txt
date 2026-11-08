############################################################
# MCAL DIO (AUTOSAR DIO Driver) module
# Builds the MCAL DIO component as a static library on top of
# the Pico SDK hardware_gpio.
############################################################

add_library(mcal_dio STATIC
    src/mcal_dio.c
    src/mcal_dio_cfg.c
)

target_include_directories(mcal_dio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(mcal_dio PUBLIC
    pico_stdlib
    hardware_gpio
    Platform_Libs
)
