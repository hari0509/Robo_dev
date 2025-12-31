#!/bin/bash

############################################################
# Build script for vip_apps (programs/buildall)
############################################################

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROGRAMS_DIR="$(dirname "$SCRIPT_DIR")"
OUTPUT_DIR="${PROGRAMS_DIR}/output/build"

echo "=== VIP Apps Build Script ==="
echo "Script dir:   $SCRIPT_DIR"
echo "Programs dir: $PROGRAMS_DIR"
echo "Output dir:   $OUTPUT_DIR"
echo ""

# Create output/build directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Navigate to build directory
cd "$OUTPUT_DIR"

# Clean previous build (optional, uncomment to always clean)
rm -rf *

echo "Running CMake configuration from: $(pwd)"
cmake "$SCRIPT_DIR"

echo ""
echo "Running make with parallel jobs..."
make -j4

echo ""
echo "=== Build Complete ==="
echo "Artifacts available in: $OUTPUT_DIR"
echo ""
