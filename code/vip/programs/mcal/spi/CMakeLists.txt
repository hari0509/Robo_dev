############################################################
# MCAL SPI (AUTOSAR SPI Handler/Driver) module
# Builds the MCAL SPI component as a static library on top of
# the Pico SDK hardware_spi.
############################################################

add_library(mcal_spi STATIC
    src/mcal_spi.c
    src/mcal_spi_cfg.c
)

target_include_directories(mcal_spi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(mcal_spi PUBLIC
    pico_stdlib
    pico_sync
    hardware_spi
    hardware_gpio
    mcal_dio
    Platform_Libs
)