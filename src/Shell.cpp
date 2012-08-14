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
#include <stdio.h>
#include <exception>

#include "Shell.h"
#include "Command.h"

using namespace std;

Shell::Shell() :
    _exitFlag(false)
{
    Command::setShell(this);

    add(new CommandBod);
    add(new CommandBootf);
    add(new CommandBor);
    add(new CommandConnect);
    add(new CommandDebug);
    add(new CommandDisass);
    add(new CommandDump);
    add(new CommandErase);
    add(new CommandExit);
    add(new CommandGo);
    add(new CommandHelp);
    add(new CommandHistory);
    add(new CommandLock);
    add(new CommandInfo);
    add(new CommandMrb);
    add(new CommandMrf);
    add(new CommandMrw);
    add(new CommandMwb);
    add(new CommandMwf);
    add(new CommandMww);
    add(new CommandPio);
    add(new CommandRead);
    add(new CommandScan);
    add(new CommandSecurity);
    add(new CommandVerify);
    add(new CommandWrite);
    add(new CommandReset);

    _commandList.sort();
}

Shell::~Shell()
{
    CommandList::iterator it;

    for (it = _commandList.begin();
         it != _commandList.end();
         it++)
    {
        delete *it;
    }
}

Command*
Shell::find(const char* name)
{
    CommandList::iterator it;
    int len;
    Command *command = NULL;

    len = strlen(name);
    for (it = _commandList.begin();
        it != _commandList.end();
        it++)
    {
        if (strncmp((*it)->name(), name, len) == 0)
        {
            if (command)
            {
                printf("Ambiguous command: \"%s\".  Try \"help\".\n", name);
                return NULL;
            }
            command = *it;
        }
    }

    if (!command)
    {
        printf("Undefined command: \"%s\".  Try \"help\".\n", name);
        return NULL;
    }

    return command;
}

void
Shell::invoke(char* argv[], int argc)
{
    Command* command;

    command = find(argv[0]);
    if (!command)
        return;

    try
    {
        command->invoke(argv, argc);
    }
    catch (SambaError& e)
    {
        printf("\n%s.\nPort is disconnected.\n", e.what());
        Command::disconnect();
    }
    catch (exception& e)
    {
        printf("\n%s.\n", e.what());
    }
}

void
Shell::help()
{
    CommandList::iterator it;

    for (it = _commandList.begin();
        it != _commandList.end();
        it++)
    {
        printf("%s -- %s\n", (*it)->name(), (*it)->help());
    }
}

void
Shell::usage(const char* name)
{
    Command* command;

    command = find(name);
    if (!command)
        return;

    printf("%s\n", command->help());
    printf("Usage: %s\n", command->usage());
}

void
Shell::add(Command* command)
{
    _commandList.push_back(command);
}
