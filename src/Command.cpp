///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2012, ShumaTech
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#define __STDC_LIMIT_MACROS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "Command.h"

#define min(a, b)   ((a) < (b) ? (a) : (b))

using namespace std;

Shell* Command::_shell = NULL;
Samba Command::_samba;
PortFactory Command::_portFactory;
FlashFactory Command::_flashFactory;
Flash::Ptr Command::_flash;
Flasher Command::_flasher(_flash);
bool Command::_connected = false;

Command::Command(const char* name, const char* help, const char* usage) :
    _name(name), _help(help), _usage(usage)
{
    assert(_shell != NULL);
}

bool
Command::error(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf(".  Try \"help %s\".\n", _name);

    return false;
}

bool
Command::argNum(int argc, int num)
{
    if (argc != num)
        return error("Command requires %d argument%s",
                     num - 1, (num > 1) ? "s" : "", _name);

    return true;
}

bool
Command::argRange(int argc, int min, int max)
{
    if (argc < min || argc > max)
        return error("Command requires %d to %d arguments", min - 1, max - 1);

    return true;
}

bool
Command::argUint32(const char* arg, uint32_t* value)
{
    long long ll;
    char *end;

    errno = 0;
    ll = strtoll(arg, &end, 0);
    if (errno != 0 || *end != '\0')
        return error("Invalid number \"%s\"", arg);
    if (ll < 0 || ll > UINT32_MAX)
        return error("Number \"%s\" is out of range", arg);

    *value = ll;

    return true;
}

bool
Command::argBool(const char* arg, bool* value)
{
    int len = strlen(arg);
    if (strncasecmp(arg, "true", len) == 0)
        *value = true;
    else if (strncasecmp(arg, "false", len) == 0)
        *value = false;
    else
        return error("Invalid boolean \"%s\"", arg);

    return true;
}

bool
Command::argState(const char* arg, bool* value)
{
    int len = strlen(arg);
    if (strncasecmp(arg, "enable", len) == 0)
        *value = true;
    else if (strncasecmp(arg, "disable", len) == 0)
        *value = false;
    else
        return error("Invalid state \"%s\"", arg);

    return true;
}

bool
Command::connected()
{
    if (!_connected)
    {
        printf("No device connected.  Use \"connect\" or \"scan\" first.\n");
        return false;
    }
    return true;
}

bool
Command::flashable()
{
    if (!connected())
        return false;

    if (_flash.get() == NULL)
    {
        printf("Flash on device is not supported.\n");
        return false;
    }
    return true;
}

bool
Command::createFlash()
{
    uint32_t chipId = _samba.chipId();

    _flash = _flashFactory.create(_samba, chipId);
    if (_flash.get() == NULL)
    {
        printf("Flash for chip ID %08x is not supported\n", chipId);
        return false;
    }

    return true;
}

void
Command::hexdump(uint32_t addr, uint8_t *buf, size_t count)
{
    int lpad;
    int rpad;
    size_t size;
    size_t offset;
    const uint32_t ROW_SIZE = 16;
    const uint32_t ROW_MASK = ~(ROW_SIZE - 1);

    printf("            0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");

    while (count > 0)
    {
        lpad = (addr % ROW_SIZE);
        rpad = ROW_SIZE - min(lpad + count, ROW_SIZE);
        size = ROW_SIZE - rpad - lpad;

        printf("%08x | ", addr & ROW_MASK);

        printf("%*s", 3 * lpad, "");
        for (offset = 0; offset < size; offset++)
            printf("%02x ", buf[offset]);
        printf("%*s", 3 * rpad, "");

        printf("| ");

        printf("%*s", lpad, "");
        for (offset = 0; offset < size; offset++)
            printf("%c", isprint(buf[offset]) ? buf[offset] : '.');
        printf("%*s", rpad, "");

        printf("\n");

        buf += size;
        addr += size;
        count -= size;
    }
}

const char*
Command::binstr(uint32_t value, int bits, char low, char high)
{
    static char buf[36];
    char *str = buf;

    assert(bits <= 32 && bits > 0);

    for (int bitnum = bits - 1; bitnum >= 0; bitnum--)
    {
        *str++ = (value & (1 << bitnum)) ? high : low;
        if (bitnum % 8 == 0)
            *str++ = ' ';
    }
    *(str - 1) = '\0';

    return buf;
}

void
Command::disconnect()
{
    _connected = false;
}

bool
Command::operator < (const Command& rhs)
{
    return (strcmp(_name, rhs._name) == -1);
}

CommandBod::CommandBod() :
    Command("bod",
            "Change the brownout detect flag.",
            "bod [BOOL]\n"
            "  BOOL -- boolean value either \"true\" or \"false\"")
{}

void
CommandBod::invoke(char* argv[], int argc)
{
    bool value;
    
    if (!argNum(argc, 2) ||
        !argBool(argv[1], &value) ||
        !flashable())
        return;

    if (!_flash->canBod())
    {
        printf("Unsupported on this flash device\n");
        return;
    }

    _flash->setBod(value);
    printf("BOD flag set to %s\n", value ? "true" : "false");
}

CommandBootf::CommandBootf() :
    Command("bootf",
            "Change the boot to flash flag.",
            "bootf [BOOL]\n"
            "  BOOL -- boolean value either \"true\" or \"false\"")
{}

void
CommandBootf::invoke(char* argv[], int argc)
{
    bool value;
    
    if (!argNum(argc, 2) ||
        !argBool(argv[1], &value) ||
        !flashable())
        return;

    _flash->setBootFlash(value);
    printf("Boot to flash flag set to %s\n", value ? "true" : "false");
}

CommandBor::CommandBor() :
    Command("bor",
            "Change the brownout reset flag.",
            "bor [BOOL]\n"
            "  BOOL -- boolean value either \"true\" or \"false\"")
{}

void
CommandBor::invoke(char* argv[], int argc)
{
    bool value;
    
    if (!argNum(argc, 2) ||
        !argBool(argv[1], &value) ||
        !flashable())
        return;

    if (!_flash->canBor())
    {
        printf("Unsupported on this flash device\n");
        return;
    }

    _flash->setBor(value);
    printf("BOR flag set to %s\n", value ? "true" : "false");
}

CommandConnect::CommandConnect() :
    Command("connect",
            "Connect to device over serial port.",
            "connect [SERIAL]\n"
            "  SERIAL -- host-specific serial port")
{}

void
CommandConnect::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 2))
        return;

    if (!_samba.connect(_portFactory.create(argv[1])))
    {
        printf("No device found on %s\n", argv[1]);
        _connected = false;
        return;
    }

    printf("Connected to device on %s\n", argv[1]);
    _connected = true;
    createFlash();
}

CommandDebug::CommandDebug() :
    Command("debug",
            "Change the debug state.",
            "debug [STATE]\n"
            "  STATE - either \"disable\" or \"enable\"")
{}

void
CommandDebug::invoke(char* argv[], int argc)
{
    bool state;

    if (!argNum(argc, 2) ||
        !argState(argv[1], &state))
        return;

    _samba.setDebug(state);
}

CommandDump::CommandDump() :
    Command("dump",
            "Dump memory in hexadecimal and ascii.",
            "dump [ADDRESS] [COUNT]\n"
            "  ADDRESS -- starting memory address\n"
            "  COUNT -- count of bytes to display")
{}

void
CommandDump::invoke(char* argv[], int argc)
{
    uint32_t addr;
    uint32_t count;
    uint8_t* buf;

    if (!argNum(argc, 3) ||
        !argUint32(argv[1], &addr) ||
        !argUint32(argv[2], &count) ||
        !connected())
        return;

    buf = (uint8_t*) malloc(count);
    if (!buf)
        return;

    try
    {
        _samba.read(addr, buf, count);
    }
    catch (...)
    {
        free(buf);
        throw;
    }

    hexdump(addr, buf, count);
    free(buf);
}

CommandErase::CommandErase() :
    Command("erase",
            "Erase the entire flash.",
            "erase")
{}

void
CommandErase::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 1) ||
        !flashable())
        return;

    _flasher.erase();
    printf("Flash is erased\n");
}

CommandExit::CommandExit() :
    Command("exit",
            "Exit the BOSSA shell.",
            "exit")
{}

void
CommandExit::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 1))
        return;

    _shell->exitFlag() = true;
}

CommandGo::CommandGo() :
    Command("go",
            "Execute ARM code at address.",
            "go [ADDRESS]\n"
            "  ADDRESS -- starting memory address of code to execute")
{}

void
CommandGo::invoke(char* argv[], int argc)
{
    uint32_t addr;

    if (!argNum(argc, 2) ||
        !argUint32(argv[1], &addr) ||
        !connected())
        return;

    printf("Executing code at %#x\n", addr);
    _samba.go(addr);
}

CommandHelp::CommandHelp() :
    Command("help",
            "Display help for a command.",
            "help <COMMAND>\n"
            "  COMMAND -- (optional) display detailed usage for this command,\n"
            "             display summary help for all commands if not given")
{}

void
CommandHelp::invoke(char* argv[], int argc)
{
    if (!argRange(argc, 1, 2))
        return;

    if (argc == 1)
        _shell->help();
    else
        _shell->usage(argv[1]);
}

CommandHistory::CommandHistory() :
    Command("history",
            "List the command history.",
            "history")
{}

void
CommandHistory::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 1))
        return;
printf("history_base=%d\n", history_base);
    for (int i = history_base; i < history_base + history_length; i++)
    {
        HIST_ENTRY *entry = history_get(i);
        if (entry)
            printf ("  %d  %s\n", i, entry->line);
    }
}

CommandInfo::CommandInfo() :
    Command("info",
            "Display information about the flash.",
            "info")
{}

void
CommandInfo::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 1) ||
        !flashable())
        return;

    _flasher.info(_samba);
}

CommandLock::CommandLock() :
    Command("lock",
            "Set lock bits in the flash.",
            "lock <BITS>"
            "  BITS -- (optional) comma separated list of bits,"
            "          all bits if not given\n")
{}

void
CommandLock::invoke(char* argv[], int argc)
{
    string bits;

    if (!flashable())
        return;

    for (int argn = 1; argn < argc; argn++)
        bits += argv[argn];

    _flasher.lock(bits, true);
    printf("Locked regions %s", bits.c_str());
}

CommandMrb::CommandMrb() :
    Command("mrb",
            "Read bytes from memory.",
            "mrb [ADDRESS] <COUNT>\n"
            "  ADDRESS -- starting memory address\n"
            "  COUNT -- (optional) count of bytes to display, 1 if not given")
{}

void
CommandMrb::invoke(char* argv[], int argc)
{
    uint32_t addr;
    uint32_t count = 1;
    uint8_t value;

    if (!argRange(argc, 2, 3) ||
        !argUint32(argv[1], &addr) ||
        (argc == 3 && !argUint32(argv[2], &count)) ||
        !connected())
        return;

    while (count > 0)
    {
        value = _samba.readByte(addr);
        printf("%08x : %02x  %s\n", addr, value, binstr(value, 8));
        addr++;
        count--;
    }
}

CommandMrf::CommandMrf() :
    Command("mrf",
            "Read memory to file.",
            "mrf [ADDRESS] [COUNT] [FILE]\n"
            "  ADDRESS -- memory address to read\n"
            "  COUNT -- count of bytes to read\n"
            "  FILE -- file name on host filesystem to write")
{}

void
CommandMrf::invoke(char* argv[], int argc)
{
    uint32_t addr;
    uint32_t count;
    FILE* infile;
    uint8_t buf[1024];
    ssize_t fbytes;

    if (!argNum(argc, 4) ||
        !argUint32(argv[1], &addr) ||
        !argUint32(argv[2], &count) ||
        !connected())
        return;

    infile = fopen(argv[3], "wb");
    if (!infile)
        throw FileOpenError(errno);

    try
    {
        while (count > 0)
        {
            fbytes = min(count, sizeof(buf));
            _samba.read(addr, buf, fbytes);
            fbytes = fwrite(buf, 1, fbytes, infile);
            if (fbytes < 0)
                throw FileIoError(errno);
            if ((size_t) fbytes != min(count, sizeof(buf)))
                throw FileShortError();
            count -= fbytes;
        }
    }
    catch (...)
    {
        fclose(infile);
        throw;
    }

    fclose(infile);
}

CommandMrw::CommandMrw() :
    Command("mrw",
            "Read words from memory.",
            "mrw [ADDRESS] <COUNT>\n"
            "  ADDRESS -- starting memory address\n"
            "  COUNT -- (optional) count of words to display, 1 if not given")
{}

void
CommandMrw::invoke(char* argv[], int argc)
{
    uint32_t addr;
    uint32_t count = 1;
    uint32_t value;

    if (!argRange(argc, 2, 3) ||
        !argUint32(argv[1], &addr) ||
        (argc == 3 && !argUint32(argv[2], &count)) ||
        !connected())
        return;

    while (count > 0)
    {
        value = _samba.readWord(addr);
        printf("%08x : %08x  %s\n", addr, value, binstr(value, 32));
        addr += 4;
        count--;
    }
}

CommandMwb::CommandMwb() :
    Command("mwb",
            "Write bytes to memory.",
            "mwb [ADDRESS] <VALUE>\n"
            "  ADDRESS -- starting memory address\n"
            "  VALUE -- (optional) value of byte to write, if not given"
            "           command will repeatedly prompt for input")
{}

void
CommandMwb::invoke(char* argv[], int argc)
{
    uint32_t addr;
    uint32_t value;

    if (!argRange(argc, 2, 3) ||
        !argUint32(argv[1], &addr) ||
        (argc == 3 && !argUint32(argv[2], &value)) ||
        !connected())
        return;

    do
    {
        if (argc == 2)
        {
            char* input = readline("? ");
            if (!input)
                return;
            if (input == '\0' ||
                !argUint32(input, &value))
            {
                free(input);
                return;
            }
            free(input);
        }

        if (value > 255)
        {
            error("Value out of range");
            return;
        }

        _samba.writeByte(addr, value);
        printf("%08x : %02x\n", addr, value);
        addr++;
    } while (argc == 2);
}

CommandMwf::CommandMwf() :
    Command("mwf",
            "Write memory from file.",
            "mwf [ADDRESS] [FILE]\n"
            "  ADDRESS -- memory address to write\n"
            "  FILE -- file name on host filesystem to read")
{}

void
CommandMwf::invoke(char* argv[], int argc)
{
    uint32_t addr;
    FILE* infile;
    uint8_t buf[1024];
    ssize_t fsize;
    ssize_t fbytes;
    ssize_t fpos;

    if (!argNum(argc, 3) ||
        !argUint32(argv[1], &addr) ||
        !connected())
        return;

    infile = fopen(argv[2], "rb");
    if (!infile)
        throw FileOpenError(errno);

    try
    {
        if (fseek(infile, 0, SEEK_END) != 0 ||
            (fsize = ftell(infile)) < 0)
            throw FileIoError(errno);

        rewind(infile);

        for (fpos = 0; fpos < fsize; fpos += fbytes)
        {
            fbytes = fread(buf, 1, min((size_t)fsize, sizeof(buf)), infile);
            if (fbytes < 0)
                throw FileIoError(errno);
            if (fbytes == 0)
                break;
            _samba.write(addr, buf, fbytes);
        }
    }
    catch (...)
    {
        fclose(infile);
        throw;
    }
    fclose(infile);
    printf("Wrote %ld bytes to address %08x\n", (long) fsize, addr);
}

CommandMww::CommandMww() :
    Command("mww",
            "Write words to memory.",
            "mww [ADDRESS] <VALUE>\n"
            "  ADDRESS -- starting memory address\n"
            "  VALUE -- (optional) value of word to write, if not given"
            "           command will repeatedly prompt for input")
{}

void
CommandMww::invoke(char* argv[], int argc)
{
    uint32_t addr;
    uint32_t value;

    if (!argRange(argc, 2, 3) ||
        !argUint32(argv[1], &addr) ||
        (argc == 3 && !argUint32(argv[2], &value)) ||
        !connected())
        return;

    do
    {
        if (argc == 2)
        {
            char* input = readline("? ");
            if (!input)
                return;
            if (input == '\0' ||
                !argUint32(input, &value))
            {
                free(input);
                return;
            }
            free(input);
        }

        _samba.writeWord(addr, value);
        printf("%08x : %08x\n", addr, value);
        addr++;
    } while (argc == 2);
}

CommandPio::CommandPio() :
    Command("pio",
            "Parallel input/output operations.",
            "pio [LINE] [OPERATION]\n"
            "  LINE -- PIO line name (i.e. pa28, pc5, etc.)\n"
            "          All lines if only port given (i.e. pa, pc, etc.)\n"
            "  OPERATION -- operation to perform on the PIO line.\n"
            "    status -- show the line status\n"
            "    high -- drive the output high\n"
            "    low -- drive the output low\n"
            "    read -- read the input level\n"
            "    input -- make the line an input\n"
            "    peripheral [AB] -- set the line to a peripheral\n"
            "      [AB] -- peripheral \"a\" or \"b\"\n"
            "    multidrive [STATE] -- set the multi-drive state\n"
            "      STATE - either \"disable\" or \"enable\"\n"
            "    pullup [STATE] -- set the pull-up state\n"
            "      STATE - either \"disable\" or \"enable\""
            )
{}

void
CommandPio::invoke(char* argv[], int argc)
{
    uint32_t line;
    uint32_t chipId;
    uint32_t eproc;
    uint32_t arch;
    uint32_t addr = 0;
    size_t len;
    char port;

    if (!argRange(argc, 3, 4) ||
        !connected())
        return;

    if (strlen(argv[1]) < 2 ||
        tolower(argv[1][0]) != 'p')
    {
        error("Invalid PIO line name");
        return;
    }

    if (argv[1][2] == '\0')
    {
        line = 0xffffffff;
    }
    else
    {
        if (!argUint32(&argv[1][2], &line))
            return;

        if (line >= 32)
        {
            error("Invalid PIO line number");
            return;
        }

        line = (1 << line);
    }
    
    port = tolower(argv[1][1]);

    chipId = _samba.chipId();
    eproc = (chipId >> 5) & 0x7;
    arch = (chipId >> 20) & 0xff;

    // Check for Cortex-M3 register set
    if (eproc == 3)
    {
        // Check for SAM3U special case
        if (arch >= 0x80 && arch <= 0x81)
        {
            switch (port)
            {
                case 'a': addr = 0x400e0c00; break;
                case 'b': addr = 0x400e0e00; break;
                case 'c': addr = 0x400e1000; break;
            }
        }
        else
        {
            switch (port)
            {
                case 'a': addr = 0x400e0e00; break;
                case 'b': addr = 0x400e1000; break;
                case 'c': addr = 0x400e1200; break;
            }
        }
    }
    else
    {
        switch (port)
        {
            case 'a': addr = 0xfffff400; break;
            case 'b': addr = 0xfffff600; break;
            case 'c': addr = 0xfffff800; break;
        }
    }

    if (addr == 0)
    {
        printf("Invalid PIO line \"%s\"\n", argv[1]);
        return;
    }

    static const uint32_t PIO_PER = 0x0;
    static const uint32_t PIO_PDR = 0x4;
    static const uint32_t PIO_PSR = 0x8;
    static const uint32_t PIO_OER = 0x10;
    static const uint32_t PIO_ODR = 0x14;
    static const uint32_t PIO_OSR = 0x18;
    static const uint32_t PIO_SODR = 0x30;
    static const uint32_t PIO_CODR = 0x34;
    static const uint32_t PIO_ODSR = 0x38;
    static const uint32_t PIO_PDSR = 0x3c;
    static const uint32_t PIO_MDER = 0x50;
    static const uint32_t PIO_MDDR = 0x54;
    static const uint32_t PIO_MDSR = 0x58;
    static const uint32_t PIO_PUDR = 0x60;
    static const uint32_t PIO_PUER = 0x64;
    static const uint32_t PIO_PUSR = 0x68;
    static const uint32_t PIO_ABSR = 0x70;

    len = strlen(argv[2]);
    if (strncasecmp(argv[2], "status", len) == 0)
    {
        if (line != 0xffffffff)
        {
            uint32_t reg = _samba.readWord(addr + PIO_PSR);
            printf("PIO Mode      : %s\n", (reg & line) ? "enable" : "disable");
            
            reg = _samba.readWord(addr + PIO_OSR);
            printf("Direction     : %s\n", (reg & line) ? "output" : "input");

            reg = _samba.readWord(addr + PIO_PDSR);
            printf("Input Level   : %s\n", (reg & line)? "high" : "low");

            reg = _samba.readWord(addr + PIO_ODSR);
            printf("Output Level  : %s\n", (reg & line)? "high" : "low");
            
            reg = _samba.readWord(addr + PIO_MDSR);
            printf("Multi-Drive   : %s\n", (reg & line)? "enable" : "disable");

            reg = _samba.readWord(addr + PIO_PUSR);
            printf("Pull-Up       : %s\n", (reg & line)? "disable" : "enable");

            reg = _samba.readWord(addr + PIO_ABSR);
            printf("Peripheral    : %s\n", (reg & line) ? "B" : "A");
        }
        else
        {
            printf("                3      2 2      1 1\n");
            printf("                1      4 3      6 5      8 7      0\n");

            uint32_t reg = _samba.readWord(addr + PIO_PSR);
            printf("PIO Mode      : %s\n", binstr(reg, 32, 'D', 'E'));
            
            reg = _samba.readWord(addr + PIO_OSR);
            printf("Direction     : %s\n", binstr(reg, 32, 'I', 'O'));

            reg = _samba.readWord(addr + PIO_PDSR);
            printf("Input Level   : %s\n", binstr(reg, 32, 'L', 'H'));

            reg = _samba.readWord(addr + PIO_ODSR);
            printf("Output Level  : %s\n", binstr(reg, 32, 'L', 'H'));
            
            reg = _samba.readWord(addr + PIO_MDSR);
            printf("Multi-Drive   : %s\n", binstr(reg, 32, 'D', 'E'));
            
            reg = _samba.readWord(addr + PIO_PUSR);
            printf("Pull-Up       : %s\n", binstr(reg, 32, 'E', 'D'));
            
            reg = _samba.readWord(addr + PIO_ABSR);
            printf("Peripheral    : %s\n", binstr(reg, 32, 'A', 'B'));
        }
    }
    else if (strncasecmp(argv[2], "high", len) == 0)
    {
        _samba.writeWord(addr + PIO_SODR, line);
        _samba.writeWord(addr + PIO_OER, line);
        _samba.writeWord(addr + PIO_PER, line);
        printf("%s is high output\n", argv[1]);
    }
    else if (strncasecmp(argv[2], "low", len) == 0)
    {
        _samba.writeWord(addr + PIO_CODR, line);
        _samba.writeWord(addr + PIO_OER, line);
        _samba.writeWord(addr + PIO_PER, line);
        printf("%s is low output\n", argv[1]);
    }
    else if (strncasecmp(argv[2], "read", len) == 0)
    {
        uint32_t reg = _samba.readWord(addr +  PIO_PDSR);
        printf("%s is %s\n", argv[1], (reg & line) ? "high" : "low");
    }
    else if (strncasecmp(argv[2], "input", len) == 0)
    {
        _samba.writeWord(addr + PIO_ODR, line);
        _samba.writeWord(addr + PIO_PER, line);
        printf("%s is an input\n", argv[1]);
    }
    else if (strncasecmp(argv[2], "peripheral", len) == 0)
    {
        uint32_t reg;
        
        if (!argNum(argc, 4))
            return;

        reg = _samba.readWord(addr + PIO_ABSR);
        if (strcasecmp(argv[3], "a") == 0)
            reg &= ~line;
        else if (strcasecmp(argv[3], "b") == 0)
            reg |= line;
        else
        {
            error("Peripheral must be \"a\" or \"b\"");
            return;
        }

        _samba.writeWord(addr + PIO_ABSR, reg);
        _samba.writeWord(addr + PIO_PDR, line);
        
        printf("%s set to peripheral %s\n", argv[1], argv[3]);
    }
    else if (strncasecmp(argv[2], "pullup", len) == 0)
    {
        bool state;
        if (!argNum(argc, 4) ||
            !argState(argv[3], &state))
            return;
        
        if (state)
            _samba.writeWord(addr + PIO_PUER, line);
        else
            _samba.writeWord(addr + PIO_PUDR, line);
        printf("%s pullup is %s\n", argv[1], argv[3]);
    }
    else if (strncasecmp(argv[2], "multidrive", len) == 0)
    {
        bool state;
        if (!argNum(argc, 4) ||
            !argState(argv[3], &state))
            return;
        
        if (state)
            _samba.writeWord(addr + PIO_MDER, line);
        else
            _samba.writeWord(addr + PIO_MDDR, line);
        printf("%s multidrive is %s\n", argv[1], argv[3]);
    }
    else
    {
        printf("Invalid PIO operation\n");
        return;
    }
}

CommandRead::CommandRead() :
    Command("read",
            "Read flash into a binary file.",
            "read [FILE] <COUNT>\n"
            "  FILE -- file name on host filesystem"
            "  COUNT -- (optional) count of bytes to read, defaults\n"
            "           to entire flash if not given")
{}

void
CommandRead::invoke(char* argv[], int argc)
{
    uint32_t count = 0;

    if (!argRange(argc, 2, 3) ||
        (argc == 3 && !argUint32(argv[2], &count)) ||
        !flashable())
        return;

    _flasher.read(argv[1], count);
}

CommandScan::CommandScan() :
    Command("scan",
            "Scan all serial ports for a device.",
            "scan")
{}

void
CommandScan::invoke(char* argv[], int argc)
{
    string port;

    if (!argNum(argc, 1))
        return;

    for (port = _portFactory.begin();
         port != _portFactory.end();
         port = _portFactory.next())
    {
        printf("Checking port %s...\n", port.c_str());
        if (_samba.connect(_portFactory.create(port)))
        {
            printf("Device found on %s\n", port.c_str());
            _connected = true;
            createFlash();
            return;
        }
    }

    _connected = false;

    printf("Auto scan for device failed.\n"
           "Try specifying a serial port with the \"connect\" command.\n");
}

CommandSecurity::CommandSecurity() :
    Command("security",
            "Enable the security flag.",
            "security")
{}

void
CommandSecurity::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 1) ||
        !flashable())
        return;

    _flash->setSecurity();
}

CommandUnlock::CommandUnlock() :
    Command("unlock",
            "Clear lock bits in the flash.",
            "unlock <BITS>"
            "  BITS -- (optional) comma separated list of bits,"
            "          all bits if not given\n")
{}

void
CommandUnlock::invoke(char* argv[], int argc)
{
    string bits;

    if (!flashable())
        return;

    for (int argn = 1; argn < argc; argn++)
        bits += argv[argn];

    _flasher.lock(bits, false);
    printf("Unlocked regions %s", bits.c_str());
}

CommandVerify::CommandVerify() :
    Command("verify",
            "Verify binary file with the flash.",
            "verify [FILE]\n"
            "  FILE -- file name on host filesystem")
{}

void
CommandVerify::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 2) ||
        !flashable())
        return;

    _flasher.verify(argv[1]);
}

CommandWrite::CommandWrite() :
    Command("write",
            "Write binary file into flash.",
            "write [FILE]\n"
            "  FILE -- file name on host filesystem")
{}

void
CommandWrite::invoke(char* argv[], int argc)
{
    if (!argNum(argc, 2) ||
        !flashable())
        return;

    _flasher.write(argv[1]);
}

CommandReset::CommandReset() :
    Command("reset",
            "Reset the CPU. (only for supported CPU)",
            "reset\n")
{}

void
CommandReset::invoke(char* argv[], int argc)
{
    _samba.reset();
}

