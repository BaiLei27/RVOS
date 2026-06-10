// #include <cstdio>
#include "Drivers/Serial/ns16550A.hh"

NS16550A::NS16550A(uintptr_t base, uint8_t irq)
    : Serial<NS16550A>(irq),
      Regs_(std::launder(reinterpret_cast<ns16x50::Regs *>(base))),
      PhysAddr_(base)
{
    // Cfg_.baudRate_= 38400;
}

NS16550A::NS16550A(uintptr_t base, uint8_t irq, uint32_t clockHz): NS16550A(base, irq)
{
    ClockHz_= clockHz;
}

int NS16550A::Init()
{
    if(HasInit_) {
        return 0;
    }

    // Regs_= reinterpret_cast<Regs *>(PhysAddr_);
    // Regs_= std::launder(reinterpret_cast<ns16x50::Regs *>(PhysAddr_));

    if(!Regs_) {
        return -1;
    }

    Regs_->ierDlm_.Write(0x00); // disable all interrupts
    this->applyUartConfig();

    Regs_->iirFcr_.Write(ns16x50::FCR::FIFOEN
                         | ns16x50::FCR::RCVRCLR
                         | ns16x50::FCR::XMITCLR);

    // Regs_->ierDlm_.Write(ns16x50::IER::RXEN | ns16x50::IER::TXEN);

    HasInit_= true;
    return 0;
}

void NS16550A::Deinit()
{
    Regs_->ierDlm_.Write(0x00U);
    HasInit_= false;
}

std::string_view NS16550A::GetName() const noexcept
{
    return "uart0";
}

std::optional<uint8_t> NS16550A::GetChar()
{
    if(!isRxReady()) {
        return std::nullopt;
    }
    return Regs_->rthrDll_.Read();
}

int NS16550A::PutChar(uint8_t c)
{
    while(!isTxEmpty()) {
    }
    Regs_->rthrDll_.Write(c);

    return 0;
}

int NS16550A::SetConfig(const drv::uart::Config &cfg)
{
    if(cfg.baudRate_ == 0 || cfg.dataBits_ < 5 || cfg.dataBits_ > 8) {
        return -1;
    }

    Cfg_= cfg;
    if(HasInit_) {
        this->applyUartConfig();
    }
    return 0;
}

int NS16550A::GetConfig(drv::uart::Config &cfg) const
{
    cfg= Cfg_;
    return 0;
}

void NS16550A::applyUartConfig()
{
    if(Cfg_.baudRate_ == 0 || ClockHz_ == 0) {
        return;
    }
    this->calcDivisor();
    this->setBaudRate();
    Regs_->lcr_.Write(buildLcr()); // data bits, stop bits, parity, clear DLAB
    Regs_->ierDlm_.Write(0x00);    // restore interrupts to default

    using ns16x50::MCR;
    uint8_t mcr= MCR::OP2 | MCR::RTS | MCR::DTR;

    if(0x02 == Cfg_.flowControl_) {
        mcr|= MCR::AFCE; // auto flow control enable
    }

    Regs_->mcr_.Write(mcr);
}

void NS16550A::setBaudRate()
{
    Regs_->lcr_.SetBits(ns16x50::LCR::DLAB); //	divisor latch enable
    Regs_->rthrDll_.Write(Divisor_ & 0xFFU); // LSB
    Regs_->ierDlm_.Write(Divisor_ >> 8);     // MSB
}

uint8_t NS16550A::buildLcr() const
{
    using ns16x50::LCR;
    uint8_t lcr= 0;

    switch(Cfg_.dataBits_) {
    case 5:  break;
    case 6:  lcr|= LCR::WLS1; break;
    case 7:  lcr|= LCR::WLS2; break;
    default: lcr|= LCR::WLS1 | LCR::WLS2; break; // 8 data bits
    }

    if(2 == Cfg_.stopBits_) {
        lcr|= LCR::STB;
    }

    switch(Cfg_.parity_) {
    case 1:  lcr|= LCR::PEN; break;            // odd
    case 2:  lcr|= LCR::PEN | LCR::EPS; break; // even
    default: break;                            // none
    }

    return lcr;
}

uint16_t NS16550A::calcDivisor()
{
    /*
     * calculate baud rate divisor. a variant of
     * (uint32_t)(pclk / (16.0 * baud_rate) + 0.5)
     */
    return (Divisor_= ((ClockHz_ + (Cfg_.baudRate_ << 3)) / Cfg_.baudRate_) >> 4);
}

bool NS16550A::isRxReady() const { return (Regs_->lsr_.Read() & ns16x50::LSR::DR) != 0x00; }

bool NS16550A::isTxEmpty() const { return (Regs_->lsr_.Read() & ns16x50::LSR::THE) != 0x00; }

ns16x50::Regs *NS16550A::GetRegs() const { return Regs_; }
