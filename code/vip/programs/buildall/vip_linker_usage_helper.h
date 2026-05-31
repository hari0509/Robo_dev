/**
 * genesis_linker_usage.h
 * code/vip/programs/buildall/linker/genesis_linker_usage.h
 *
 * Helper macros for placing variables and functions in the custom
 * Genesis linker sections defined in genesis_rp2040.ld.
 *
 * Include this header in any module that needs to interact with the
 * custom memory layout.
 */

#ifndef GENESIS_LINKER_USAGE_H
#define GENESIS_LINKER_USAGE_H

#include <stdint.h>

/* ---- Section placement macros --------------------------------------------- */

/**
 * GENESIS_NOINIT
 * Variables marked with this attribute are placed in .noinit SRAM.
 * They are NOT zero-initialised on startup — they survive a warm reset.
 *
 * Example:
 *   GENESIS_NOINIT uint32_t g_reset_cause;
 *   GENESIS_NOINIT uint8_t  g_safe_state_buf[64];
 */
#define GENESIS_NOINIT \
    __attribute__((section(".noinit")))

/**
 * GENESIS_SCRATCH_X
 * Functions placed in SRAM scratch bank X execute from single-cycle SRAM.
 * Use for ISRs or the FreeRTOS port layer where deterministic latency matters.
 *
 * Example:
 *   GENESIS_SCRATCH_X void SysTick_Handler(void) { ... }
 */
#define GENESIS_SCRATCH_X(name) \
    __attribute__((section(".scratch_x." #name), noinline))

/**
 * GENESIS_SCRATCH_Y
 * Same as SCRATCH_X but in bank Y — intended for DMA/PIO descriptor buffers.
 *
 * Example:
 *   GENESIS_SCRATCH_Y static dma_channel_config g_dma_cfg;
 */
#define GENESIS_SCRATCH_Y(name) \
    __attribute__((section(".scratch_y." #name)))

/* ---- Linker-exported symbols ---------------------------------------------- */

/* SRAM layout pointers — declared by genesis_rp2040.ld */
extern uint32_t __noinit_start__;
extern uint32_t __noinit_end__;
extern uint32_t __scratch_x_start__;
extern uint32_t __scratch_x_end__;
extern uint32_t __scratch_y_start__;
extern uint32_t __scratch_y_end__;


#endif /* GENESIS_LINKER_USAGE_H */

