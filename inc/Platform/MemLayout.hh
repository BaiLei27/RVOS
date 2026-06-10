/**
 * @file MemoryMap.hh
 * @brief QEMU virt machine physical memory map (board-specific)
 */
#pragma once

#include <cstdint>

#include "Drivers/Serial/ns16550A.hh"

/**
 * see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[]
    static const MemMapEntry virt_memmap[]= {
        [VIRT_DEBUG]       = { 0x0,        0x100                             },
        [VIRT_MROM]        = { 0x1000,     0xf000                            },
        [VIRT_TEST]        = { 0x100000,   0x1000                            },
        [VIRT_RTC]         = { 0x101000,   0x1000                            },
        [VIRT_CLINT]       = { 0x2000000,  0x10000                           },
        [VIRT_ACLINT_SSWI] = { 0x2F00000,  0x4000                            },
        [VIRT_PCIE_PIO]    = { 0x3000000,  0x10000                           },
        [VIRT_IOMMU_SYS]   = { 0x3010000,  0x1000                            },
        [VIRT_PLATFORM_BUS]= { 0x4000000,  0x2000000                         },
        [VIRT_PLIC]        = { 0xc000000,  VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },
        [VIRT_APLIC_M]     = { 0xc000000,  APLIC_SIZE(VIRT_CPUS_MAX)         },
        [VIRT_APLIC_S]     = { 0xd000000,  APLIC_SIZE(VIRT_CPUS_MAX)         },
        [VIRT_UART0]       = { 0x10000000, 0x100                             },
        [VIRT_VIRTIO]      = { 0x10001000, 0x1000                            },
        [VIRT_FW_CFG]      = { 0x10100000, 0x18                              },
        [VIRT_FLASH]       = { 0x20000000, 0x4000000                         },
        [VIRT_IMSIC_M]     = { 0x24000000, VIRT_IMSIC_MAX_SIZE               },
        [VIRT_IMSIC_S]     = { 0x28000000, VIRT_IMSIC_MAX_SIZE               },
        [VIRT_PCIE_ECAM]   = { 0x30000000, 0x10000000                        },
        [VIRT_PCIE_MMIO]   = { 0x40000000, 0x40000000                        },
        [VIRT_DRAM]        = { 0x80000000, 0x0                               },
    };

 * 0x00001000 -- boot ROM, provided by qemu
 * 0x02000000 -- CLINT
 * 0x0C000000 -- PLIC
 * 0x10000000 -- UART0
 * 0x10001000 -- virtio disk
 * 0x80000000 -- boot ROM jumps here in machine mode, where we load our kernel
 */
namespace dts::qemu {

inline constexpr uintptr_t G_UART0_BASE= 0x10000000;
inline constexpr uint32_t G_UART0_SIZE = 0x100;
inline constexpr uint8_t G_UART0_IRQ   = 10;
/** QEMU models 16550 with classic 1.8432 MHz baud clock. */
inline constexpr uint32_t G_UART0_CLOCK_HZ= 1843200;

NS16550A g_uart0(G_UART0_BASE, G_UART0_IRQ, G_UART0_CLOCK_HZ);

} // namespace dts::qemu
