# Robo_dev
Robo DevTop

## Build

To configure and build all apps using the new parent `buildall` CMake,
create `code/vip/programs/output/build` and run CMake from that build
directory (keeps build files isolated under `output`):

```bash
# change into programs and create nested output build directory
cd code/vip/programs
mkdir -p output/build && cd output/build

# configure using the buildall parent CMake (source is ../buildall)
cmake ../../buildall

# build all apps
make -j4
```

Per-app binary directories will be created under `code/vip/programs/output/build`,
for example the `blink` executable will be at `code/vip/programs/output/build/blink_build/blink.elf`.
