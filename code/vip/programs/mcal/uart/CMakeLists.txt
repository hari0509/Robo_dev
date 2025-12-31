############################################################
# MCAL UART module
# Builds the MCAL UART component as a static library.
############################################################

add_library(mcal_uart STATIC
    src/mcal_uart.c
    src/mcal_uart_cfg.c
)

target_include_directories(mcal_uart PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(mcal_uart PUBLIC pico_stdlib)
