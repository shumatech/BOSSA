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
#include <stdlib.h>
#include <ctype.h>
#include <exception>
#include <readline/readline.h>
#include <readline/history.h>

#include "Shell.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static int
split(char* str, char* tokv[], int tokn)
{
    int tokc;

    for (tokc = 0; tokc < tokn - 1; tokc++)
    {
        while (*str && isspace(*str))
            str++;

        if (!*str)
            break;

        tokv[tokc] = str;

        while (*str && !isspace(*str))
            str++;

        if (*str)
            *str++ = '\0';
    }

    if (*str)
        tokv[tokc++] = str;

    return tokc;
}

int
main(int argc, char* argv[])
{
    char *input;
    char *str;
    char *tokv[5];
    int tokc;
    char *expansion;
    int result;

    printf("Press Ctrl-D or enter \"exit\" to end session.\n"
           "Enter \"help\" to display a command list.\n");

    using_history();

    try
    {
        Shell shell;

        while (!shell.exitFlag())
        {
            input = readline("bossa> ");
            if (!input)
            {
                printf("\n");
                break;
            }

            for (str = input; *str && isspace(*str); str++);

            if (*str)
            {
                result = history_expand(input, &expansion);
                if (result >= 0 && result != 2)
                {
                    add_history(expansion);
                    tokc = split(expansion, tokv, ARRAY_SIZE(tokv));
                    shell.invoke(tokv, tokc);
                }
                free(expansion);
            }
            free(input);
        }
    }
    catch(...)
    {
        printf("\nUnhandled exception\n");
        return 1;
    }

    return 0;
}
