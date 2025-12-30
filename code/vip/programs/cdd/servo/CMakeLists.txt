############################################################
# cdd servo library
# Builds the CDD servo component as a static library.
############################################################

add_library(cdd_servo STATIC
    src/cdd_servo.c
)

target_include_directories(cdd_servo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cdd_servo PUBLIC pico_stdlib)
