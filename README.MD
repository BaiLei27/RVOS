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
    * **note**:Your compilation toolchain must be `-mcmodel=medany` model, otherwise you will encounter  `relocation truncated to fit: R_RISCV_HI20 against .LC0'`
    * [read this SiFive blog article.](https://www.sifive.com/blog/all-aboard-part-4-risc-v-code-models)
* **Ninja** or GNU `make`
* (Optional) **QEMU for RISC-V** to run the OS without real hardware
* If you want to compile sub projects, you also need `gtkmm-4.0`

```shell
git clone https://github.com/BaiLei27/RVOS --recurse-submodules #Clone sub projects are necessary because they have some dependencies between them
```

## **Build Steps**

From the project root:

```shell
cd path/RVOS
Arch=$(uname -m)
cmake -B build-${Arch} -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-${Arch} -v
```
This generates in builddir root:

```
RVOS   # Bare-metal RISC-V kernel image
RVOS.map   # Linker map
```

## **Run on QEMU**

After building target `run` manual

```shell
qemu-system-riscv64 -M virt -m 1G -nographic -smp 1 -bios none -kernel [target] # [target] such as `RVOS`
```
or run cmake target:
```shell
cmake --build build-${Arch}/build-RVOS -v -t run-[target]
#or
sh ./scripts/shell/qemu_run_tst.sh [target]
```
Alternatively, target `debug`:

```shell
qemu-system-riscv64 -M virt -m 1G -nographic -smp 1 -bios none -kernel-kernel [target] -s -S &
gdb [target] -ex "target remote :1234" -ex "b _start" -ex "c"
```
or run cmake target:
```shell
cmake --build build-${Arch}/build-RVOS -v -t dbg-[target]
#or
sh ./scripts/shell/qemu_debug.sh [target]
```
---

[ref1]:(https://www.sifive.com/blog/all-aboard-part-4-risc-v-code-models)
