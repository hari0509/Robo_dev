add_library(Selftest STATIC
    src/TestMain.c
    src/TestScheduler.c
)

target_include_directories(Selftest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(Selftest PUBLIC
    pico_stdlib
    freertos
    mcal_gpt
)

set_target_properties(Selftest PROPERTIES LINKER_LANGUAGE C)
