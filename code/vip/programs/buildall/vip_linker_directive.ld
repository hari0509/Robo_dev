
/* 1. Pull SDK linker */
INCLUDE "pico_flash_region.ld"

/* 2. ---- Memory regions -------------------------------------------------------- */

#ifndef GENESIS_SRAM_ORIGIN
#define GENESIS_SRAM_ORIGIN     0x20000000
#endif
#ifndef GENESIS_SRAM_SIZE
#define GENESIS_SRAM_SIZE       256K
#endif

#define GENESIS_SCRATCH_X_ORIGIN 0x20040000
#define GENESIS_SCRATCH_X_SIZE   16K
#define GENESIS_SCRATCH_Y_ORIGIN 0x20044000
#define GENESIS_SCRATCH_Y_SIZE   16K

MEMORY
{
    FLASH   (rx)  : ORIGIN = GENESIS_FLASH_ORIGIN,   LENGTH = GENESIS_FLASH_SIZE
    RAM     (rwx) : ORIGIN = GENESIS_SRAM_ORIGIN,    LENGTH = GENESIS_SRAM_SIZE
    SCRATCH_X(rwx): ORIGIN = GENESIS_SCRATCH_X_ORIGIN, LENGTH = GENESIS_SCRATCH_X_SIZE
    SCRATCH_Y(rwx): ORIGIN = GENESIS_SCRATCH_Y_ORIGIN, LENGTH = GENESIS_SCRATCH_Y_SIZE
}

/* 3. ---- Entry point ---------------------------------------------------------- */
/* Tells the linker that _entry_point is the very first instruction to execute after reset. */
ENTRY(_entry_point)

/* 4. ---- Output sections ------------------------------------------------------ */
SECTIONS
{
    /* ------------------------------------------------------------------ */
    /* 1.  BOOT2 — 256-byte second-stage bootloader (mandatory for RP2040) */
    /* ------------------------------------------------------------------ */
    .boot2 : {
        __boot2_start__ = .;
        KEEP(*(.boot2))
        __boot2_end__ = .;
    } > FLASH

    ASSERT(__boot2_end__ - __boot2_start__ == 256,
           "ERROR: .boot2 must be exactly 256 bytes")

    /* ------------------------------------------------------------------ */
    /* 2.  Vector table + startup                                          */
    /* ------------------------------------------------------------------ */
    .vectors : {
        __vectors_start__ = .;
        KEEP(*(.vectors))
        __vectors_end__ = .;
    } > FLASH

    /* ------------------------------------------------------------------ */
    /* 3.  Binary info (Pico-SDK metadata block)                          */
    /* ------------------------------------------------------------------ */
    .binary_info_header : {
        KEEP(*(.binary_info_header))
    } > FLASH
    .addr_location_of_binary_info : {
        KEEP(*(.addr_location_of_binary_info))
    } > FLASH

    /* ------------------------------------------------------------------ */
    /* 4.  OS / Kernel code  (FreeRTOS core + port layer)                 */
    /*     Placed first so it lands in the lowest flash addresses for      */
    /*     potential MPU region alignment.                                 */
    /* ------------------------------------------------------------------ */
    .text.kernel : {
        __kernel_flash_start__ = .;
        /* FreeRTOS scheduler, queue, task, timer, event group */
        *tasks.c.obj*(.text* .rodata*)
        *queue.c.obj*(.text* .rodata*)
        *timers.c.obj*(.text* .rodata*)
        *event_groups.c.obj*(.text* .rodata*)
        *stream_buffer.c.obj*(.text* .rodata*)
        /* RP2040 port layer */
        *port.c.obj*(.text* .rodata*)
        *portASM.S.obj*(.text*)
        __kernel_flash_end__ = .;
    } > FLASH

    /* ------------------------------------------------------------------ */
    /* 5.  Application code                                               */
    /* ------------------------------------------------------------------ */
    .text : {
        __app_flash_start__ = .;
        *(.text*)                   /* All other .text sections           */
        . = ALIGN(4);
        *(.rodata*)                 /* Read-only data (strings, consts)   */
        . = ALIGN(4);
        __app_flash_end__ = .;
    } > FLASH

    /* ------------------------------------------------------------------ */
    /* 6.  C++ init / fini arrays                                         */
    /* ------------------------------------------------------------------ */
    .init_array : {
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    } > FLASH

    .fini_array : {
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
    } > FLASH

    /* ------------------------------------------------------------------ */
    /* 7.  ARM exception unwind tables                                    */
    /* ------------------------------------------------------------------ */
    .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH
    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH

    /* ------------------------------------------------------------------ */
    /* 8.  Pico-SDK binary_info section                                   */
    /* ------------------------------------------------------------------ */
    .binary_info : {
        __binary_info_start = .;
        KEEP(*(.binary_info.keep.*))
        *(.binary_info.*)
        __binary_info_end = .;
    } > FLASH

    /* ------------------------------------------------------------------ */
    /* 9.  Initialised data — LMA in Flash, VMA in SRAM                  */
    /* ------------------------------------------------------------------ */
    .data : {
        __data_start__ = .;
        *(vtable)
        *(.time_critical*)          /* Explicitly time-critical functions */
        *(.data*)
        . = ALIGN(4);
        *(.after_data.*)
        . = ALIGN(4);
        /* Pico-SDK mutex/spinlock init tables */
        __mutex_array_start = .;
        KEEP(*(.mutex_array))
        __mutex_array_end = .;
        . = ALIGN(4);
        __data_end__ = .;
    } > RAM AT> FLASH

    /* LMA pointer — startup copies [__data_start__, __data_end__) from here */
    __data_load__ = LOADADDR(.data);

    /* ------------------------------------------------------------------ */
    /* 10. BSS — zero-initialised at startup                              */
    /* ------------------------------------------------------------------ */
    .bss : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    /* ------------------------------------------------------------------ */
    /* 11. NOINIT — NOT zero-initialised; survives warm reboot            */
    /*     Use for: safe-state buffers, diagnostic counters, crash logs    */
    /*     Mark variables with: __attribute__((section(".noinit")))        */
    /* ------------------------------------------------------------------ */
    .noinit (NOLOAD) : {
        . = ALIGN(4);
        __noinit_start__ = .;
        *(.noinit*)
        . = ALIGN(4);
        __noinit_end__ = .;
    } > RAM

    /* ------------------------------------------------------------------ */
    /* 12. Heap — grows upward from here                                  */
    /*     configTOTAL_HEAP_SIZE in FreeRTOSConfig.h must not exceed      */
    /*     (__heap_end__ - __heap_start__).                                */
    /* ------------------------------------------------------------------ */
    .heap (NOLOAD) : {
        . = ALIGN(8);
        __heap_start__ = .;
        . += GENESIS_HEAP_SIZE;     /* default 48 KB — override in CMake  */
        __heap_end__ = .;
    } > RAM

    /* ------------------------------------------------------------------ */
    /* 13. Core 0 stack — grows downward toward heap end                  */
    /* ------------------------------------------------------------------ */
    .stack0 (NOLOAD) : {
        . = ALIGN(32);
        __stack0_bottom__ = .;
        . += GENESIS_CORE0_STACK_SIZE;
        __stack0_top__ = .;         /* Used as initial SP for core 0       */
    } > RAM

    /* ------------------------------------------------------------------ */
    /* 14. Core 1 stack                                                   */
    /* ------------------------------------------------------------------ */
    .stack1 (NOLOAD) : {
        . = ALIGN(32);
        __stack1_bottom__ = .;
        . += GENESIS_CORE1_STACK_SIZE;
        __stack1_top__ = .;         /* Used as initial SP for core 1       */
    } > RAM

    /* ---- SRAM scratch banks --------------------------------------------- */

    /* ------------------------------------------------------------------ */
    /* 15. scratch_x — FreeRTOS ISR / scheduler hot path                  */
    /*     Functions in this section execute from single-cycle SRAM.      */
    /*     Mark with: __attribute__((section(".scratch_x.<name>")))       */
    /* ------------------------------------------------------------------ */
    .scratch_x : {
        __scratch_x_start__ = .;
        *(.scratch_x.*)
        . = ALIGN(4);
        __scratch_x_end__ = .;
    } > SCRATCH_X AT > FLASH

    __scratch_x_source__ = LOADADDR(.scratch_x);

    /* ------------------------------------------------------------------ */
    /* 16. scratch_y — DMA descriptors / PIO state machine buffers        */
    /* ------------------------------------------------------------------ */
    .scratch_y : {
        __scratch_y_start__ = .;
        *(.scratch_y.*)
        . = ALIGN(4);
        __scratch_y_end__ = .;
    } > SCRATCH_Y AT > FLASH

    __scratch_y_source__ = LOADADDR(.scratch_y);

    /* ---- Assertions ------------------------------------------------------- */

    /* Ensure main SRAM is not over-committed */
    ASSERT((__stack1_top__) <= (GENESIS_SRAM_ORIGIN + GENESIS_SRAM_SIZE),
           "ERROR: SRAM overflow — reduce heap or stack sizes")

    /* Ensure scratch banks are not over-committed */
    ASSERT(__scratch_x_end__ <= (GENESIS_SCRATCH_X_ORIGIN + GENESIS_SCRATCH_X_SIZE),
           "ERROR: scratch_x overflow")
    ASSERT(__scratch_y_end__ <= (GENESIS_SCRATCH_Y_ORIGIN + GENESIS_SCRATCH_Y_SIZE),
           "ERROR: scratch_y overflow")

    /* ---- Discard sections ------------------------------------------------- */
    /DISCARD/ : {
        *(.note.GNU-stack)
        *(.note.*)
        *(.comment)
        *(.rel.*)
    }
}

/* ---- Default size values (override via target_compile_definitions) --------- */
#ifndef GENESIS_HEAP_SIZE
#define GENESIS_HEAP_SIZE       49152    /* 48 KB */
#endif

#ifndef GENESIS_CORE0_STACK_SIZE
#define GENESIS_CORE0_STACK_SIZE 4096   /* 4 KB */
#endif

#ifndef GENESIS_CORE1_STACK_SIZE
#define GENESIS_CORE1_STACK_SIZE 4096   /* 4 KB */
#endif
