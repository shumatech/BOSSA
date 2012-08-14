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
#ifndef _COMMAND_H
#define _COMMAND_H

#include <stdint.h>

#include "Shell.h"
#include "Samba.h"
#include "PortFactory.h"
#include "FlashFactory.h"
#include "Flasher.h"

class Command
{
public:
    Command(const char* name, const char* help, const char* usage);
    virtual ~Command() {}

    virtual void invoke(char* argv[], int argc) = 0;
    const char* name() { return _name; };
    const char* help() { return _help; };
    const char* usage() { return _usage; };

    static void setShell(Shell* shell) { _shell = shell; }
    static void disconnect();

    bool operator < (const Command& rhs);

protected:
    static Shell* _shell;
    static Samba _samba;
    static PortFactory _portFactory;
    static FlashFactory _flashFactory;
    static Flash::Ptr _flash;
    static Flasher _flasher;
    static bool _connected;

    bool error(const char* fmt, ...);
    bool argNum(int argc, int num);
    bool argRange(int argc, int min, int max);
    bool argUint32(const char* arg, uint32_t* value);
    bool argBool(const char* arg, bool* value);
    bool argState(const char* arg, bool* value);
    
    bool createFlash();
    bool connected();
    bool flashable();

    void hexdump(uint32_t addr, uint8_t *buf, size_t count);
    const char* binstr(uint32_t value, int bits, char low = '0', char high = '1');

private:
    const char* _name;
    const char* _help;
    const char* _usage;
};

class CommandBod : public Command
{
public:
    CommandBod();
    virtual void invoke(char* argv[], int argc);
};

class CommandBootf : public Command
{
public:
    CommandBootf();
    virtual void invoke(char* argv[], int argc);
};

class CommandBor : public Command
{
public:
    CommandBor();
    virtual void invoke(char* argv[], int argc);
};

class CommandConnect : public Command
{
public:
    CommandConnect();
    virtual void invoke(char* argv[], int argc);
};

class CommandDebug : public Command
{
public:
    CommandDebug();
    virtual void invoke(char* argv[], int argc);
};

class CommandDisass : public Command
{
public:
    CommandDisass();
    virtual void invoke(char* argv[], int argc);
};

class CommandDump : public Command
{
public:
    CommandDump();
    virtual void invoke(char* argv[], int argc);
};
class CommandErase : public Command
{
public:
    CommandErase();
    virtual void invoke(char* argv[], int argc);
};

class CommandExit : public Command
{
public:
    CommandExit();
    virtual void invoke(char* argv[], int argc);
};

class CommandGo : public Command
{
public:
    CommandGo();
    virtual void invoke(char* argv[], int argc);
};

class CommandHelp : public Command
{
public:
    CommandHelp();
    virtual void invoke(char* argv[], int argc);
};

class CommandHistory : public Command
{
public:
    CommandHistory();
    virtual void invoke(char* argv[], int argc);
};

class CommandInfo : public Command
{
public:
    CommandInfo();
    virtual void invoke(char* argv[], int argc);
};

class CommandLock : public Command
{
public:
    CommandLock();
    virtual void invoke(char* argv[], int argc);
};

class CommandMrb : public Command
{
public:
    CommandMrb();
    virtual void invoke(char* argv[], int argc);
};

class CommandMrf : public Command
{
public:
    CommandMrf();
    virtual void invoke(char* argv[], int argc);
};

class CommandMrw : public Command
{
public:
    CommandMrw();
    virtual void invoke(char* argv[], int argc);
};

class CommandMwb : public Command
{
public:
    CommandMwb();
    virtual void invoke(char* argv[], int argc);
};

class CommandMwf : public Command
{
public:
    CommandMwf();
    virtual void invoke(char* argv[], int argc);
};

class CommandMww : public Command
{
public:
    CommandMww();
    virtual void invoke(char* argv[], int argc);
};

class CommandPio : public Command
{
public:
    CommandPio();
    virtual void invoke(char* argv[], int argc);
};

class CommandRead : public Command
{
public:
    CommandRead();
    virtual void invoke(char* argv[], int argc);
};

class CommandScan : public Command
{
public:
    CommandScan();
    virtual void invoke(char* argv[], int argc);
};

class CommandSecurity : public Command
{
public:
    CommandSecurity();
    virtual void invoke(char* argv[], int argc);
};

class CommandUnlock : public Command
{
public:
    CommandUnlock();
    virtual void invoke(char* argv[], int argc);
};

class CommandVerify : public Command
{
public:
    CommandVerify();
    virtual void invoke(char* argv[], int argc);
};

class CommandWrite : public Command
{
public:
    CommandWrite();
    virtual void invoke(char* argv[], int argc);
};

class CommandReset : public Command
{
public:
    CommandReset();
    virtual void invoke(char* argv[], int argc);
};

#endif // _COMMAND_H
