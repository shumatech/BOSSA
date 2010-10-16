#ifndef _LINUXPORTFACTORY_H
#define _LINUXPORTFACTORY_H

#include "SerialPort.h"

#include <sys/types.h>
#include <dirent.h>

#include <string>


class LinuxPortFactory
{
public:
    LinuxPortFactory();
    virtual ~LinuxPortFactory();
    
    virtual std::string begin();
    virtual std::string end();
    virtual std::string next();
    
    virtual SerialPort::Ptr create(const std::string& name);
    
private:
    std::string _empty;
    DIR* _dir;
};

#endif // _LINUXPORTFACTORY_H
