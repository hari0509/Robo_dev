#include "mcal_gpt.h"
#include <stddef.h>

/* Timer configuration table */
const gpt_timer_config_t gpt_timer_cfg[GPT_TIMER_MAX] = {
    /* Timer 0 - 10ms periodic timer */
    {
        .timer_id = GPT_TIMER_0,
        .period_us = 10000,      /* 10 milliseconds */
        .mode = GPT_MODE_PERIODIC,
        .callback = NULL         /* Set at runtime if needed */
    },
    
    /* Timer 1 - 50ms periodic timer */
    {
        .timer_id = GPT_TIMER_1,
        .period_us = 50000,      /* 50 milliseconds */
        .mode = GPT_MODE_PERIODIC,
        .callback = NULL
    },
    
    /* Timer 2 - 100ms oneshot timer */
    {
        .timer_id = GPT_TIMER_2,
        .period_us = 100000,     /* 100 milliseconds */
        .mode = GPT_MODE_ONESHOT,
        .callback = NULL
    },
    
    /* Timer 3 - 1 second periodic timer */
    {
        .timer_id = GPT_TIMER_3,
        .period_us = 1000000,    /* 1 second */
        .mode = GPT_MODE_PERIODIC,
        .callback = NULL
    }
};

/**
 * @brief Get timer configuration
 * @param timer_id Timer ID
 * @return Pointer to timer configuration
 */
const gpt_timer_config_t* gpt_get_timer_config(gpt_timer_id_t timer_id) {
    if (timer_id >= GPT_TIMER_MAX) {
        return NULL;
    }
    return &gpt_timer_cfg[timer_id];
}

/**
 * @brief Configure timer at runtime
 * @param config Pointer to timer configuration
 */
void gpt_configure_timer(const gpt_timer_config_t *config) {
    if (config == NULL || config->timer_id >= GPT_TIMER_MAX) {
        return;
    }
    
    /* Note: Configuration should be done before starting the timer */
    gpt_timer_config_t *cfg = (gpt_timer_config_t *)&gpt_timer_cfg[config->timer_id];
    *cfg = *config;
}
