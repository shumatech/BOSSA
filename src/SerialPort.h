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
    
    virtual bool timeout(int millisecs) = 0;
    virtual int read(uint8_t* data, int size) = 0;
    virtual int write(const uint8_t* data, int size) = 0;
    virtual int get() = 0;
    virtual int put(int c) = 0;

    virtual std::string name() const { return _name; }
    
    typedef std::auto_ptr<SerialPort> Ptr;
    
protected:
    std::string _name;
};

#endif // _SERIALPORT_H
