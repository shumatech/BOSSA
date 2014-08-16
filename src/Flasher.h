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
#ifndef _FLASHER_H
#define _FLASHER_H

#include <string>
#include <exception>

#include "Flash.h"
#include "Samba.h"
#include "FileError.h"

class FileSizeError : public FileError
{
public:
    FileSizeError() : FileError() {};
    virtual const char* what() const throw() { return "file operation exceeds flash size"; }
};

class Flasher
{
public:
    Flasher(Flash::Ptr& flash) : _flash(flash) {}
    virtual ~Flasher() {}

    void erase();
    void write(const char* filename);
    bool verify(const char* filename);
    void read(const char* filename, long fsize);
    void lock(std::string& regionArg, bool enable);
    void info(Samba& samba);

private:
    void progressBar(int num, int div);

    Flash::Ptr& _flash;
};

#endif // _FLASHER_H
