add_library(Platform_Libs INTERFACE)

target_include_directories(Platform_Libs INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform_Libs
)

target_link_libraries(Platform_Libs INTERFACE
    pico_stdlib
    freertos
)
