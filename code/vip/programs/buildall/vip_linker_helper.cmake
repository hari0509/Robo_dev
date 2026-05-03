# =============================================================================
#
# Usage in buildall/CMakeLists.txt:
#
#   include(${CMAKE_CURRENT_LIST_DIR}/cmake/genesis_linker.cmake)
#
#   add_executable(blink blink/main.c)
#   target_link_libraries(blink pico_stdlib FreeRTOS-Kernel ...)
#   genesis_apply_linker(blink)
#   pico_add_extra_outputs(blink)
#
# Optional per-target overrides (set BEFORE calling genesis_apply_linker):
#
#   set_target_properties(blink PROPERTIES
#       GENESIS_HEAP_SIZE        65536    # 64 KB heap
#       GENESIS_CORE0_STACK_SIZE 8192     # 8 KB stack
#       GENESIS_CORE1_STACK_SIZE 4096
#   )
# =============================================================================

cmake_minimum_required(VERSION 3.17)

# Allow disabling linker GC so we can produce an un-pruned/original map file
option(GENESIS_ENABLE_GC "Enable linker --gc-sections (default ON)" OFF)

# Path to our custom linker script (relative to this .cmake file's directory)
set(GENESIS_LINKER_SCRIPT
    "${CMAKE_CURRENT_LIST_DIR}/vip_linker_directive.ld"
    CACHE FILEPATH "Genesis RP2040 custom linker script"
)

if(NOT EXISTS "${GENESIS_LINKER_SCRIPT}")
    message(FATAL_ERROR
        "genesis_linker.cmake: linker script not found at:\n"
        "  ${GENESIS_LINKER_SCRIPT}\n"
        "Ensure linker/vip_linker_directive.ld is present in the buildall tree."
    )
endif()

# -----------------------------------------------------------------------------
# genesis_apply_linker(<target>)
#
# Overrides the Pico-SDK default linker script with vip_linker_directive.ld and
# configures diagnostic outputs (.map file).
# -----------------------------------------------------------------------------
function(genesis_apply_linker TARGET)

    # ---- 1. Override Pico-SDK linker script ---------------------------------
    # pico_set_linker_script sets the PICO_TARGET_LINKER_SCRIPT property which
    # pico_standard_link reads during link options generation.
    pico_set_linker_script(${TARGET} ${GENESIS_LINKER_SCRIPT})

    # ---- 2. Declare a build dependency so CMake re-links on script changes --
    set_property(TARGET ${TARGET} APPEND PROPERTY LINK_DEPENDS
        "${GENESIS_LINKER_SCRIPT}"
    )

    # ---- 3. Emit a linker map file next to the ELF --------------------------
    # Basic link map + cross-ref + memory usage
    target_link_options(${TARGET} PRIVATE
        -Wl,-Map=$<TARGET_FILE_DIR:${TARGET}>/$<TARGET_FILE_BASE_NAME:${TARGET}>.map
        -Wl,--cref
        -Wl,--print-memory-usage
    )

    # Optionally enable GC of unused sections (default ON). Set
    # `-DGENESIS_ENABLE_GC=OFF` at configure time to keep all sections
    # and generate an "original" (pre-GC) map file.
    if(GENESIS_ENABLE_GC)
        target_link_options(${TARGET} PRIVATE -Wl,--gc-sections)
    endif()

    # ---- 4. Per-target memory sizes (read from target properties) -----------
    # Defaults match the linker script #defines; override via set_target_properties.

    get_target_property(_heap   ${TARGET} GENESIS_HEAP_SIZE)
    get_target_property(_stack0 ${TARGET} GENESIS_CORE0_STACK_SIZE)
    get_target_property(_stack1 ${TARGET} GENESIS_CORE1_STACK_SIZE)

    if(NOT _heap)
        set(_heap 49152)    # 48 KB default
    endif()
    if(NOT _stack0)
        set(_stack0 4096)   # 4 KB default
    endif()
    if(NOT _stack1)
        set(_stack1 4096)   # 4 KB default
    endif()

    target_compile_definitions(${TARGET} PRIVATE
        GENESIS_HEAP_SIZE=${_heap}
        GENESIS_CORE0_STACK_SIZE=${_stack0}
        GENESIS_CORE1_STACK_SIZE=${_stack1}
        # Expose the same symbols to C so FreeRTOSConfig.h can cross-check
        configTOTAL_HEAP_SIZE=${_heap}
    )

    # Also pass the values as linker --defsym so the .ld preprocessor macros
    # can be overridden from CMake without editing the script.
    target_link_options(${TARGET} PRIVATE
        -Wl,--defsym=GENESIS_HEAP_SIZE=${_heap}
        -Wl,--defsym=GENESIS_CORE0_STACK_SIZE=${_stack0}
        -Wl,--defsym=GENESIS_CORE1_STACK_SIZE=${_stack1}
    )

    # ---- 5. Log -------------------------------------------------------
    message(STATUS
        "[Genesis] ${TARGET}: custom linker applied "
        "(heap=${_heap} B, stack0=${_stack0} B, stack1=${_stack1} B)"
    )

endfunction()

# -----------------------------------------------------------------------------
# genesis_apply_linker_all()
#
# Convenience: apply the linker to every executable in the current directory.
# Call AFTER all add_executable() calls.
# -----------------------------------------------------------------------------
function(genesis_apply_linker_all)
    get_property(_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_t IN LISTS _targets)
        get_target_property(_type ${_t} TYPE)
        if(_type STREQUAL "EXECUTABLE")
            genesis_apply_linker(${_t})
        endif()
    endforeach()
endfunction()
