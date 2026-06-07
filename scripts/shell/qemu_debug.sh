#!/usr/bin/env bash
set -euo pipefail
GDB_PORT="1234"

# Import and reuse all base logic from qemu_run.sh
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
source "$SCRIPT_DIR/qemu_run.sh"

# Default breakpoints for debug mode
DEFAULT_BREAKPOINTS=("_start" "main")

# Override usage information for debug mode
usage() {
    cat <<EOF
Usage: $0 <kernel> [extra-qemu-args...]
Run QEMU in debug mode (-s -S) and automatically attach gdb/lldb

Example:
  $0 build/kernel.elf
  $0 build/kernel.elf -m 2G -smp 4
EOF
    exit 1
}

# Find available debugger (lldb first, then gdb)
find_debugger() {
    if command -v lldb &>/dev/null; then
        echo "lldb"
    elif command -v gdb &>/dev/null; then
        echo "gdb"
    else
        echo "Error: No debugger found. Install gdb or lldb first." >&2
        exit 1
    fi
}

# Build debugger command with breakpoints
build_debugger_command() {
    local dbg="$1"       # Debugger executable (gdb/lldb)
    local target="$2"    # Target ELF file
    local port="$3"      # GDB stub port
    shift 3
    local breakpoints=("$@")  # List of breakpoints to set

    if [ "$dbg" = "gdb" ]; then
        DEBUGGER_CMD=(gdb -q --nh "$target")  # Quiet mode, no ~/.gdbinit
        DEBUGGER_CMD+=(-ex "target remote :$port")  # Connect to remote target
        for bp in "${breakpoints[@]}"; do
            DEBUGGER_CMD+=(-ex "break $bp")  # Set breakpoint
        done
        DEBUGGER_CMD+=(-ex "continue")  # Continue execution after connection
    else  # lldb
        DEBUGGER_CMD=(lldb "$target")
        DEBUGGER_CMD+=(-o "gdb-remote localhost:$port")  # Connect to GDB remote
        for bp in "${breakpoints[@]}"; do
            DEBUGGER_CMD+=(-o "breakpoint set -n $bp")  # Set breakpoint
        done
        DEBUGGER_CMD+=(-o "continue")  # Continue execution after connection
    fi
}

# Cleanup function: kill QEMU process on exit
cleanup() {
    if [ -n "${QEMU_PID:-}" ] && kill -0 "$QEMU_PID" &>/dev/null; then
        echo "==> Cleaning up QEMU process (PID: $QEMU_PID)"
        kill "$QEMU_PID" &>/dev/null
        wait "$QEMU_PID" 2>/dev/null || true
    fi
}

# Main function: debug mode
main_debug() {
    parse_args "$@"

    # Build base QEMU command and add debug arguments
    build_qemu_command
    QEMU_FULL_CMD+=(-s -S)  # -s: shorthand for -gdb tcp::1234, -S: freeze CPU at startup

    # Find available debugger
    local debugger
    debugger=$(find_debugger)
    echo "++ Start debugger: $debugger"

    # Register cleanup hook for exit signals
    trap cleanup EXIT INT TERM

    # Start QEMU debug server in background
    echo "++ QEMU cmd: ${QEMU_FULL_CMD[*]}"
    "${QEMU_FULL_CMD[@]}" &
    QEMU_PID=$!  # Store QEMU process ID

    # Wait for QEMU to start and open GDB stub
    sleep 1

    # Build and execute debugger command
    build_debugger_command "$debugger" "$KERNEL" "$GDB_PORT" "${DEFAULT_BREAKPOINTS[@]}"
    echo "++ Attaching $debugger to QEMU($QEMU_PID)..."
    "${DEBUGGER_CMD[@]}"

    # Perform cleanup after debugger exits
    cleanup
}

# Run debug main function
main_debug "$@"
