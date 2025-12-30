# Turing Folder Structure

## Overview

The `turing` folder contains third-party repositories and SDKs:
- **pico-sdk** – Raspberry Pi Pico SDK (hardware abstraction layer, HALs, libraries)
- **FreeRTOS-Kernel** – FreeRTOS RTOS kernel and RP2040-specific port

## Directory Tree

```
code/vip/turing/
├── pico-sdk/                           # Raspberry Pi Pico SDK
│   ├── CMakeLists.txt                  # SDK CMake configuration
│   ├── LICENSE.TXT                     # SDK license
│   ├── README.md                       # SDK documentation
│   ├── pico_sdk_init.cmake             # SDK initialization CMake module
│   ├── pico_sdk_version.cmake          # SDK version information
│   │
│   ├── cmake/                          # CMake helper modules
│   │   ├── pico_utils.cmake
│   │   ├── generic_board.cmake
│   │   ├── pico_pre_load_platform.cmake
│   │   ├── pico_pre_load_toolchain.cmake
│   │   └── Platform/                   # Platform-specific CMake
│   │
│   ├── src/                            # SDK source code
│   │   ├── boards/                     # Board definitions (pico, pico_w, etc.)
│   │   ├── common/                     # Common SDK utilities
│   │   │   ├── pico_sync/              # Synchronization primitives
│   │   │   ├── pico_time/              # Timing utilities
│   │   │   ├── pico_util/              # General utilities
│   │   │   └── ...
│   │   ├── rp2_common/                 # RP2040-specific implementations
│   │   │   ├── hardware_adc/           # ADC driver
│   │   │   ├── hardware_clocks/        # Clock configuration
│   │   │   ├── hardware_dma/           # DMA controller
│   │   │   ├── hardware_gpio/          # GPIO driver
│   │   │   ├── hardware_i2c/           # I2C driver
│   │   │   ├── hardware_irq/           # Interrupt handling
│   │   │   ├── hardware_pwm/           # PWM driver
│   │   │   ├── hardware_spi/           # SPI driver
│   │   │   ├── hardware_uart/          # UART/Serial driver
│   │   │   ├── hardware_timer/         # Timer driver
│   │   │   ├── pico_multicore/         # Multicore support
│   │   │   ├── pico_platform/          # Platform initialization
│   │   │   ├── pico_runtime/           # Runtime initialization
│   │   │   ├── pico_stdio/             # Standard I/O (printf/puts)
│   │   │   └── ...
│   │   └── rp2040/                     # RP2040-specific code
│   │       └── pico_platform/
│   │
│   ├── lib/                            # Third-party libraries
│   │   ├── tinyusb/                    # USB stack
│   │   └── ...
│   │
│   ├── external/                       # External imports
│   │   ├── pico_sdk_import.cmake       # Module to import SDK into projects
│   │   └── ...
│   │
│   ├── tools/                          # Tools and utilities
│   │   └── ...
│   │
│   ├── docs/                           # Documentation
│   │   └── ...
│   │
│   ├── test/                           # SDK tests
│   │   └── ...
│   │
│   ├── bazel/                          # Bazel build system (alternative)
│   │   └── ...
│   │
│   └── BUILD.bazel, MODULE.bazel       # Bazel configuration files
│
└── FreeRTOS-Kernel/                    # FreeRTOS RTOS Kernel
    ├── CMakeLists.txt                  # FreeRTOS CMake configuration
    ├── LICENSE.md                      # FreeRTOS license (MIT)
    ├── README.md                       # FreeRTOS documentation
    ├── History.txt                     # FreeRTOS version history
    │
    ├── tasks.c                         # Task management
    ├── queue.c                         # Queue/message passing
    ├── timers.c                        # Software timers
    ├── event_groups.c                  # Event group synchronization
    ├── stream_buffer.c                 # Stream buffer utilities
    ├── croutine.c                      # Co-routines (legacy)
    ├── list.c                          # Linked list utilities
    │
    ├── include/                        # FreeRTOS public headers
    │   ├── FreeRTOS.h                  # Main FreeRTOS header
    │   ├── task.h                      # Task API
    │   ├── queue.h                     # Queue API
    │   ├── timers.h                    # Timer API
    │   ├── event_groups.h              # Event group API
    │   ├── semphr.h                    # Semaphore/Mutex API
    │   ├── stream_buffer.h             # Stream buffer API
    │   ├── message_buffer.h            # Message buffer API
    │   ├── portable.h                  # Portable layer header
    │   ├── projdefs.h                  # Project definitions
    │   └── ...
    │
    ├── portable/                       # Port implementations
    │   ├── CMakeLists.txt              # Portable layer CMake
    │   │
    │   ├── ThirdParty/                 # Third-party ports
    │   │   ├── GCC/
    │   │   │   └── RP2040/             # **RP2040 GCC Port** (used in this project)
    │   │   │       ├── CMakeLists.txt
    │   │   │       ├── FreeRTOS_Kernel_import.cmake  # Import module
    │   │   │       ├── port.c          # RP2040 port implementation
    │   │   │       ├── portasm.S       # Assembly-level port code
    │   │   │       └── include/
    │   │   │           └── portmacro.h # Port-specific macros
    │   │   └── ...
    │   │
    │   ├── MemMang/                    # Memory management schemes
    │   │   ├── heap_1.c                # Simple non-freeing allocator
    │   │   ├── heap_2.c                # Coalescing free list
    │   │   ├── heap_3.c                # malloc/free wrapper (used here)
    │   │   ├── heap_4.c                # Coalescing first-fit
    │   │   └── heap_5.c                # Multiple pools
    │   │
    │   ├── Common/                     # Common utility code
    │   │   └── ...
    │   │
    │   └── readme.txt                  # Portable layer documentation
    │
    └── MISRA.md                        # MISRA C compliance information
```

## Key Integration Points

### Used in This Project

**pico-sdk components included:**
- `hardware_gpio/` – GPIO control for LED toggling
- `hardware_uart/` – Serial communication (stdio)
- `pico_stdlib/` – Standard library wrapper
- `pico_multicore/` – Multicore support (for future use)
- `pico_sync/` – Synchronization primitives

**FreeRTOS components included:**
- `portable/ThirdParty/GCC/RP2040/` – RP2040 port
- `portable/MemMang/heap_3.c` – Memory allocator (malloc/free wrapper)
- Core task management from `tasks.c`, `queue.c`, `timers.c`

### CMake Integration

Both SDKs are imported via CMake in `buildall/CMakeLists.txt`:

```cmake
# Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
pico_sdk_init()

# FreeRTOS Kernel
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
```

These imports provide:
- `pico_stdlib` – Pico SDK target
- `FreeRTOS-Kernel` – FreeRTOS core target
- `FreeRTOS-Kernel-Heap3` – FreeRTOS heap allocator target

## Build Configuration

- **Toolchain:** ARM GCC (arm-none-eabi)
- **Platform:** RP2040 (Cortex-M0+, 2-core)
- **Board:** Raspberry Pi Pico (default)
- **Optimization:** Release mode (-O3)

## Revision History

- **v1.0** - Initial commit - Hari
  - Documented pico-sdk structure and components
  - Documented FreeRTOS-Kernel RP2040 port integration
  - Identified key HAL drivers and RTOS features used
