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
#ifndef _Shell_H
#define _Shell_H

#include <string>
#include <list>


class Command;

class Shell
{
public:
    virtual ~Shell();
    Shell();

    void invoke(char* argv[], int argc);
    void help();
    void usage(const char* name);
    void add(Command* command);
    Command* find(const char* name);

    bool& exitFlag() { return _exitFlag; }

private:
    typedef std::list<Command*> CommandList;
    CommandList _commandList;
    bool _exitFlag;
};

#endif // _Shell_H
