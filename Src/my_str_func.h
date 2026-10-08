#ifndef MY_STR_FUNC_H
#define MY_STR_FUNC_H
#include "types.h"

u32 my_strcmp(cs8*str1, cs8*str2);
s32 my_atoi(cs8*str);
s8*my_itoa(s32 num, s8*str, s32 base);

#endif

