#include "ISA/InstFormat.hh"

InstLayout::InstLayout(uint32_t instruction): entity_(instruction) { }

InstLayout &InstLayout::operator= (uint32_t instruction)
{
    entity_= instruction;
    return *this;
}

InstLayout::operator uint32_t () const
{
    return entity_;
}
