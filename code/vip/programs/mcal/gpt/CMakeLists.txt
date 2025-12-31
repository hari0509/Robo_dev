############################################################
# MCAL GPT (General Purpose Timer) module
# Builds the MCAL GPT component as a static library.
############################################################

add_library(mcal_gpt STATIC
    src/mcal_gpt.c
    src/mcal_gpt_cfg.c
)

target_include_directories(mcal_gpt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(mcal_gpt PUBLIC pico_stdlib)