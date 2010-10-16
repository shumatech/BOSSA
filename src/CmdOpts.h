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
