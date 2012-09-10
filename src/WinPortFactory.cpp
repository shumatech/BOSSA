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
#include "WinPortFactory.h"
#include "WinSerialPort.h"

#define USB_DEVICE_NAME "\\Device\\USB"

WinPortFactory::WinPortFactory() :
    _devInfo(INVALID_HANDLE_VALUE), _cfgMgr(NULL), _devNode(NULL), _devNum(0)
{
}

WinPortFactory::~WinPortFactory()
{
    cleanup();
}

SerialPort::Ptr
WinPortFactory::create(const std::string& name)
{
    bool isUsb = false;
    char szNtDeviceName[MAX_PATH];

    if (QueryDosDevice(name.c_str(), szNtDeviceName, MAX_PATH))
    {
        if (strncmp(szNtDeviceName, USB_DEVICE_NAME, sizeof(USB_DEVICE_NAME) - 1) == 0)
        {
            isUsb = true;
        }
    }

    return create(name, isUsb);
}

SerialPort::Ptr
WinPortFactory::create(const std::string& name, bool isUsb)
{
    return SerialPort::Ptr(new WinSerialPort(name, isUsb));
}

void
WinPortFactory::cleanup()
{
    _devNum = 0;
    _devNode = NULL;

    if (_cfgMgr == NULL)
    {
        FreeLibrary(_cfgMgr);
        _cfgMgr = NULL;
    }

    if (_devInfo != INVALID_HANDLE_VALUE)
    {
        SetupDiDestroyDeviceInfoList(_devInfo);
        _devInfo = INVALID_HANDLE_VALUE;
    }
}

std::string
WinPortFactory::error()
{
    cleanup();
    return end();
}

std::string
WinPortFactory::begin()
{
    DWORD size = 0;

    if (_devInfo != INVALID_HANDLE_VALUE)
        cleanup();

    SetupDiClassGuidsFromNameA("Ports", 0, 0, &size);
    if (size < 1)
        return error();

    GUID guids[size];

    if (!SetupDiClassGuidsFromNameA("Ports", guids, size * sizeof(GUID), &size))
    {
        return error();
    }

    _devInfo = SetupDiGetClassDevs(guids, NULL, NULL, DIGCF_PRESENT);
    if(_devInfo == INVALID_HANDLE_VALUE)
        return error();

    _cfgMgr = LoadLibrary("cfgmgr32");
    if (!_cfgMgr)
        return error();

    _devNode = (CM_Open_DevNode_Key) GetProcAddress(_cfgMgr, "CM_Open_DevNode_Key");
    if (!_devNode)
        return error();

    return next();
}

std::string
WinPortFactory::end()
{
    return std::string();
}

std::string
WinPortFactory::next()
{
    int rc;
    BYTE devName[16];
    SP_DEVINFO_DATA devData;
    HKEY devKey;
    DWORD len;

    if (_devInfo == INVALID_HANDLE_VALUE)
        return end();

    while (1)
    {
        devData.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(_devInfo, _devNum, &devData))
            return error();

        rc = _devNode(devData.DevInst,
                      KEY_QUERY_VALUE,
                      0,
                      1,
                      &devKey,
                      0);

        if (rc != ERROR_SUCCESS)
            return error();

        len = sizeof(devName);
        rc = RegQueryValueEx(devKey,
                             "portname",
                             NULL,
                             NULL,
                             devName,
                             &len);

        RegCloseKey(devKey);
        if (rc != ERROR_SUCCESS)
            return error();

        _devNum++;

        if (strncmp("COM", (char*) devName, 3) == 0)
            break;
    }

    return std::string((char*) devName, len);
}
