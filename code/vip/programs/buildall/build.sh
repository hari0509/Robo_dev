#!/bin/bash

############################################################
                # Working #
# 1. create build directory if it doesn't exist
# mkdir -p "$OUTPUT_DIR"

# 2. navigate to build directory
# cd "$OUTPUT_DIR"

# 3. run conan to install dependencies
# conan install "$SCRIPT_DIR/profiles" --profile:host="$SCRIPT_DIR/profiles/genesis.ini" --profile:build=default --output-folder=. --build=missing


# 4. run cmake to configure the build system (code/vip/programs/output/build/CMakeCache.txt will be generated)
# cmake "$SCRIPT_DIR"

# 5. run make to build (Compilation and linking) the project
#    4threads were used here, you can adjust this number based on your CPU cores
# make -j4
############################################################

############################################################
# Build script for vip_apps (programs/buildall)
############################################################

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROGRAMS_DIR="$(dirname "$SCRIPT_DIR")"
OUTPUT_DIR="${PROGRAMS_DIR}/output/build"

echo "=== VIP Build Script ==="
echo "Script dir:   $SCRIPT_DIR"
echo "Programs dir: $PROGRAMS_DIR"
echo "Output dir:   $OUTPUT_DIR"
echo ""

# Create output/build directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Navigate to build directory
cd "$OUTPUT_DIR"

# Clean previous build (optional, uncomment to always clean)
# rm -rf *

# install dependencies with conan
conan install "$SCRIPT_DIR/profiles" --profile:host="$SCRIPT_DIR/profiles/genesis.ini" --profile:build=default --output-folder=$OUTPUT_DIR --build=missing

# Run the CMake configuration step
# Pico SDK owns the toolchain file, so we don't need to specify it here. CMake will find it via the CMAKE_PREFIX_PATH.
echo "Running CMake configuration from: $(pwd)"
cmake "$SCRIPT_DIR" -DPICO_PLATFORM=rp2040 -DCMAKE_PREFIX_PATH="$OUTPUT_DIR/build/Release/generators"


# Run the build step with parallel jobs
echo ""
echo "Running make with parallel jobs..."
make -j4

echo ""
echo "=== Build Complete ==="
echo "Artifacts available in: $OUTPUT_DIR"
echo ""

# Display memory usage and core details
if [ -f "$OUTPUT_DIR/vip_system.elf" ]; then
    echo "=== RP2040 Memory & Core Details ==="
    echo ""
    echo "--- Firmware Size Analysis ---"
    arm-none-eabi-size "$OUTPUT_DIR/vip_system.elf"
    echo ""
    
    echo "--- Memory Breakdown ---"
    FLASH=$(arm-none-eabi-size "$OUTPUT_DIR/vip_system.elf" | tail -1 | awk '{print $1}')
    RAM=$(arm-none-eabi-size "$OUTPUT_DIR/vip_system.elf" | tail -1 | awk '{print $2 + $3}')
    FLASH_KB=$(awk "BEGIN {printf \"%.2f\", $FLASH / 1024}")
    RAM_KB=$(awk "BEGIN {printf \"%.2f\", $RAM / 1024}")
    FLASH_PCT=$(awk "BEGIN {printf \"%.1f\", $FLASH * 100 / 2097152}")
    RAM_PCT=$(awk "BEGIN {printf \"%.1f\", $RAM * 100 / 270336}")
    
    echo "Flash (code + data):  $FLASH bytes (~${FLASH_KB} KB) / 2048 KB total (${FLASH_PCT}% used)"
    echo "RAM (stack + heap):   $RAM bytes (~${RAM_KB} KB) / 264 KB total (${RAM_PCT}% used)"
    echo ""
    
    # echo "--- Core Details ---"
    # echo "Microcontroller: RP2040 (Raspberry Pi Pico)"
    # echo "CPU Cores: 2 x ARM Cortex-M0+"
    # echo "CPU Speed: 133 MHz"
    # echo "Flash: 2 MB"
    # echo "RAM: 264 KB"
    # echo "GPIO Pins: 30"
    # echo ""
fi
