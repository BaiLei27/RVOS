# RVOS Development Documentation

> For the current codebase (C++23 / RISC-V / picolibc). Updated: 2026-09-30
> Companion docs: `docs/TODO.MD` (roadmap & progress), `docs/Naming_conventions.md` (naming conventions)
> Repository: https://github.com/BaiLei27/RVOS

---

## 1. Project Overview

RVOS is a minimal operating system for RISC-V (QEMU virt), written in modern C++ (C++23) and running bare-metal.

| Item | Value |
| ---- | ---- |
| Language | C++23 (`CMAKE_CXX_STANDARD 23`), assembly `.S` |
| Target | `riscv64`, `-march=rv64gc -mabi=lp64d -mcmodel=medany` |
| Runtime | picolibc (`picolibcpp.ld` provides `_start` and the C library) |
| Virtual platform | QEMU `virt`, 1 CPU (`-smp 1`), DDR @ `0x80000000` |
| Build | CMake >= 3.20 + Ninja/Make + RISC-V bare-metal toolchain >= 12.0 |

Source layout (actual directories):

```
inc/
  Arch/CSR/CSRManager.hh        S-mode CSR access singleton
  Arch/Trap/Trap.hh             S-mode trap dispatch (per-hart)
  HAL/Device/                   DevHandle/DevManager/IDevice/DeviceCRTP
  HAL/Drivers/Serial/           NS16550A/ISerial/UartConfig
  HAL/MMIO/MMIO.hpp             register read/write wrapper
  HAL/Bus/IBus.hh               (skeleton)
  Kernel/Kernel.hh              kernel singleton
  Platform/MemLayout.hh         QEMU virt platform memory map
  Util/ISingleton.hpp           CRTP singleton base
  Util/Concepts/                device/bus concept constraints
src/
  Arch/CSR/CSRManager.cc
  Arch/Trap/Trap.cc, trap_entry.S
  Arch/Boot/*.S                 (startup files; actual entry is picolibc `_start`)
  HAL/Device/DevManager.cc
  HAL/Drivers/Serial/ns16550A.cc
  Kernel/Kernel.cc, Init/init.cc
tests/
  arch/trap/TrapSync.cc         S-mode synchronous exception tests
  hal/drv/uart0.cc              NS16550A integration tests
```

---

## 2. Build & Run

```shell
Arch=$(uname -m)
cmake -B build-${Arch} -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-${Arch} -v
```

Outputs: `build-${Arch}/build-RVOS/RVOS` (kernel image) + `RVOS.map` (linker map).

Run on QEMU:

```shell
qemu-system-riscv64 -M virt -m 1G -nographic -smp 1 -bios none -kernel build-*/build-RVOS/RVOS
# or via cmake target
cmake --build build-${Arch}/build-RVOS -v -t run-RVOS
```

Debug: `cmake --build build-${Arch}/build-RVOS -v -t dbg-RVOS` (starts `-s -S` + gdb remote `:1234`).

---

## 3. Boot Flow

```
picolibc `_start` (M-mode: disable interrupts, init sp, copy .data, zero .bss)
  └─> main()                       src/Kernel/Init/init.cc
        └─> Kernel::Instance().Run()   src/Kernel/Kernel.cc
              │ 1. init(): boardInit() -> register g_uart0 -> InitAll() -> print banner
              │ 2. SupervisorTrap::SetTickHandler(OnTimerTick)
              │ 3. SupervisorTrap::Init()      // stvec = s_trap_vector_base (direct mode)
              │ 4. CSRManager::StartSupervisorMode(SupervisorMain)
              │       M-mode: MPP=S, MEPC=entry, paging off (SATP=0),
              │       delegate medeleg/mideleg=ALL, SIE(SEIE|STIE),
              │       PMP all physical memory, SSTC timer (stimecmp), tp=hartid
              │       └─> mret into S-mode
              └─> S-mode SupervisorMain()
                    EnableInterrupts()           // sstatus.SIE = 1
                    for(;;) wfi                   // idle, woken by timer interrupt
```

Key points:

- **M-mode runs only once for the transition**: all M-mode privileged setup (PMP/delegation/timer) is centralized in `StartSupervisorMode()`; thereafter the system runs in S-mode.
- **The trap vector must be installed before enabling interrupts**: `SupervisorTrap::Init()` runs before `EnableInterrupts()`.
- **The timer interrupt drives the tick**: `initializeTimer()` already sets `stimecmp = time + INTERVAL_TICKS` in M-mode; after entering S-mode with interrupts enabled, a trap fires every period.

---

## 4. Modules & Interfaces

### 4.1 Kernel (`inc/Kernel/Kernel.hh`, `src/Kernel/Kernel.cc`)

Singleton (`ISingleton<Kernel>`). Members: `PdevMgr_`, `Ready_`, `TickCount_` (tick counter, incremented by the timer interrupt).

| Interface | Description |
| ---- | ---- |
| `int Run()` | Entry: `init()` -> register tick hook -> install trap vector -> `StartSupervisorMode` (never returns) |
| `static void SupervisorMain() noexcept` | S-mode entry: enable interrupts + `wfi` idle loop |
| `static void OnTimerTick() noexcept` | Timer tick callback: `TickCount_++` |

### 4.2 Arch/CSR — `arch::csr::Manager` (`inc/Arch/CSR/CSRManager.hh`)

Singleton (`ISingleton<Manager>`), wraps S/M-mode CSR access.

**Public interface**

| Interface | Description |
| ---- | ---- |
| `uint64_t ReadSstatus()/ReadSepc()/ReadScause()/ReadStval()/ReadTime()/ReadMhartid()` | Read the corresponding CSR |
| `void WriteSstatus/WriteSepc/WriteStvec(uint64_t)` | Write CSR |
| `void SetStimecmpIntervalTicks()` | `stimecmp = time + INTERVAL_TICKS` (SSTC periodic timer) |
| `static bool IsFromSupervisorMode(uint64_t sstatus)` | Whether the trap came from S-mode (`SPP` bit) |
| `static uint64_t Cpuid()` | Read `tp` to get the current hartid |
| `void EnableInterrupts()/DisableInterrupts()` | Global S-mode interrupt toggle (sstatus.SIE) |
| `void EnableTimerInterrupt()/DisableTimerInterrupt()` | S-mode timer interrupt toggle (sie.STIE) |
| `[[noreturn]] void StartSupervisorMode(void (*pEntry)())` | M→S transition (see boot flow) |

**Private templates (compile-time CSR constants, single instruction)**: `writeCsr<Reg>`, `readCsr<Reg>`, `setCsrBits<Reg>` (`csrrs`), `clearCsrBits<Reg>` (`csrrc`).
To access a new CSR, just add an entry to the `Reg` enum and call the template — no runtime switch needed.

**Key enums**: `Reg` (SSTATUS…STIMECMP), `SstatusFlag`, `SieFlag`, `MedelegFlag`, `MidelegFlag`, `MenvcfgFlag`, `McounterenFlag`, `PmpFlag`, `Timer` (`INTERVAL_TICKS=1'000'000`), `MstatusMppFlag`.

### 4.3 Arch/Trap — `arch::trap::SupervisorTrap` (`inc/Arch/Trap/Trap.hh`, `src/Arch/Trap/Trap.cc`)

S-mode trap dispatch, **per-hart instance pool** (up to `G_kMaxHarts=8`), selected via `tp`/`Cpuid()`. Each instance holds the trap context: `Scause_` (union, bit63 = interrupt flag), `Epc_`, `Sstatus_`, `Stval_`, `HartId_`.

**Public interface**

| Interface | Description |
| ---- | ---- |
| `static SupervisorTrap &GetInstance() noexcept` | Instance for the current hart |
| `static SupervisorTrap &GetInstance(uint64_t hartid) noexcept` | Instance for a given hart (clamped to 0) |
| `void OnTrap() noexcept` | C-level trap entry (called by `s_trap_handler`), dispatches interrupt/exception, writes back sepc only when changed |
| `static void Init()` | Install stvec (direct mode, points to `s_trap_vector_base` in `trap_entry.S`) |
| `static void SetTickHandler(void (*pHandler)() noexcept) noexcept` | Register the timer tick callback (kernel extension point) |
| `static std::string_view ExceptionName(uint64_t code)` | Sync exception code -> name |

**Enums**: `SyncException` (INST_ADDR_MISALIGNED…ECALL_FROM_M_MODE), `InterruptCause` (SUPERVISOR_SOFTWARE/TIMER/EXTERNAL).

**Trap assembly entry** (`src/Arch/Trap/trap_entry.S`, direct-mode stvec):
saves caller-saved regs (`ra/gp/tp/t0-t6/a0-a7`) -> `call s_trap_handler` -> restore -> `sret`.
On entry the hardware already updated sstatus (SPIE=SIE, SIE=0, SPP=previous mode) and sepc; `sret` restores them.

**Exception/interrupt semantics**:
- Interrupts: TIMER -> `SetStimecmpIntervalTicks()` + invoke tick callback; SOFTWARE/EXTERNAL ignored.
- Exceptions: BREAKPOINT / ECALL_FROM_S_MODE -> `Epc_ += 4` (skip the instruction and resume); all others -> `panic()` (`wfi` infinite loop).

### 4.4 HAL/Device (`inc/HAL/Device/`)

Two device abstractions: **new-style (concept + type-erased Handle)** and **legacy (virtual IDevice)**, both supported by `dev::Manager`.

**`dev::Handle` (`DevHandle.hpp`)** — type-erased device port. Constructed via `Handle(D &impl)` which generates function-pointer thunks driven by concepts.

| Interface | Description |
| ---- | ---- |
| `int Init()/void Deinit()` | Lifecycle |
| `std::string_view GetName()` | Device name |
| `std::optional<uint8_t> GetChar()/int PutChar(uint8_t)/int Puts(string_view)` | Character stream |
| `int SetConfig(const drv::uart::Config&)/int GetConfig(drv::uart::Config&)` | Serial config |
| `bool IsCharDevice()/IsSerialDevice()` | Capability query |
| `explicit operator bool()` | Validity |

**`dev::Manager` (`DevManager.hh`)** — device registry singleton, capacity `G_kMaxDevices=32`.

| Interface | Description |
| ---- | ---- |
| `int Register(const dev::Handle&)` / `int Register(IDevice*)` | Register (dedup by name/pointer) |
| `int InitAll()/void DeinitAll()` | Batch init/deinit |
| `dev::Handle FindHandle(string_view)` / `IDevice *Find(string_view)` | Look up by name |
| `std::size_t GetCount()` | Number registered |

**Legacy virtual interfaces**: `IDevice` (Init/Deinit/GetName/Suspend/Resume/Control), `IChar` (+GetChar/PutChar/Puts), `IBlock` (marker), `ISerial` (+SetConfig/GetConfig/GetIRQ). New code should prefer concept + Handle.

**Static-polymorphic interfaces (`DeviceCRTP.hpp`, deducing-this)**: `Char` (`Puts(this Self&&, text)`), `Serial` (`SetConfig/GetConfig(this Self&&, …)` forwarded to the derived implementation + `virtual GetIRQ()`). Derived classes no longer need a template base argument.

**Concepts (`inc/Util/Concepts/DeviceCep.hh`)**: `DeviceLifecycle`, `CharReadable`, `CharWritable`, `CharDevice`, `SerialConfigurable`, `SerialDevice`; `BusCep.hh`: `hal::concepts::Bus`.

### 4.5 HAL/Drivers/Serial — NS16550A (`inc/HAL/Drivers/Serial/ns16550A.hh`)

`class NS16550A final : public Serial`, a 16550A UART driver based on the `MMIO<uint8_t>` register block `ns16x50::Regs` (rthrDll_/ierDlm_/iirFcr_/lcr_/mcr_/lsr_/msr_/scr_).

| Interface | Description |
| ---- | ---- |
| `NS16550A(uintptr_t base, uint8_t irq)` / `(base, irq, uint32_t clockHz)` | Construct (with clock frequency) |
| `int Init()/void Deinit()` | Init/close (idempotent) |
| `std::string_view GetName() const` | Device name |
| `std::optional<uint8_t> GetChar()/int PutChar(uint8_t)` | Char read/write |
| `int SetConfig(const drv::uart::Config&)/GetConfig(drv::uart::Config&) const` | Baud/data-bits/parity config |
| `ns16x50::Regs *GetRegs() const` | Raw register access |

Satisfies `static_assert(cep::SerialDevice<NS16550A>)`.

**Config structs (`UartConfig.hh`)**: `drv::uart::Config{baudRate_, dataBits_, stopBits_, parity_, flowControl_}`; `drv::hw::Config{cfg_, base_, irq_, clockHz_}`.

### 4.6 HAL/MMIO (`inc/HAL/MMIO/MMIO.hpp`)

Register wrapper: `MMIO<T>` (by value, typically inside a register-block object) and `PMMIO<T>` (constructed from an address).

| Interface | Description |
| ---- | ---- |
| `T Read()/void Write(T)` | Read/write (volatile, cannot be optimized away) |
| `void SetBits(T mask)/ClearBits(T mask)/ToggleBits(T mask)` | Bit operations |
| `void Set(pos)/Reset(pos)` | Set/clear a single bit |
| `operator=(T)` / `operator T()` | Assignment/read sugar |
| `GetAddr()` | Return the underlying address/value pointer |

### 4.7 Util

**`ISingleton<T>` (`inc/Util/ISingleton.hpp`)** — CRTP singleton base. Derive: `class Foo : public ISingleton<Foo>`; construct with `explicit Foo(Access a) : ISingleton<Foo>(a) {}` (or `using ISingleton<Foo>::ISingleton;`). `Instance()` returns the unique instance; copy/move are deleted. **Note: only for truly unique global singletons, not per-hart instance pools (e.g. `SupervisorTrap`).**

### 4.8 Platform (`inc/Platform/MemLayout.hh`)

`namespace dts::qemu`: `G_UART0_BASE=0x10000000`, `G_UART0_IRQ=10`, `G_UART0_CLOCK_HZ=1843200`, plus the global `NS16550A g_uart0(...)`. The QEMU virt memory map is documented in the file header.

### 4.9 Tests

| Target/file | Content |
| ---- | ---- |
| `tests/arch/trap/TrapSync.cc` | S-mode synchronous exception tests (store/load fault, misaligned, illegal inst, breakpoint, ecall), entered via `StartSupervisorMode` |
| `tests/hal/drv/uart0.cc` | NS16550A integration tests (Init/name/IRQ/IO/config/LCR/MCR/Deinit) |
| `tests/inc/Checks.hpp` | `tst::Checks`: `Check(cond, msg)` / formatted `Check` / `Report` / `Exit` |
| `tests/picolibc/` | Early UART print (`uart_putc` etc.) and test-platform startup |

---

## 5. Key Runtime Flows

### 5.1 Timer tick flow

```
SSTC timer expires (sie.STIE=1 and sstatus.SIE=1)
  └─> stvec -> trap_entry.S (save context; SIE already cleared by hardware)
        └─> s_trap_handler (extern "C")
              └─> SupervisorTrap::GetInstance().OnTrap()
                    ├─ read scause/sepc/stval/sstatus
                    ├─ handleInterrupt(SUPERVISOR_TIMER)
                    │     ├─ CSRManager::SetStimecmpIntervalTicks()   // re-arm next period
                    │     └─ g_tickHandler() -> Kernel::OnTimerTick()  // TickCount_++
                    └─ sepc unchanged, no write-back
  └─> sret (hardware restores SIE) -> back to wfi
```

### 5.2 Synchronous exception flow

```
ecall/ebreak/illegal inst ... -> trap_entry.S -> s_trap_handler -> OnTrap()
  └─> handleException()
        ├─ BREAKPOINT / ECALL_FROM_S_MODE -> sepc += 4 -> write back sepc -> sret to continue
        └─ others -> panic() (wfi infinite loop)
```

---

## 6. Coding Conventions

- Follow `docs/Naming_conventions.md` for names, types, member suffixes, comments (classes UpCamelCase, members `Xxx_`, constants `UPPER_CASE`, pointer prefix `p`, bool prefix `Is/Has/Can/En`, etc.).
- Prefer modern constructs in new code: concept constraints, `std::bit_cast`/`std::to_underlying`, deducing-this (P0847), `ISingleton` for singletons.
- Headers `.hh`, implementations `.cc`; RISC-V inline assembly is confined to the CSR/MMIO wrappers.

---

## 7. Known Limitations & Next Steps

- No task/scheduler: a `wfi` idle loop with tick counting; no context switch, ready queue, synchronization primitives, soft timers, or syscalls yet.
- No PLIC/peripheral interrupts: only the SSTC timer interrupt; external/software interrupts are ignored in `handleInterrupt`.
- No dynamic memory allocator (only stack/heap regions from the linker script).
- `src/Arch/Boot/os_start.S` calls the undefined `KernelInit` and is not used by the current build (the actual entry is picolibc `_start`); pending confirmation for removal.
- Multi-hart notes: `SupervisorTrap` is a per-hart instance pool, but the tick callback is currently a module-level global (valid for single-core).
