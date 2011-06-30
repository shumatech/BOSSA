#ifndef _SAMBA_H
#define _SAMBA_H

#include <string>
#include <stdint.h>
#include <exception>
#include <memory>

#include "SerialPort.h"

class SambaError : public std::exception
{
public:
    SambaError() : exception() {};
    const char* what() const throw() { return "SAM-BA operation failed"; }
};

class Samba
{
public:
    Samba();
    virtual ~Samba();
    
    bool connect(SerialPort::Ptr port);
    void disconnect();
    
    void writeByte(uint32_t addr, uint8_t value);
    uint8_t readByte(uint32_t addr);
    
    void writeWord(uint32_t addr, uint32_t value);
    uint32_t readWord(uint32_t addr);
    
    void write(uint32_t addr, const uint8_t* buffer, int size);
    void read(uint32_t addr, uint8_t* buffer, int size);
    
    void go(uint32_t addr);
    
    std::string version();
    
    uint32_t chipId();

    void setDebug(bool debug) { _debug = debug; }
    
    const SerialPort& getSerialPort() { return *_port; }
    
private:
    bool _debug;
    bool _isUsb;
    bool _writeLast;
    SerialPort::Ptr _port;
    
    bool init();
    
    uint16_t crc16Calc(const uint8_t *data, int len);
    bool crc16Check(const uint8_t *blk);
    void crc16Add(uint8_t *blk);
    void writeXmodem(const uint8_t* buffer, int size);
    void readXmodem(uint8_t* buffer, int size);

    void writeBinary(const uint8_t* buffer, int size);
    void readBinary(uint8_t* buffer, int size);
};

#endif // _SAMBA_H
