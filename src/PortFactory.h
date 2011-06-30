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
    virtual std::string end() = 0;
    virtual std::string next() = 0;
    
    virtual SerialPort::Ptr create(const std::string& name) = 0;
};

#if defined(__WIN32__)
#include "WinPortFactory.h"
typedef WinPortFactory PortFactory;
#elif defined(__linux__)
#include "LinuxPortFactory.h"
typedef LinuxPortFactory PortFactory;
#elif defined(__APPLE__)
#include "OSXPortFactory.h"
typedef OSXPortFactory PortFactory;
#else
#error "Platform is not supported"
#endif

#endif // _PORTFACTORY_H
