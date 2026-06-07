#!/usr/bin/env bash
set -euo pipefail

# Global configuration (shared between both scripts)
NPROC=1

# Print usage information
usage() {
    cat <<EOF
Usage: $0 <kernel> [extra-qemu-args...]
Run the RISC-V kernel directly in QEMU (with semihosting)

Example:
  $0 build/kernel.elf
  $0 build/kernel.elf -m 2G -smp 4
EOF
    exit 1
}

# Parse and validate command line arguments
parse_args() {
    if [[ $# -lt 1 ]]; then
        usage
    fi
    KERNEL="$1"  # Path to kernel ELF file
    shift
    EXTRA_QEMU_ARGS=("$@")  # Additional QEMU arguments

    # Check if kernel file exists
    if [[ ! -f "$KERNEL" ]]; then
        echo "Error: Kernel file not found: $KERNEL" >&2
        exit 1
    fi

    # Check if readelf is available
    if ! command -v readelf &>/dev/null; then
        echo "Error: 'readelf' command not found" >&2
        exit 1
    fi

    # Auto-detect RISC-V architecture (32/64-bit)
    local elf_class
    elf_class="$(readelf -h "$KERNEL" | awk -F: '/Class:/ {gsub(/^[ \t]+/, "", $2); print $2}')"
    case "$elf_class" in
        *ELF32*) QEMU_BIN="qemu-system-riscv32" ;;  # 32-bit RISC-V
        *ELF64*) QEMU_BIN="qemu-system-riscv64" ;;  # 64-bit RISC-V
        *) echo "Error: Unsupported ELF class: $elf_class" >&2; exit 1 ;;
    esac

    # Check if QEMU binary is installed
    if ! command -v "$QEMU_BIN" &>/dev/null; then
        echo "Error: $QEMU_BIN not found. Install it via your package manager." >&2
        exit 1
    fi
}

# Build complete QEMU command with all arguments
build_qemu_command() {
    # Base QEMU arguments (shared between all modes)
    local common_args=(
        -M virt             # Use virt machine type
        -m 1G               # Allocate 1GB of memory
        -smp "$NPROC"       # Number of CPU cores
        -nographic          # Disable graphical output
        -monitor none       # Disable QEMU monitor
        -no-reboot          # Exit on reboot instead of restarting
        -bios none          # Use no BIOS (direct kernel boot)
        -kernel "$KERNEL"   # Kernel image to boot
    )

    local run_args=(
        -semihosting-config enable=on  # Enable semihosting
    )

    # Combine all arguments into final command
    QEMU_FULL_CMD=(
        "$QEMU_BIN"
        "${common_args[@]}"
        "${run_args[@]}"
        "${EXTRA_QEMU_ARGS[@]}"
    )
}

# Main function: run QEMU directly
main_run() {
    parse_args "$@"
    build_qemu_command
    set -x
    exec "${QEMU_FULL_CMD[@]}"  # Replace current process with QEMU
}

# Run main function only if script is executed directly (not sourced)
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main_run "$@"
fi
