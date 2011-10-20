#ifndef _ARM_DIS_H
#define _ARM_DIS_H

#include <bfd.h>

int
arm_dis_buf(unsigned char *b, int len, bfd_vma pc, int is_thumb, int little_code);

#endif // _ARM_DIS_H

