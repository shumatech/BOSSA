#ifndef _WORDCOPYAPPLET_H
#define _WORDCOPYAPPLET_H

#include "Applet.h"
#include "WordCopyArm.h"

class WordCopyApplet : public Applet
{
public:
    WordCopyApplet(Samba& samba, uint32_t addr);
    virtual ~WordCopyApplet();

    void setDstAddr(uint32_t dstAddr);
    void setSrcAddr(uint32_t srcAddr);
    void setWords(uint32_t words);

private:
    static WordCopyArm applet;
};

#endif // _WORDCOPYAPPLET_H
