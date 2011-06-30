#include "OSXPortFactory.h"
#include "PosixSerialPort.h"

#include <string.h>
#include <stdio.h>

#include <string>

OSXPortFactory::OSXPortFactory()
{
    _dir = opendir("/dev");
}

OSXPortFactory::~OSXPortFactory()
{
    if (_dir)
        closedir(_dir);
}

SerialPort::Ptr
OSXPortFactory::create(const std::string& name)
{
    bool isUsb = false;

    if (name.find("usb") != std::string::npos)
        isUsb = true;

    return SerialPort::Ptr(new PosixSerialPort(name, isUsb));
}

std::string
OSXPortFactory::begin()
{
    if (!_dir)
        return end();

    rewinddir(_dir);

    return next();
}

std::string
OSXPortFactory::next()
{
    struct dirent* entry;

    if (!_dir)
        return end();

    while ((entry = readdir(_dir)))
    {
        if (strncmp("cu.", entry->d_name, sizeof("cu.") - 1) == 0)
            return std::string(entry->d_name);
    }

    return end();
}

std::string
OSXPortFactory::end()
{
    return std::string();
}
