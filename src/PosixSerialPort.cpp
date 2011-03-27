#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include <string>

using namespace std;

class LinuxSerialEnum
{
public:
    LinuxSerialEnum();
    virtual ~LinuxSerialEnum();

    string begin();
    string next();
    string end();

private:
    DIR* _dir;
};

LinuxSerialEnum::LinuxSerialEnum()
{
    _dir = opendir("/dev");
}

LinuxSerialEnum::~LinuxSerialEnum()
{
    if (_dir)
        closedir(_dir);
}

string
LinuxSerialEnum::begin()
{
    struct dirent* entry;

    if (!_dir)
        return end();

    rewinddir(_dir);

    return next();
}

string
LinuxSerialEnum::next()
{
    struct dirent* entry;

    if (!_dir)
        return end();

    while ((entry = readdir(_dir)))
    {
        if (strncmp("ttyUSB", entry->d_name, sizeof("ttyUSB") - 1) == 0)
            return string(entry->d_name);
        else if (strncmp("ttyS", entry->d_name, sizeof("ttyS") - 1) == 0)
            return string(entry->d_name);
    }

    return end();
}

string
LinuxSerialEnum::end()
{
    return string();
}

class PosixSerialPort
{
public:
    PosixSerialPort(const char* dev, int baud);
    virtual ~PosixSerialPort();

    int read(void *buffer, int len);
    int write(const void *buffer, int len);

private:
    int _devfd;
    int _timeout;
};

PosixSerialPort::PosixSerialPort(const char* dev, int baud) : _timeout(0)
{
    string devPath("/dev/");
    struct termios options;
    speed_t speed;

    devPath += dev;

    _devfd = open(devPath.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (_devfd == -1)
        throw string("open");
   
    if (tcgetattr(_devfd, &options) == -1)
    {
        close(_devfd);
        _devfd = -1;
        throw string("tcgetattr");
    }

    switch (baud)
    {
    case 115200:
        speed = B115200;
        break;
    case 921600:
        speed = B921600;
        break;
    default:
        throw string("baud");
    }

    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);
   
    options.c_cflag |= (CLOCAL | CREAD);

    // No parity
    options.c_cflag &= ~PARENB;
    //options.c_iflag |= (INPCK | ISTRIP); if parity

    // Stop bit
    options.c_cflag &= ~CSTOPB;

    // 8 bits
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
   
    // No hardware flow control
    options.c_cflag &= ~CRTSCTS;

    // No software flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Raw input
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Raw output
    options.c_oflag &= ~OPOST;

    // No wait time
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 0;

    if (tcsetattr(_devfd, TCSANOW, &options) == -1)
    {
        close(_devfd);
        _devfd = -1;
        throw string("tcsetattr");
    }
}

PosixSerialPort::~PosixSerialPort()
{
    if (_devfd != -1)
        close(_devfd);
}

int
PosixSerialPort::read(void *buffer, int len)
{
    fd_set fds;
    struct timeval timeout;
    int numread = 0;
    int retval;

    if (_devfd == -1)
        return -1;

    while (numread < len)
    {
        FD_ZERO(&fds);
        FD_SET(_devfd, &fds);
       
        timeout.tv_sec  = 0;
        timeout.tv_usec = _timeout * 1000;
       
        retval = select(_devfd + 1, &fds, NULL, NULL, &timeout);
       
        if (retval < 0)
            return -1;
        else if (retval == 0)
            return numread;

        retval = ::read(_devfd, (uint8_t*) buffer + numread, len - numread);
        if (retval < 0)
            return -1;
        numread += retval;
    }
}

int
PosixSerialPort::write(const void *buffer, int len)
{
    if (_devfd == -1)
        return -1;

    return ::write(_devfd, buffer, len);
}

int
main(int argc, char* argv[])
{
    LinuxSerialEnum serialEnum;
    string portIter;

    for (portIter = serialEnum.begin();
         portIter != serialEnum.end();
         portIter = serialEnum.next())
    {
        printf("Open %s\n", portIter.c_str());
        try
        {
            PosixSerialPort port(portIter.c_str(), 115200);
            port.write("ls\n", 3);
            sleep(1);
            char buffer[1024];
            int bytes = port.read(buffer, sizeof(buffer) - 1);
            printf("read %d bytes\n", bytes);
            buffer[bytes] = '\0';
            printf("Buffer:\n%s\n", buffer);
        }
        catch (string& s)
        {
            printf("Exception: %s\n", s.c_str());
        }
    }

    return 0;
}

