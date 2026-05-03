############################################################
# Child CMake for `apps/blink`
############################################################

## --- Application executable -----------------------------------------

add_library(blink STATIC src/asw_blink.c)

## Outputs and linking
target_link_libraries(blink PUBLIC pico_stdlib)

# Expose generated helper headers from the sibling `buildall` directory
target_include_directories(blink PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../buildall)

# End of CMakeLists
