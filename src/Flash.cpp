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
    
    _wordCopy.setDstAddr(addr);
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
