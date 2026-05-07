#!/usr/bin/env bash

# Usage: ./qemu-debug <target_elf>
set -euo pipefail

if [ $# -lt 1 ]; then
    echo "Usage: $0 <kernel_binary>"
    exit 1
fi

target="$1"

cleanup() {
    echo "Cleaning up: killing QEMU (pid ${QEMU_PID})"
    kill -- -${QEMU_PID} &>/dev/null
}

trap cleanup EXIT INT TERM

qemu-system-riscv64                           \
    -M virt -m 1G -smp 1                      \
    -nographic  -no-reboot                    \
    -bios none  -kernel "${target}" -s -S &

QEMU_PID=$!

echo "QEMU started with pid '${QEMU_PID}', waiting for GDB connection..."

gdb -q "${target}"              \
    -ex "target remote :1234"   \
    -ex "b _start"              \
    -ex "b main"                \
    -ex "c"

wait "$QEMU_PID" 2>/dev/null || true
