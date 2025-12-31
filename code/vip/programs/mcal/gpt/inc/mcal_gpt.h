#ifndef MCAL_GPT_H
#define MCAL_GPT_H

#include <stdint.h>
#include <stdbool.h>

/* Timer IDs */
typedef enum {
    GPT_TIMER_0 = 0,
    GPT_TIMER_1 = 1,
    GPT_TIMER_2 = 2,
    GPT_TIMER_3 = 3,
    GPT_TIMER_MAX
} gpt_timer_id_t;

/* Timer modes */
typedef enum {
    GPT_MODE_ONESHOT = 0,  /* Single shot */
    GPT_MODE_PERIODIC = 1  /* Repeating */
} gpt_timer_mode_t;

/* Timer callback function type */
typedef void (*gpt_callback_t)(void);

/* Timer configuration structure */
typedef struct {
    gpt_timer_id_t timer_id;
    uint32_t period_us;      /* Period in microseconds */
    gpt_timer_mode_t mode;
    gpt_callback_t callback;
} gpt_timer_config_t;

/* Timer status structure */
typedef struct {
    bool is_running;
    uint32_t elapsed_us;
    uint32_t period_us;
} gpt_timer_status_t;

/* Function prototypes */

/* Initialization */
void gpt_init(void);

/* Generic timer functions */
void gpt_start_timer(gpt_timer_id_t timer_id);
void gpt_stop_timer(gpt_timer_id_t timer_id);
bool gpt_is_timer_elapsed(gpt_timer_id_t timer_id);
uint64_t gpt_get_timervalue(gpt_timer_id_t timer_id);
gpt_timer_status_t gpt_get_timer_status(gpt_timer_id_t timer_id);

#endif /* MCAL_GPT_H */
