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
