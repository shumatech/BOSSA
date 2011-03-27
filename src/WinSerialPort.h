#ifndef _WINSERIALPORT_H
#define _WINSERIALPORT_H

#include "SerialPort.h"

#include <memory>
#include <windows.h>

class WinSerialPort : public SerialPort
{
public:
    WinSerialPort(const std::string& name);
    virtual ~WinSerialPort();
    
    bool open(int baud = 115200,
              int data = 8,
              SerialPort::Parity parity = SerialPort::ParityNone,
              SerialPort::StopBit stop = SerialPort::StopBitOne);
    void close();
    void flush();
    bool isUsb() { return _isUsb; };

    bool timeout(int millisecs);
    int read(uint8_t* data, int size);
    int write(const uint8_t* data, int size);
    int getc();
    int putc(int c);

private:
    HANDLE _handle;
    bool _isUsb;
};

#endif // _SERIALPORT_H
