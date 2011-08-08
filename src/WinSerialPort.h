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
