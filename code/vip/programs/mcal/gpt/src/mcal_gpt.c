#include "mcal_gpt.h"
#include "pico/time.h"
#include <string.h>

/* Timer control block */
typedef struct {
    gpt_timer_id_t timer_id;
    uint32_t period_us;
    gpt_timer_mode_t mode;
    gpt_callback_t callback;
    uint64_t start_time_us;
    bool is_running;
    uint32_t alarm_num;
} gpt_timer_ctrl_t;

/* Global timer control blocks */
static gpt_timer_ctrl_t gpt_timers[GPT_TIMER_MAX];
static bool gpt_initialized = false;

static int64_t gpt_general_timer_callback(alarm_id_t id, void *user_data) {
    gpt_timer_ctrl_t *ctrl = (gpt_timer_ctrl_t *)user_data;
    
    if (ctrl == NULL || !ctrl->is_running) {
        return 0;  /* No action needed */
    }
    
    /* Invoke user callback if set */
    if (ctrl->callback) {
        ctrl->callback();
    }
    
    if (ctrl->mode == GPT_MODE_PERIODIC) {
        /* Reschedule for periodic mode */
        return ctrl->period_us;
    } else {
        /* Stop timer for one-shot mode */
        ctrl->is_running = false;
        return 0;
    }
}

/* Timer callback wrapper for alarm 0 */
static int64_t gpt_timer0_callback(alarm_id_t id, void *user_data) {
    return gpt_general_timer_callback(id, user_data);
}

/* Timer callback wrapper for alarm 1 */
static int64_t gpt_timer1_callback(alarm_id_t id, void *user_data) {
    return gpt_general_timer_callback(id, user_data);
}

/* Timer callback wrapper for alarm 2 */
static int64_t gpt_timer2_callback(alarm_id_t id, void *user_data) {
    return gpt_general_timer_callback(id, user_data);
}

/* Timer callback wrapper for alarm 3 */
static int64_t gpt_timer3_callback(alarm_id_t id, void *user_data) {
    return gpt_general_timer_callback(id, user_data);
}

/* Callback array for each alarm */
static alarm_callback_t timer_callbacks[GPT_TIMER_MAX] = {
    gpt_timer0_callback,
    gpt_timer1_callback,
    gpt_timer2_callback,
    gpt_timer3_callback
};

/**
 * @brief Initialize GPT module
 */
void gpt_init(void) {
    if (gpt_initialized) {
        return;
    }
    
    memset(gpt_timers, 0, sizeof(gpt_timers));
    
    for (int i = 0; i < GPT_TIMER_MAX; i++) {
        gpt_timers[i].timer_id = (gpt_timer_id_t)i;
        gpt_timers[i].alarm_num = i;
        gpt_timers[i].is_running = false;
        gpt_timers[i].callback = NULL;
    }
    
    gpt_initialized = true;
}

/**
 * @brief Start a timer with specified period and callback
 */
void gpt_start_timer(gpt_timer_id_t timer_id) {
    if (timer_id >= GPT_TIMER_MAX) {
        return;
    }
    
    gpt_timer_ctrl_t *ctrl = &gpt_timers[timer_id];
    
    if (ctrl->is_running) {
        return;  /* Already running */
    }
    
    ctrl->start_time_us = time_us_64();
    ctrl->is_running = true;
    
    /* Schedule alarm */
    add_alarm_in_us(ctrl->period_us, timer_callbacks[timer_id], ctrl, false);
}

/**
 * @brief Stop a timer
 */
void gpt_stop_timer(gpt_timer_id_t timer_id) {
    if (timer_id >= GPT_TIMER_MAX) {
        return;
    }
    
    gpt_timers[timer_id].is_running = false;
}

/**
 * @brief Check if timer has elapsed
 */
bool gpt_is_timer_elapsed(gpt_timer_id_t timer_id) {
    if (timer_id >= GPT_TIMER_MAX) {
        return false;
    }
    
    gpt_timer_ctrl_t *ctrl = &gpt_timers[timer_id];
    
    if (!ctrl->is_running) {
        return false;
    }
    
    uint64_t elapsed = time_us_64() - ctrl->start_time_us;
    return elapsed >= ctrl->period_us;
}

/**
 * @brief Get current timer value (elapsed time in microseconds)
 */
uint64_t gpt_get_timervalue(gpt_timer_id_t timer_id) {
    if (timer_id >= GPT_TIMER_MAX) {
        return 0;
    }
    
    gpt_timer_ctrl_t *ctrl = &gpt_timers[timer_id];
    
    if (!ctrl->is_running) {
        return 0;
    }
    
    return time_us_64() - ctrl->start_time_us;
}
/**
 * @brief Get timer status
 */
gpt_timer_status_t gpt_get_timer_status(gpt_timer_id_t timer_id) {
    gpt_timer_status_t status = {0};
    
    if (timer_id >= GPT_TIMER_MAX) {
        return status;
    }
    
    gpt_timer_ctrl_t *ctrl = &gpt_timers[timer_id];
    status.is_running = ctrl->is_running;
    status.period_us = ctrl->period_us;
    
    if (ctrl->is_running) {
        status.elapsed_us = (uint32_t)(time_us_64() - ctrl->start_time_us);
    }
    
    return status;
}
