#ifndef _OSXPORTFACTORY_H
#define _OSXPORTFACTORY_H

#include "SerialPort.h"

#include <sys/types.h>
#include <dirent.h>

#include <string>


class OSXPortFactory
{
public:
    OSXPortFactory();
    virtual ~OSXPortFactory();
    
    virtual std::string begin();
    virtual std::string end();
    virtual std::string next();
    
    virtual SerialPort::Ptr create(const std::string& name);
    
private:
    std::string _empty;
    DIR* _dir;
};

#endif // _OSXPORTFACTORY_H
