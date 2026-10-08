//i2c_eeprom.h


#include "types.h"


void i2c_eeprom_write(u8 slaveAddr,u16 wBuffAddr,u8 dat);

u8 i2c_eeprom_read(u8 slaveAddr,u16 rBuffAddr);

void i2c_eeprom_pageWrite(u8 slaveAddr,u16 wBuffStartAddr,u8 *p,u8 nBytes);

void i2c_eeprom_seq_read(u8 slaveAddr,u16 rBuffStartAddr,u8 *p,u8 nBytes);
