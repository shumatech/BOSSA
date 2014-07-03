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
#include "WinSerialPort.h"

#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>

WinSerialPort::WinSerialPort(const std::string& name, bool isUsb) :
    SerialPort(name), _handle(INVALID_HANDLE_VALUE), _isUsb(isUsb)
{
}

WinSerialPort::~WinSerialPort()
{
    close();
}

#ifdef DEBUG
static void
printLastError()
{
    char buffer[100];
    DWORD dw = GetLastError();

    if (FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        0,
        buffer,
        sizeof(buffer),
        NULL ))
    {
        printf("Error (%d): %s", (int) dw, (LPSTR) buffer);
    }
    else
    {
        printf("Error (%d)\n", (int) dw);
    }
}
#endif

bool
WinSerialPort::open(int baud, int data, SerialPort::Parity parity, SerialPort::StopBit stop)
{
    DCB dcbSerialParams;

    if (_handle != INVALID_HANDLE_VALUE)
        return false;

    std::string device = "\\\\.\\" + _name;
    _handle = CreateFile(device.c_str(),
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         0,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         0);

    if (_handle == INVALID_HANDLE_VALUE)
        return false;

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(_handle, &dcbSerialParams))
    {
        CloseHandle(_handle);
        return false;
    }

    dcbSerialParams.BaudRate = baud;

    dcbSerialParams.ByteSize = data;

    switch (parity)
    {
    case ParityNone:
        dcbSerialParams.Parity = NOPARITY;
        break;
    case ParityOdd:
        dcbSerialParams.Parity = ODDPARITY;
        break;
    case ParityEven:
        dcbSerialParams.Parity = EVENPARITY;
        break;
    default:
        CloseHandle(_handle);
        return false;
    }

    switch (stop)
    {
    case StopBitOne:
        dcbSerialParams.StopBits = ONESTOPBIT;
        break;
    case StopBitOneFive:
        dcbSerialParams.StopBits = ONE5STOPBITS;
        break;
    case StopBitTwo:
        dcbSerialParams.StopBits = TWOSTOPBITS;
        break;
    default:
        CloseHandle(_handle);
        return false;
    }

    if (!SetCommState(_handle, &dcbSerialParams))
    {
        CloseHandle(_handle);
        return false;
    }

    timeout(INT_MAX);

    return true;
}

void
WinSerialPort::close()
{
    if (_handle != INVALID_HANDLE_VALUE)
        CloseHandle(_handle);
    _handle = INVALID_HANDLE_VALUE;
}

bool
WinSerialPort::timeout(int millisecs)
{
    COMMTIMEOUTS timeouts;

    if (_handle == INVALID_HANDLE_VALUE)
        return false;

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = millisecs;
    timeouts.ReadTotalTimeoutMultiplier = 0;

    timeouts.WriteTotalTimeoutConstant = MAXDWORD;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    if (!SetCommTimeouts(_handle, &timeouts))
        return false;

    return true;
}

int
WinSerialPort::read(uint8_t* data, int size)
{
    DWORD bytes;
    int total = 0;

    if (_handle == INVALID_HANDLE_VALUE)
        return -1;

    while (size > 0)
    {
        if (!ReadFile(_handle, data, size, &bytes, NULL))
            return -1;
        if (bytes == 0)
            break;
        size -= bytes;
        data += bytes;
        total += bytes;
    }

    return total;
}

int
WinSerialPort::write(const uint8_t* data, int size)
{
    DWORD bytes;

    if (_handle == INVALID_HANDLE_VALUE)
        return -1;

    if (!WriteFile(_handle, data, size, &bytes, NULL))
        return -1;

    return bytes;
}

int
WinSerialPort::get()
{
    uint8_t val;
    DWORD bytes;

    if (_handle == INVALID_HANDLE_VALUE)
        return -1;

    if (!ReadFile(_handle, &val, 1, &bytes, NULL))
        return -1;

    if (bytes != 1)
        return -1;

    return val;
}

int
WinSerialPort::put(int c)
{
    uint8_t val = c;
    DWORD bytes;

    if (_handle == INVALID_HANDLE_VALUE)
        return -1;

    if (!WriteFile(_handle, &val, 1, &bytes, NULL))
        return -1;

    if (bytes != 1)
        return -1;

    return val;
}

void
WinSerialPort::flush()
{
    Sleep(1);
}
