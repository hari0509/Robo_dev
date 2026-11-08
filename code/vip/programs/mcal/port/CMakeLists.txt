############################################################
# MCAL PORT (AUTOSAR Port Driver) module
# Builds the MCAL PORT component as a static library on top of
# the Pico SDK hardware_gpio.
############################################################

add_library(mcal_port STATIC
    src/mcal_port.c
    src/mcal_port_cfg.c
)

target_include_directories(mcal_port PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(mcal_port PUBLIC
    pico_stdlib
    hardware_gpio
    Platform_Libs
)
