#!/bin/sh

IMAGES_DIR="$(cd "$(dirname "$0")" && pwd)"

exec qemu-system-arm \
  -M vexpress-a9 \
  -smp 1 \
  -m 256 \
  -kernel "${IMAGES_DIR}/zImage" \
  -dtb "${IMAGES_DIR}/vexpress-v2p-ca9.dtb" \
  -drive file="${IMAGES_DIR}/rootfs.ext2",if=sd,format=raw \
  -append "console=ttyAMA0,115200 rootwait root=/dev/mmcblk0" \
  -net nic,model=lan9118 \
  -net user \
  -nographic \
  -s \
  "$@"

