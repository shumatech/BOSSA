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
#ifndef _FILEERROR_H
#define _FILEERROR_H

#include <exception>
#include <errno.h>
#include <string.h>

#include "Flash.h"
#include "Samba.h"

class FileError : public std::exception
{
public:
    FileError() : std::exception() {}
};

class FileOpenError : public FileError
{
public:
    FileOpenError(int errnum) : FileError(), _errnum(errnum) {};
    const char* what() const throw() { return strerror(_errnum); }
private:
    int _errnum;
};

class FileIoError : public FileError
{
public:
    FileIoError(int errnum) : FileError(), _errnum(errnum) {};
    const char* what() const throw() { return strerror(_errnum); }
private:
    int _errnum;
};

class FileShortError : public FileError
{
public:
    FileShortError() : FileError() {};
    const char* what() const throw() { return "short write"; }
};

#endif // _FILEERROR_H
