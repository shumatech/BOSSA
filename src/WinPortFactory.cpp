///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2012, ShumaTech
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
