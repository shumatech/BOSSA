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
#ifndef _OPTION_H
#define _OPTION_H

#include <string>
#include <stdio.h>

typedef enum
{
    ArgNone,
    ArgOptional,
    ArgRequired
} ArgHas;

typedef enum
{
    ArgInt,
    ArgString
} ArgType;

typedef struct
{
    ArgHas has;
    ArgType type;
    const char* name;
    union
    {
        void* voidPtr;
        int* intPtr;
        std::string* strPtr;
    } value;
} OptArg;

typedef struct
{
    char letter;
    const char* name;
    bool* present;
    OptArg arg;
    const char* help;
} Option;

class CmdOpts
{
public:
    CmdOpts(int argc, char* argv[], int numOpts, Option* opts);
    virtual ~CmdOpts();

    void usage(FILE* out);
    int parse();

private:
    int _argc;
    char** _argv;
    int _numOpts;
    Option* _opts;

    int find(char letter);
};

#endif // _OPTION_H
