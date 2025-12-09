# **RVOS — A Minimal, Modern RISC-V Operating System written in Contemporary C++**

**RVOS** is a minimal, educational-oriented operating system designed for the RISC-V architecture.
The project is fully implemented using **modern C++ (C++20/23)** with clean abstractions, a focus on clarity, and an emphasis on understanding low-level system behavior.

RVOS aims to provide a compact and comprehensible codebase for exploring:

* Bare-metal RISC-V execution flow
* Low-level hardware initialization
* Memory layout & privilege levels
* Interrupt handling
* Simple kernel infrastructure
* Hardware–software interaction from power-on to running C++ code

It is designed as a personal research project to deepen understanding of operating systems and kernel

---

# Build

RVOS uses a minimal and portable build setup based on **CMake** and a **RISC-V bare-metal toolchain**.
Only standard tools are required, allowing the project to be built on Linux, macOS, or MSYS2 (Windows).

## **Prerequisites**

You will need:

* **CMake ≥ 3.20**
* **RISC-V GCC bare-metal toolchain >= 12.0** (e.g., `riscv64-unknown-elf-gcc riscv64-unknown-elf-toolchain`)
* **Ninja** or GNU `make`
* (Optional) **QEMU for RISC-V** to run the OS without real hardware:

Example installation (MSYS2 / Arch ):

```shell
sudo pacman -S riscv64-elf-gcc riscv64-elf-newlib # or riscv64-unknown-elf-gcc
sudo pacman -S cmake ninja qemu-system-riscv
```
in Ubuntu
```shell
sudo apt install riscv64-unknown-elf-gcc #or gcc-riscv64-unknown-elf
sudo apt install cmake ninja-build qemu-system-riscv
```
---

## **Build Steps**

From the project root:

```shell
cmake -S . -B build-x86_64  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=riscv64-elf-gcc -DCMAKE_CXX_COMPILER=riscv64-elf-g++ -G Ninja #[deprecate]
# You need to set the `-DCMAKE_C_COMPILER`(default `riscv64-elf-gcc`) parameter in the root directory `ExternalProject_Add` first
cmake -S . -B build-x86_64 -G Ninja
cmake --build build-x86_64 -v
```
This generates in builddir:

```
RVOS.elf   # Bare-metal RISC-V kernel image
RVOS.map   # Linker map
```

## **Run on QEMU**

After building target `run` manual

```shell
qemu-system-riscv64 -nographic -smp 1 -machine virt -bios none -serial mon:stdio -kernel [target name]
```
or run cmake target:
```shell
cmake --build build-x86_64/build-RVOS -v -t run-[target]
```
Alternatively, target `debug`:

```shell
qemu-system-riscv64 -nographic -smp 1 -machine virt -bios none -serial mon:stdio -kernel [target name] -s -S&
gdb [target name] -ex "target remote :1234" -ex "b _start" -ex "c"
```
or run cmake target:
```shell
cmake --build build-x86_64/build-RVOS -v -t dbg-[target]
```

---
