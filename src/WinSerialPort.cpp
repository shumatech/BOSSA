#include "WinSerialPort.h"

#include <string.h>
#include <assert.h>
#include <limits.h>

#define USB_DEVICE_NAME "\\Device\\USB"

WinSerialPort::WinSerialPort(const std::string& name) :
    SerialPort(name), _handle(INVALID_HANDLE_VALUE), _isUsb(false)
{
    char szNtDeviceName[MAX_PATH];
    if (QueryDosDevice(_name.c_str(), szNtDeviceName, MAX_PATH))
    {
        if (strncmp(szNtDeviceName, USB_DEVICE_NAME, sizeof(USB_DEVICE_NAME) - 1) == 0)
        {
            _isUsb = true;
        }
    }
}

WinSerialPort::~WinSerialPort()
{
    close();
}

bool
WinSerialPort::open(int baud, int data, SerialPort::Parity parity, SerialPort::StopBit stop)
{
    DCB dcbSerialParams;
    
    if (_handle != INVALID_HANDLE_VALUE)
        return false;
    
    _handle = CreateFile(_name.c_str(),
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
