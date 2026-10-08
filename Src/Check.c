//checking purpose 
#include "types.h"
//#include <string.h>
#include"my_str_func.h"
#include "i2c_eeprom_defines.h"
#include "i2c_eeprom.h"
#include "lcd.h"
#include "delay.h"
#include"lcd_defines.h"

u8 check_bytes(u16 buffAddr, u8 *data, u8 bytes)
{
    u8 buf[10];

    if(bytes > 10)
        return 0;

    i2c_eeprom_seq_read(
        I2C_EEPROM_SA1,
        buffAddr,
        buf,
        bytes);

    if(my_strcmp((cs8 *)data,
                 (cs8 *)buf) == 0)
    {
        return 1;
    }

    return 0;
}

u8 check_byte(u16 buffAddr)
{
    u8 buf;

    buf = i2c_eeprom_read(
              I2C_EEPROM_SA1,
              buffAddr);

    Cmd_LCD(CLEAR_LCD);
    StrLCD("vote cnt:");

    U32LCD(buf);

    delay_ms(1000);

    if(buf == 0)
        return 0;

    return buf;
}

