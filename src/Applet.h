#ifndef _APPLET_H
#define _APPLET_H

#include <stdint.h>

#include "Samba.h"

class Applet
{
public:
    Applet(Samba& samba,
           uint32_t addr,
           uint8_t* code,
           uint32_t size,
           uint32_t start,
           uint32_t stack,
           uint32_t reset);
    virtual ~Applet() {}

    virtual uint32_t size() { return _size; }
    virtual uint32_t addr() { return _addr; }
    
    virtual void setStack(uint32_t stack);
    
    virtual void run();
    virtual void runv();

protected:
    Samba& _samba;
    uint32_t _addr;
    uint32_t _size;
    uint32_t _start;
    uint32_t _stack;
    uint32_t _reset;
};

#endif // _APPLET_H
