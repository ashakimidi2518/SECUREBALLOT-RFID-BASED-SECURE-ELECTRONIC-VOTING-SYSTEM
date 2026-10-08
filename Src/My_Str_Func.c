#include "types.h"

u32 my_strcmp(cs8*str1, cs8*str2)
{
    while(*str1 && *str2)
    {
        if(*str1 != *str2)
        {
            return (u8)*str1 -(u8)*str2;
        }

        str1++;
        str2++;
    }

    return (u8)*str1 -(u8)*str2;
}

s32 my_atoi(cs8*str)
{
    s32 num = 0;
    s8 sign = 1;

    if(*str == '-')
    {
        sign = -1;
        str++;
    }

    while(*str >= '0' && *str <= '9')
    {
        num = (num * 10) + (*str - '0');
        str++;
    }

    return num * sign;
}

s8* my_itoa(s32 num, s8* str, s32 base)
{
    u8 i = 0;
    u8 sign = 0;
    u8 rem;
    u8 start, end;
    s8 temp;

    if(base < 2 || base > 16)
    {
        str[0] = '\0';
        return str;
    }

    if(num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    if(num < 0 && base == 10)
    {
        sign = 1;
        num = -num;
    }

    while(num > 0)
    {
        rem = num % base;

        if(rem < 10)
            str[i++] = rem + '0';
        else
            str[i++] = rem - 10 + 'A';

        num = num / base;
    }

    if(sign)
        str[i++] = '-';

    str[i] = '\0';

    /* Reverse the string */
    start = 0;
    end = i - 1;

    while(start < end)
    {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }

    return str;
}

