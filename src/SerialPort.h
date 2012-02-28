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
#ifndef _SERIALPORT_H
#define _SERIALPORT_H

#include <string>
#include <memory>
#include <stdint.h>

class SerialPort
{
public:
    SerialPort(const std::string& name) : _name(name) {}
    virtual ~SerialPort() {}

    enum Parity
    {
        ParityNone,
        ParityOdd,
        ParityEven,
    };

    enum StopBit
    {
        StopBitOne,
        StopBitOneFive,
        StopBitTwo,
    };

    virtual bool open(int baud = 115200,
                      int data = 8,
                      Parity parity = ParityNone,
                      StopBit stop = StopBitOne) = 0;
    virtual void close() = 0;

    virtual bool isUsb() = 0;

    virtual int read(uint8_t* data, int size) = 0;
    virtual int write(const uint8_t* data, int size) = 0;
    virtual int get() = 0;
    virtual int put(int c) = 0;

    virtual bool timeout(int millisecs) = 0;
    virtual void flush() = 0;

    virtual std::string name() const { return _name; }

    typedef std::auto_ptr<SerialPort> Ptr;

protected:
    std::string _name;
};

#endif // _SERIALPORT_H
