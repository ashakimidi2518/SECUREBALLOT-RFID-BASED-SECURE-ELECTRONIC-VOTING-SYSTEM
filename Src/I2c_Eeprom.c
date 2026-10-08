#include "types.h"
#include "i2c.h"
#include "delay.h"

void i2c_eeprom_write(u8 slaveAddr,u16 wBuffAddr,u8 dat)
{
	I2C_Start();
	I2C_Write(slaveAddr<<1);
	I2C_Write(wBuffAddr>>8);
	I2C_Write(wBuffAddr);
	I2C_Write(dat);
	I2C_Stop();
	delay_ms(10);
}

u8 i2c_eeprom_read(u8 slaveAddr,u16 rBuffAddr)
{
	u8 dat;
	I2C_Start();
	I2C_Write(slaveAddr<<1);
	I2C_Write(rBuffAddr>>8);
	I2C_Write(rBuffAddr);
	I2C_Restart();
	I2C_Write((slaveAddr<<1)|1);
	dat=I2C_NACK();
	I2C_Stop();
	return dat;
}

void i2c_eeprom_pageWrite(u8 slaveAddr,u16 wBuffStartAddr,u8 *p,u8 nBytes)
{
	u8 i;
	I2C_Start();
	I2C_Write(slaveAddr<<1);
	I2C_Write(wBuffStartAddr>>8);
	I2C_Write(wBuffStartAddr);
	for(i=0;i<nBytes;i++)
	{
		I2C_Write(p[i]);
	}
	I2C_Stop();
	delay_ms(10);
}

void i2c_eeprom_seq_read(u8 slaveAddr,u16 rBuffStartAddr,u8 *p,u8 nBytes)
{ 
	u8 i;
	I2C_Start();
	I2C_Write(slaveAddr<<1);
	I2C_Write(rBuffStartAddr>>8);
	I2C_Write(rBuffStartAddr);
	I2C_Restart();
	I2C_Write(slaveAddr<<1|1);
	for(i=0;i<nBytes-1;i++)
	{
		p[i]=I2C_MACK();
	}
	p[i]=I2C_NACK();
	I2C_Stop();
}									 
