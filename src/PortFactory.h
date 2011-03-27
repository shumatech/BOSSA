#ifndef _PORTFACTORY_H
#define _PORTFACTORY_H

#include <string>

#include "SerialPort.h"

class PortFactoryBase
{
public:
    PortFactoryBase() {}
    virtual ~PortFactoryBase() {}
    
    virtual std::string begin() = 0;
    virtual const std::string& end() = 0;
    virtual std::string next() = 0;
    
    virtual SerialPort::Ptr create(const std::string& name) = 0;
};

#ifdef __WIN32__
#include "WinPortFactory.h"
typedef WinPortFactory PortFactory;
#else
#error "Platform is not supported"
#endif

#endif // _PORTFACTORY_H
