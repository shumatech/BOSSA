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
#ifndef _WINSERIALPORT_H
#define _WINSERIALPORT_H

#include "SerialPort.h"

#include <memory>
#include <windows.h>

class WinSerialPort : public SerialPort
{
public:
    WinSerialPort(const std::string& name, bool isUsb);
    virtual ~WinSerialPort();

    bool open(int baud = 115200,
              int data = 8,
              SerialPort::Parity parity = SerialPort::ParityNone,
              SerialPort::StopBit stop = SerialPort::StopBitOne);
    void close();

    bool isUsb() { return _isUsb; };

    int read(uint8_t* data, int size);
    int write(const uint8_t* data, int size);
    int get();
    int put(int c);

    bool timeout(int millisecs);
    void flush();

private:
    HANDLE _handle;
    bool _isUsb;
};

#endif // _WINSERIALPORT_H
