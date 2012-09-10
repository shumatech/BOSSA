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
#ifndef _WINPORTFACTORY_H
#define _WINPORTFACTORY_H

class WinPortFactory;
#include "PortFactory.h"

#include <windows.h>
#include <setupapi.h>

class WinPortFactory : public PortFactoryBase
{
public:
    WinPortFactory();
    virtual ~WinPortFactory();

    std::string begin();
    std::string end();
    std::string next();

    SerialPort::Ptr create(const std::string& name);
    SerialPort::Ptr create(const std::string& name, bool isUsb);

private:
    typedef DWORD WINAPI (*CM_Open_DevNode_Key)(DWORD, DWORD, DWORD, DWORD, ::PHKEY, DWORD);

    HDEVINFO _devInfo;
    HINSTANCE _cfgMgr;
    CM_Open_DevNode_Key _devNode;
    int _devNum;

    void cleanup();
    std::string error();
};

#endif // _WINPORTFACTORY_H
