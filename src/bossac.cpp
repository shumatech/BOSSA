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
#include <string>
#include <exception>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "CmdOpts.h"
#include "Samba.h"
#include "PortFactory.h"
#include "FlashFactory.h"
#include "Flasher.h"

using namespace std;

class BossaConfig
{
public:
    BossaConfig();
    virtual ~BossaConfig() {}

    bool erase;
    bool write;
    bool read;
    bool verify;
    bool port;
    bool boot;
    bool bor;
    bool bod;
    bool lock;
    bool unlock;
    bool security;
    bool info;
    bool debug;
    bool help;

    int readArg;
    string portArg;
    int bootArg;
    int bodArg;
    int borArg;
    string lockArg;
    string unlockArg;
};

BossaConfig::BossaConfig()
{
    erase = false;
    write = false;
    read = false;
    verify = false;
    port = false;
    boot = false;
    bod = false;
    bor = false;
    lock = false;
    security = false;
    info = false;
    help = false;

    readArg = 0;
    bootArg = 1;
    bodArg = 1;
    borArg = 1;
}

static BossaConfig config;
static Option opts[] =
{
    {
      'e', "erase", &config.erase,
      { ArgNone },
      "erase the entire flash"
    },
    {
      'w', "write", &config.write,
      { ArgNone },
      "write FILE to the flash; accelerated when\n"
      "combined with erase option"
    },
    {
      'r', "read", &config.read,
      { ArgOptional, ArgInt, "SIZE", { &config.readArg } },
      "read SIZE from flash and store in FILE;\n"
      "read entire flash if SIZE not specified"
    },
    {
      'v', "verify", &config.verify,
      { ArgNone },
      "verify FILE matches flash contents"
    },
    {
      'p', "port", &config.port,
      { ArgRequired, ArgString, "PORT", { &config.portArg } },
      "use serial PORT to communicate to device;\n"
      "default behavior is to auto-scan all serial ports"
    },
    {
      'b', "boot", &config.boot,
      { ArgOptional, ArgInt, "BOOL", { &config.bootArg } },
      "boot from ROM if BOOL is 0;\n"
      "boot from FLASH if BOOL is 1 [default];\n"
      "option is ignored on unsupported devices"
    },
    {
      'c', "bod", &config.bod,
      { ArgOptional, ArgInt, "BOOL", { &config.bodArg } },
      "no brownout detection if BOOL is 0;\n"
      "brownout detection is on if BOOL is 1 [default]"
    },
    {
      't', "bor", &config.bor,
      { ArgOptional, ArgInt, "BOOL", { &config.borArg } },
      "no brownout reset if BOOL is 0;\n"
      "brownout reset is on if BOOL is 1 [default]"
    },
    {
      'l', "lock", &config.lock,
      { ArgOptional, ArgString, "REGION", { &config.lockArg } },
      "lock the flash REGION as a comma-separated list;\n"
      "lock all if not given [default]"
    },
    {
      'u', "unlock", &config.unlock,
      { ArgOptional, ArgString, "REGION", { &config.unlockArg } },
      "unlock the flash REGION as a comma-separated list;\n"
      "unlock all if not given [default]"
    },
    {
      's', "security", &config.security,
      { ArgNone },
      "set the flash security flag"
    },
    {
      'i', "info", &config.info,
      { ArgNone },
      "display device information"
    },
    {
      'd', "debug", &config.debug,
      { ArgNone },
      "print debug messages"
    },
    {
      'h', "help", &config.help,
      { ArgNone },
      "display this help text"
    }
};

bool
autoScan(Samba& samba, PortFactory& portFactory, string& port)
{
    for (port = portFactory.begin();
         port != portFactory.end();
         port = portFactory.next())
    {
        if (config.debug)
            printf("Trying to connect on %s\n", port.c_str());
        if (samba.connect(portFactory.create(port)))
            return true;
    }

    return false;
}

int
help(const char* program)
{
    fprintf(stderr, "Try '%s -h' or '%s --help' for more information\n", program, program);
    return 1;
}

int
main(int argc, char* argv[])
{
    int args;
    char* pos;
    CmdOpts cmd(argc, argv, sizeof(opts) / sizeof(opts[0]), opts);

    if ((pos = strrchr(argv[0], '/')) || (pos = strrchr(argv[0], '\\')))
        argv[0] = pos + 1;

    if (argc <= 1)
    {
        fprintf(stderr, "%s: you must specify at least one option\n", argv[0]);
        return help(argv[0]);
    }

    args = cmd.parse();
    if (args < 0)
        return help(argv[0]);

    if (config.read && (config.write || config.verify))
    {
        fprintf(stderr, "%s: read option is exclusive of write or verify\n", argv[0]);
        return help(argv[0]);
    }

    if (config.read || config.write || config.verify)
    {
        if (args == argc)
        {
            fprintf(stderr, "%s: missing file\n", argv[0]);
            return help(argv[0]);
        }
        argc--;
    }
    if (args != argc)
    {
        fprintf(stderr, "%s: extra arguments found\n", argv[0]);
        return help(argv[0]);
    }

    if (config.help)
    {
        printf("Usage: %s [OPTION...] [FILE]\n", argv[0]);
        printf("Basic Open Source SAM-BA Application (BOSSA) Version " VERSION "\n"
               "Flash programmer for Atmel SAM devices.\n"
               "Copyright (c) 2011-2012 ShumaTech (http://www.shumatech.com)\n"
               "\n"
               "Examples:\n"
               "  bossac -e -w -v -b image.bin   # Erase flash, write flash with image.bin,\n"
               "                                 # verify the write, and set boot from flash\n"
               "  bossac -r0x10000 image.bin     # Read 64KB from flash and store in image.bin\n"
              );
        printf("\nOptions:\n");
        cmd.usage(stdout);
        printf("\nReport bugs to <bugs@shumatech.com>\n");
        return 1;
    }

    try
    {
        Samba samba;
        PortFactory portFactory;
        FlashFactory flashFactory;

        if (config.debug)
            samba.setDebug(true);

        if (config.port)
        {
            if (!samba.connect(portFactory.create(config.portArg)))
            {
                fprintf(stderr, "No device found on %s\n", config.portArg.c_str());
                return 1;
            }
        }
        else
        {
            string port;
            if (!autoScan(samba, portFactory, port))
            {
                fprintf(stderr, "Auto scan for device failed\n");
                fprintf(stderr, "Try specifying a serial port with the '-p' option\n");
                return 1;
            }
            printf("Device found on %s\n", port.c_str());
        }

        uint32_t chipId = samba.chipId();
        Flash::Ptr flash = flashFactory.create(samba, chipId);
        if (flash.get() == NULL)
        {
            fprintf(stderr, "Flash for chip ID %08x is not supported\n", chipId);
            return 1;
        }

        Flasher flasher(flash);

        if (config.unlock)
            flasher.lock(config.unlockArg, false);

        if (config.erase)
            flasher.erase();

        if (config.write)
            flasher.write(argv[args]);

        if (config.verify)
            if  (!flasher.verify(argv[args]))
                return 2;

        if (config.read)
            flasher.read(argv[args], config.readArg);

        if (config.boot)
        {
            printf("Set boot flash %s\n", config.bootArg ? "true" : "false");
            flash->setBootFlash(config.bootArg);
        }

        if (config.bod)
        {
            printf("Set brownout detect %s\n", config.bodArg ? "true" : "false");
            flash->setBod(config.bodArg);
        }

        if (config.bor)
        {
            printf("Set brownout reset %s\n", config.borArg ? "true" : "false");
            flash->setBor(config.borArg);
        }

        if (config.security)
        {
            printf("Set security\n");
            flash->setSecurity();
        }

        if (config.lock)
            flasher.lock(config.lockArg, true);

        if (config.info)
            flasher.info(samba);
    }
    catch (exception& e)
    {
        fprintf(stderr, "\n%s\n", e.what());
        return 1;
    }
    catch(...)
    {
        fprintf(stderr, "\nUnhandled exception\n");
        return 1;
    }

    return 0;
}

