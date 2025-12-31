############################################################
# Child CMake for `apps/blink`
############################################################

## --- Application executable -----------------------------------------

add_library(blink STATIC src/asw_blink.c)

## Outputs and linking
target_link_libraries(blink PUBLIC pico_stdlib)

# End of CMakeLists
