# MCAL GPT (General Purpose Timer) - How It Works

## Architecture Overview

```
┌─────────────────────────────────────┐
│   RP2040 Hardware (4 Alarm Channels) │
└────────────────┬────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────┐
│   GPT Module (Software Layer)        │
│  - 4 Timer Control Blocks (array)    │
│  - Callback management              │
│  - Time tracking                    │
└─────────────────────────────────────┘
```

## Step-by-Step: Timer Start and Operation

### 1. Initialization Phase (`gpt_init`)

```c
gpt_timers[4]  // Array of 4 timer control blocks (one per alarm)
{
  timer_id: 0
  period_us: 10000     // Set from config table
  mode: PERIODIC
  start_time_us: 0     // Not set yet
  is_running: false
  callback: NULL
}
```

**What happens:**
- Clears all 4 timer control blocks
- Sets `is_running = false` for all timers
- Initializes each block with its timer ID and alarm number
- Module is now ready to accept start commands

### 2. Starting a Timer (`gpt_start_timer(GPT_TIMER_0)`)

```c
void gpt_start_timer(gpt_timer_id_t timer_id) {
    // Step 1: Get the control block for this timer
    gpt_timer_ctrl_t *ctrl = &gpt_timers[timer_id];
    
    // Step 2: Capture current system time (in microseconds)
    ctrl->start_time_us = time_us_64();  // e.g., 1000000 μs
    
    // Step 3: Mark timer as active
    ctrl->is_running = true;
    
    // Step 4: Request hardware alarm from RP2040
    add_alarm_in_us(ctrl->period_us,      // 10000 μs later
                    timer_callbacks[0],     // Call gpt_timer0_callback
                    ctrl,                   // Pass control block as context
                    false);
}
```

**State Change:**
```
BEFORE:                          AFTER:
is_running: false       ───►     is_running: true
start_time_us: 0        ───►     start_time_us: 1000000
                                 Hardware alarm scheduled
```

### 3. While Timer is Running

Your code can check elapsed time **without blocking**:

```c
// Get elapsed microseconds
uint64_t elapsed = gpt_get_timervalue(GPT_TIMER_0);
// Calculates: time_us_64() - 1000000

// Check if period expired
bool expired = gpt_is_timer_elapsed(GPT_TIMER_0);
// Returns true when: elapsed >= period_us (10000 μs)
```

### 4. When Timer Expires (Hardware Alarm Fires)

```c
static int64_t gpt_timer0_callback(alarm_id_t id, void *user_data) {
    gpt_timer_ctrl_t *ctrl = (gpt_timer_ctrl_t *)user_data;
    
    // Step 1: Execute user's callback (if registered)
    if (ctrl->callback) {
        ctrl->callback();  // User's interrupt service routine
    }
    
    // Step 2: Check mode
    if (ctrl->mode == GPT_MODE_PERIODIC && ctrl->is_running) {
        return ctrl->period_us;  // Re-schedule for next period
    }
    
    // Step 3: For ONESHOT mode, mark as stopped
    ctrl->is_running = false;
    return 0;  // No reschedule
}
```

**What happens:**
- Hardware interrupt fires when period expires
- Callback function executes (may call user code)
- For PERIODIC: Automatically reschedules itself
- For ONESHOT: Stops and sets `is_running = false`

## Example Flow (PERIODIC Timer - 10ms period)

```
Time    Action                          Status
────────────────────────────────────────────────────
0μs     gpt_start_timer(0)             is_running=true, alarm scheduled
        start_time_us = 0
        
1000μs  Your code runs...              elapsed = 1000μs
5000μs  Your code checks...            is_timer_elapsed() = false
        (5000 < 10000)
        
10000μs ⚡ ALARM FIRES!                gpt_timer0_callback() executes
        ├─ Calls user callback (if any)
        └─ Reschedules for 10000μs more
        
10001μs Your code continues...         is_timer_elapsed() = true
                                        elapsed = ~10001μs
                                        
20000μs ⚡ ALARM FIRES AGAIN!          Callback executes + reschedules
```

## Key Design Features

| Feature | Details |
|---------|---------|
| **Non-blocking** | No busy-wait loops. Your code continues executing |
| **Hardware-based** | RP2040's alarm channels do the actual timing |
| **Software tracking** | `gpt_timers[]` array tracks elapsed time in software |
| **Callback model** | Optional function executes when timer expires |
| **Dual modes** | ONESHOT (fires once) or PERIODIC (repeats automatically) |
| **4 independent timers** | Each uses one RP2040 alarm channel |
| **Microsecond precision** | 1μs resolution from RP2040 timer |

## API Functions

### Initialization
- `void gpt_init(void)` - Initialize all timers

### Generic Timer Functions
- `void gpt_start_timer(gpt_timer_id_t timer_id)` - Start a timer
- `void gpt_stop_timer(gpt_timer_id_t timer_id)` - Stop a timer
- `bool gpt_is_timer_elapsed(gpt_timer_id_t timer_id)` - Check if expired
- `uint64_t gpt_get_timervalue(gpt_timer_id_t timer_id)` - Get elapsed time (μs)
- `gpt_timer_status_t gpt_get_timer_status(gpt_timer_id_t timer_id)` - Get full status

### Timer IDs (use with above functions)
- `GPT_TIMER_0` - Timer 0
- `GPT_TIMER_1` - Timer 1
- `GPT_TIMER_2` - Timer 2
- `GPT_TIMER_3` - Timer 3

## Real-World Example (from main.c)

```c
// Measure system startup time
gpt_init();                              // Initialize GPT
gpt_start_timer(GPT_TIMER_0);           // Start measuring

// ... system boots ...
OS_Init();
RTE_Init();
EcuM_Init();
vTaskStartScheduler();                   // RTOS starts

// Later, get how long boot took
uint64_t startup_time = gpt_get_timervalue(GPT_TIMER_0);
gpt_stop_timer(GPT_TIMER_0);
printf("Startup took: %llu microseconds\n", startup_time);
```

## Configuration

Timer periods are configured in `mcal_gpt_cfg.c`:

```c
const gpt_timer_config_t gpt_timer_cfg[GPT_TIMER_MAX] = {
    /* Timer 0 - 10ms periodic */
    {
        .timer_id = GPT_TIMER_0,
        .period_us = 10000,      // 10 milliseconds
        .mode = GPT_MODE_PERIODIC,
        .callback = NULL         // Set at runtime if needed
    },
    // ... more timers ...
};
```

## Summary

1. **Init**: Clear all timer blocks
2. **Start**: Capture time, mark running, schedule hardware alarm
3. **Running**: Your code can query elapsed time anytime (non-blocking)
4. **Expire**: Hardware fires interrupt, callback executes, auto-reschedule if periodic
5. **Stop**: Mark `is_running = false` to stop timer
