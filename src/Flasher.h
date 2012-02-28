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
