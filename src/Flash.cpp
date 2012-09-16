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
#include "Flash.h"

#include <assert.h>

Flash::Flash(Samba& samba,
             const std::string& name,
             uint32_t addr,
             uint32_t pages,
             uint32_t size,
             uint32_t planes,
             uint32_t lockRegions,
             uint32_t user,
             uint32_t stack)
    : _samba(samba), _name(name), _addr(addr), _pages(pages), _size(size),
      _planes(planes), _lockRegions(lockRegions), _user(user), _wordCopy(samba, user)
{
    assert((size & (size - 1)) == 0);
    assert((pages & (pages - 1)) == 0);
    assert((lockRegions & (lockRegions - 1)) == 0);

    _wordCopy.setWords(size / sizeof(uint32_t));
    _wordCopy.setStack(stack);

    _onBufferA = true;
    _pageBufferA = _user + _wordCopy.size();
    _pageBufferB = _pageBufferA + size;
}

void
Flash::lockAll()
{
    for (uint32_t region = 0; region < _lockRegions; region++)
        setLockRegion(region, true);
}

void
Flash::unlockAll()
{
    for (uint32_t region = 0; region < _lockRegions; region++)
        setLockRegion(region, false);
}

void
Flash::loadBuffer(const uint8_t* data)
{
    _samba.write(_onBufferA ? _pageBufferA : _pageBufferB, data, _size);
}
