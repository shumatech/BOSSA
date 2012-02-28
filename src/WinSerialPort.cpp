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
