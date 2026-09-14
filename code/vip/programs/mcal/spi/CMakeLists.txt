############################################################
# MCAL GPT (General Purpose Timer) module
# Builds the MCAL GPT component as a static library.
############################################################

add_library(mcal_spi STATIC
    src/mcal_spi.c
    src/mcal_spi_cfg.c
)

target_include_directories(mcal_spi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(mcal_spi PUBLIC pico_stdlib)