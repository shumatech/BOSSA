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
