#include "LinuxPortFactory.h"
#include "PosixSerialPort.h"

#include <string.h>
#include <stdio.h>

#include <string>

LinuxPortFactory::LinuxPortFactory()
{
    _dir = opendir("/dev");
}

LinuxPortFactory::~LinuxPortFactory()
{
    if (_dir)
        closedir(_dir);
}

SerialPort::Ptr
LinuxPortFactory::create(const std::string& name)
{
    return SerialPort::Ptr(new PosixSerialPort(name));
}

std::string
LinuxPortFactory::begin()
{
    if (!_dir)
        return end();

    rewinddir(_dir);

    return next();
}

std::string
LinuxPortFactory::next()
{
    struct dirent* entry;

    if (!_dir)
        return end();

    while ((entry = readdir(_dir)))
    {
        if (strncmp("ttyUSB", entry->d_name, sizeof("ttyUSB") - 1) == 0)
            return std::string(entry->d_name);
        else if (strncmp("ttyS", entry->d_name, sizeof("ttyS") - 1) == 0)
            return std::string(entry->d_name);
    }

    return end();
}

std::string
LinuxPortFactory::end()
{
    return std::string();
}
