#ifndef _POSIXSERIALPORT_H
#define _POSIXSERIALPORT_H

#include "SerialPort.h"

class PosixSerialPort : public SerialPort
{
public:
    PosixSerialPort(const std::string& name, bool isUsb);
    virtual ~PosixSerialPort();
    
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
    int _devfd;
    bool _isUsb;
    int _timeout;
};

#endif // _POSIXSERIALPORT_H
