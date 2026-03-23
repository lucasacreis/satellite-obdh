# Satellite OBDH Simulator

Embedded Linux satellite On-Board Data Handling (OBDH) simulator built for ARM Cortex-A9, targeting low Earth orbit (LEO) satellite systems.

## Architecture

- **HAL** — Hardware Abstraction Layer with SocketCAN interface
- **OBDH** — Command processing and telemetry over CAN bus
- **Beacon** — Periodic telemetry transmission with orbital environment data
- **OrbitalEnvironment** — LEO 90-minute orbit simulation (eclipse/illumination cycles)
- **CanQueue** — Thread-safe producer-consumer queue for CAN frames

## Multi-thread design
```
Thread 1 — CAN Receiver       → reads frames from vcan0 into CanQueue
Thread 2 — Command Processor  → consumes queue and dispatches commands
Thread 3 — Beacon             → transmits telemetry every N cycles
```

Graceful shutdown via SIGINT with `std::atomic<bool>`.

## Test-Driven Development

18 unit tests across 3 suites using Google Test:

| Suite | Tests | Coverage |
|---|---|---|
| ObdhTests | 5 | Command processing, telemetry, reset, edge cases |
| BeaconTests | 7 | Timing, payload encoding, periodicity, modes |
| ThreadingTests | 6 | Thread-safety, race conditions, FIFO ordering |

## Build

### Run tests (x86)
```bash
mkdir build && cd build
cmake ..
make -j4
ctest --output-on-failure --verbose
```

### Cross-compile for ARM
```bash
mkdir build-arm && cd build-arm
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm.cmake
make -j4 obdh_main
```

## Run on QEMU ARM
```bash
# Build Buildroot image (qemu_arm_vexpress_defconfig)
# Mount binary into rootfs.ext2 and boot:
qemu-system-arm -M vexpress-a9 -m 256 \
  -kernel images/zImage \
  -dtb images/vexpress-v2p-ca9.dtb \
  -drive file=images/rootfs.ext2,if=sd,format=raw \
  -append "console=ttyAMA0,115200 rootwait root=/dev/mmcblk0" \
  -nographic

# Inside QEMU:
modprobe vcan
ip link add dev vcan0 type vcan
ip link set up vcan0
/root/obdh_main
```

## Stack

- C++17 · CMake · Google Test
- ARM cross-compiler (gcc-arm-linux-gnueabihf)
- QEMU vexpress-a9 · Buildroot · SocketCAN
- WSL2 · Ubuntu 22.04
