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
#include "WordCopyApplet.h"

WordCopyApplet::WordCopyApplet(Samba& samba, uint32_t addr)
    : Applet(samba,
             addr,
             applet.code,
             sizeof(applet.code),
             addr + applet.start,
             addr + applet.stack,
             addr + applet.reset)
{
}

WordCopyApplet::~WordCopyApplet()
{
}

void
WordCopyApplet::setDstAddr(uint32_t dstAddr)
{
    _samba.writeWord(_addr + applet.dst_addr, dstAddr);
}

void
WordCopyApplet::setSrcAddr(uint32_t srcAddr)
{
    _samba.writeWord(_addr + applet.src_addr, srcAddr);
}

void
WordCopyApplet::setWords(uint32_t words)
{
    _samba.writeWord(_addr + applet.words, words);
}
