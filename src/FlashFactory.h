#ifndef _FLASHFACTORY_H
#define _FLASHFACTORY_H

#include <stdint.h>

#include "Samba.h"
#include "Flash.h"

class FlashFactory
{
public:
    FlashFactory();
    virtual ~FlashFactory();
    
    Flash::Ptr create(Samba& samba, uint32_t chipId);
};

#endif // _FLASHFACTORY_H
