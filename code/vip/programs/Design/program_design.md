# Project Folder Structure

## Current Architecture

```
code/vip/programs/
├── buildall/
│   ├── CMakeLists.txt          # Parent CMake (main build configuration)
│   ├── build.sh                # Build script
│   ├── main.c                  # RTOS entry point
│   ├── ecum.c                  # ECU Manager component
│   └── ecum.h                  # ECU Manager header
│
├── bsw_cfg/
│   ├── CMakeLists.txt          # BSW config library CMake
│   ├── os.c                    # OS abstraction implementation
│   ├── os.h                    # OS abstraction header
│   ├── rte.c                   # Runtime Environment implementation
│   ├── rte.h                   # Runtime Environment header
│   └── RTOS_config/
│       └── FreeRTOSConfig.h    # FreeRTOS configuration
│
├── apps/
│   └── blink/
│       ├── CMakeLists.txt      # Blink app library CMake
│       └── src/
│           ├── CMakeLists.txt
│           ├── asw_blink.c     # Blink component source
│           └── build/          # (Out-of-tree build dir - created during build)
│
├── cdd/
│   └── servo/
│       ├── CMakeLists.txt      # CDD servo library CMake
│       └── src/
│           ├── cdd_servo.c     # CDD servo component source
│           └── build/          # (Out-of-tree build dir - created during build)
│
├── output/
│   └── build/                  # Main out-of-tree build directory
│       ├── vip_system.elf      # Final executable
│       ├── blink_build/        # Blink target objects
│       ├── bsw_cfg_build/      # BSW config library objects
│       ├── cdd_servo_build/    # CDD servo library objects
│       └── CMakeFiles/         # CMake generated files
│
└── Design/                     # Documentation and design artifacts
    └── FOLDER_STRUCTURE.md     # This file
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
