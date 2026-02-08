############################################################
# bsw_cfg library
# Builds the OS and RTE sources as a static library that
# other targets can link against. Note: RTE references
# component mainfunctions (asw_blink_mainfunction,
# cdd_servo_mainfunction) which are resolved at link time
# when vip_system links against bsw_cfg, blink, cdd_servo.
############################################################

add_library(bsw_cfg STATIC
    os.c
    os_callout_stubs.c
    rte.c
    rte_core0.c
    rte_core1.c
)

target_include_directories(bsw_cfg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bsw_cfg PUBLIC
    freertos
    pico_stdlib
)

add_dependencies(bsw_cfg freertos)
