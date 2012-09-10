///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (C) 2011-2012 ShumaTech http://www.shumatech.com/
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////
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
    virtual SerialPort::Ptr create(const std::string& name, bool isUsb) = 0;
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
