#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <kernel>" >&2
    exit 1
fi

kernel="$1"

if [[ ! -f "$kernel" ]]; then
    echo "Error: kernel not found: $kernel" >&2
    exit 1
fi

if ! command -v readelf >/dev/null 2>&1; then
    echo "Error: readelf not found" >&2
    exit 1
fi

class="$(readelf -h "$kernel" | awk -F: '/Class:/ {gsub(/^[ \t]+/, "", $2); print $2}')"

case "$class" in
    *ELF32*)
        qemu_bin="qemu-system-riscv32"
        ;;
    *ELF64*)
        qemu_bin="qemu-system-riscv64"
        ;;
    *)
        echo "Error: unsupported kernel ELF class: $class" >&2
        exit 1
        ;;
esac
set -x
exec "$qemu_bin" \
    -machine virt \
    -nographic \
    -m 512M \
    -monitor none \
    -serial none \
    -bios none \
    -semihosting-config enable=on \
    -kernel "$kernel"
