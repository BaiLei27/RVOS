#include "Drivers/Serial/ns16550A.hh"
#include "Platform/MemLayout.hh"
#include "Checks.hpp"

namespace {

using dts::qemu::g_uart0;

[[nodiscard]] volatile uint8_t *uartMmio() noexcept
{
    return reinterpret_cast<volatile uint8_t *>(dts::qemu::G_UART0_BASE);
}

void testInit(tst::Checks &chk)
{
    chk.Check(g_uart0.Init() == 0, "Init idempotent");
}

void testName(tst::Checks &chk)
{
    chk.Check(g_uart0.GetName() == "uart0", "GetName");
}

void testIrq(tst::Checks &chk)
{
    chk.Check(g_uart0.GetIRQ() == static_cast<int>(dts::qemu::G_UART0_IRQ), "GetIRQ");
}

void testGetCharEmpty(tst::Checks &chk)
{
    chk.Check(!g_uart0.GetChar().has_value(), "GetChar empty when no RX data");
}

void testTx(tst::Checks &chk)
{
    chk.Check(g_uart0.PutChar('A') == 0, "PutChar");
            putchar('\n');
    chk.Check(g_uart0.Puts("BC\n") == 0, "Puts");

    auto lsr= g_uart0.GetRegs()->lsr_.Read();

    chk.Check((lsr & ns16x50::LSR::THE) != 0, "LSR.THE set after TX");
}

void testConfigDefaults(tst::Checks &chk)
{
    drv::uart::Config cfg {};
    chk.Check(g_uart0.GetConfig(cfg) == 0, "GetConfig");

    chk.Check(cfg.baudRate_ == 115200, "default baud 115200");
    chk.Check(cfg.dataBits_ == 8, "default 8 data bits");
    chk.Check(cfg.stopBits_ == 1, "default 1 stop bit");
    chk.Check(cfg.parity_ == 0, "default no parity");
}

void testSetConfigInvalid(tst::Checks &chk)
{
    drv::uart::Config bad {};
    bad.baudRate_= 0;
    chk.Check(g_uart0.SetConfig(bad) != 0, "SetConfig rejects baud 0");

    bad.baudRate_= 115200;
    bad.dataBits_= 4;
    chk.Check(g_uart0.SetConfig(bad) != 0, "SetConfig rejects dataBits < 5");

    bad.dataBits_= 9;
    chk.Check(g_uart0.SetConfig(bad) != 0, "SetConfig rejects dataBits > 8");
}

void testSetConfigRoundtrip(tst::Checks &chk)
{
    drv::uart::Config cfg { .baudRate_= 38400, .dataBits_= 8, .stopBits_= 1, .parity_= 0 };
    chk.Check(g_uart0.SetConfig(cfg) == 0, "SetConfig 38400");

    drv::uart::Config out {};
    chk.Check(g_uart0.GetConfig(out) == 0, "GetConfig after SetConfig");
    chk.Check(out.baudRate_ == 38400, "baud roundtrip");

    // restore default for subsequent register checks
    cfg.baudRate_= 115200;
    chk.Check(g_uart0.SetConfig(cfg) == 0, "restore default baud");
}

void testLcrAfterInit(tst::Checks &chk)
{
    auto lcr= g_uart0.GetRegs()->lcr_.Read();
    chk.Check((lcr & (ns16x50::LCR::WLS1 | ns16x50::LCR::WLS2)) != 0, "LCR 8-bit word length");
    chk.Check((lcr & ns16x50::LCR::DLAB) == 0, "LCR DLAB cleared");
    chk.Check((lcr & ns16x50::LCR::PEN) == 0, "LCR parity disabled");
}

void testMcrAfterInit(tst::Checks &chk)
{
    auto mcr= g_uart0.GetRegs()->mcr_.Read();
    chk.Check((mcr & ns16x50::MCR::DTR) != 0, "MCR DTR asserted");
    chk.Check((mcr & ns16x50::MCR::RTS) != 0, "MCR RTS asserted");
}

void testDeinitReinit(tst::Checks &chk)
{
    g_uart0.Deinit();

    auto ier= g_uart0.GetRegs()->ierDlm_.Read();
    chk.Check(ier == 0, "IER cleared after Deinit");

    chk.Check(g_uart0.Init() == 0, "re-Init after Deinit");
}

} // namespace

static_assert(cep::SerialDevice<NS16550A>);

int main()
{
    tst::Checks chk;

    testInit(chk);
    testName(chk);
    testIrq(chk);
    testGetCharEmpty(chk);
    testTx(chk);
    testConfigDefaults(chk);
    testSetConfigInvalid(chk);
    testSetConfigRoundtrip(chk);
    testLcrAfterInit(chk);
    testMcrAfterInit(chk);
    testDeinitReinit(chk);

    chk.Exit("uart0");

    exit(0);
}
