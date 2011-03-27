#include "Applet.h"

Applet::Applet(Samba& samba,
               uint32_t addr,
               uint8_t* code,
               uint32_t size,
               uint32_t start,
               uint32_t stack,
               uint32_t reset) :
    _samba(samba), _addr(addr), _size(size), _start(start), _stack(stack), _reset(reset)
{
    _samba.write(addr, code, size);
}

void
Applet::setStack(uint32_t stack)
{
    _samba.writeWord(_stack, stack);
}

void
Applet::run()
{
    // Add one to the start address for Thumb mode
    _samba.go(_start + 1);
}

void
Applet::runv()
{
    // Add one to the start address for Thumb mode
    _samba.writeWord(_reset, _start + 1);
    
    // The stack is the first reset vector
    _samba.go(_stack);
}
