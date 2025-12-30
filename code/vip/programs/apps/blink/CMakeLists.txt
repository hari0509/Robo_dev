############################################################
# Child CMake for `apps/blink`
#
# Intended to be included by `programs/buildall/CMakeLists.txt`.
# Parent CMake provides all SDK paths and the `freertos` target.
############################################################

## --- Application executable -----------------------------------------

add_library(blink STATIC src/asw_blink.c)

## Outputs and linking
# Provide the blink library which can be linked into parent binaries.
target_link_libraries(blink PUBLIC pico_stdlib freertos)

# End of CMakeLists
