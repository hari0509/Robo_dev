# Project Folder Structure

## Current Architecture

```
code/vip/programs/
├── apps
│   └── blink
│       ├── CMakeLists.txt
│       └── src
│           └── asw_blink.c
├── bsw_cfg
│   ├── CMakeLists.txt
│   ├── os.c
│   ├── os_callout_stubs.c
│   ├── os_callout_stubs.h
│   ├── os.h
│   ├── rte.c
│   ├── rte_core0.c
│   ├── rte_core0.h
│   ├── rte_core1.c
│   ├── rte_core1.h
│   ├── rte.h
│   └── RTOS_config
│       └── FreeRTOSConfig.h
├── buildall
│   ├── build.sh
│   ├── CMakeLists.txt
│   ├── ecum.c
│   ├── ecum.h
│   └── main.c
├── cdd
│   └── servo
│       ├── CMakeLists.txt
│       ├── inc
│       │   └── cdd_servo.h
│       └── src
│           └── cdd_servo.c
├── Design
│   ├── program_design.md
│   └── turing_design.md
├── mcal
│   └── gpt
│       ├── CMakeLists.txt
│       ├── inc
│       │   └── mcal_gpt.h
│       └── src
│           ├── mcal_gpt.c
│           └── mcal_gpt_cfg.c
├── output
├── sys
│   └── Nvm
│       ├── CMakeLists.txt
│       ├── inc
│       │   └── sys_nvm.h
│       └── src
│           └── sys_nvm.c
└── tools
    └── Self_Test
        ├── CMakeLists.txt
        ├── inc
        │   └── TestMain.h
        └── src
            ├── TestMain.c
            └── TestScheduler.c
```

## Key Components

### buildall/
- **Parent CMake project** that orchestrates the entire build
- Contains main entry point (`main.c`) and ECU initialization (`ecum.c`)
- Manages SDK/FreeRTOS imports and task table configuration
- Generates final `vip_system.elf` executable

### bsw_cfg/
- **Basic Software Configuration layer**
- OS abstraction and task management (`os.c/h`)
- Runtime Environment for periodic task scheduling (`rte.c/h`)
- Initializes both ASW and CDD tasks at 100ms intervals

### apps/blink/
- **Application Software (ASW) blink component**
- LED control functionality for system visibility
- Provides `asw_blink_mainfunction()` called by RTE task

### cdd/servo/
- **Calibration Data Device (CDD) servo component**
- Servo/motor control and debug LED toggling
- Provides `cdd_servo_mainfunction()` called by RTE task

### output/build/
- **Out-of-tree build directory**
- All object files, libraries, and final executable generated here
- CMake configuration run from this directory

## Build Flow

```
$ cd code/vip/programs/buildall
$ sh build.sh

→ Creates output/build if needed
→ Runs: cmake ../../buildall
→ Runs: make -j4

→ Builds:
  1. freertos (FreeRTOS kernel library)
  2. bsw_cfg (OS/RTE library)
  3. blink (ASW library)
  4. cdd_servo (CDD library)
  5. vip_system (Final executable linking all above)

→ Final artifact: output/build/vip_system.elf
```

## Build Configuration

- **Language Standard:** C11, C++17
- **Platform:** RP2040 (Raspberry Pi Pico)
- **RTOS:** FreeRTOS
- **Build Type:** Release (optimized)
- **Parallel Jobs:** 4 (make -j4)

## Revision History

- **v1.0** - Initial commit - Hari
  - Created modular build system with parent/child CMake pattern
  - Implemented task table-based OS initialization
  - Added structured code sections to all C files
  - Established out-of-tree build in `programs/output/build`
