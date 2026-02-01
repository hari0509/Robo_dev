add_library(sys_nvm STATIC
    src/sys_nvm.c
)

target_include_directories(sys_nvm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(sys_nvm PUBLIC
    pico_stdlib
    freertos
)

set_target_properties(sys_nvm PROPERTIES LINKER_LANGUAGE C)
