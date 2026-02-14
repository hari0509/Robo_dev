# 💠 Genesis Platform
Generic Platform to build the Hard real-time Embedded systems on RP2040.

## Fetch Process

```bash
git clone https://github.com/hari0509/Robo_dev.git 
```

## Build

### Quick build (using script)

The easiest way — run the build script from the repo root:

```bash
./code/vip/programs/buildall/build.sh
```

This will:
- Create `code/vip/programs/output/build` (if needed)
- Configure and build all apps
- Place artifacts in `code/vip/programs/output/build`

### Manual build

Or run CMake commands directly:

```bash
cd code/vip/programs
mkdir -p output/build && cd output/build
cmake ../../buildall
make -j4
```

Artifacts will be in `code/vip/programs/output/build`, e.g., `blink_build/blink.elf`.
