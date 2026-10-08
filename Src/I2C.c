//i2c.c

#include "i2c_defines.h"

#include <lpc21xx.h>

#include "types.h"

void Init_I2C(void)

{

	//cfg sda and scl pins

	PINSEL0|=(SCL_PIN_FUNC)|(SDA_PIN_FUNC);

	//load the values of I2SCLL and I2SCHL

	I2SCLL=LOADVAL;

	I2SCLH=LOADVAL;

	//enable the the i2c

	I2CONSET=1<<I2EN_BIT;

}

void I2C_Start(void)

{

	//ISSUE STSRT BIT

	I2CONSET=1<<STA_BIT;

	//WAIT FOR STSRT BIT STATUS

	while(((I2CONSET>>SI_BIT)&1)==0);

	//CLEAR START BIT

	I2CONCLR=1<<STAC_BIT;

}

void I2C_Restart(void)

{

	//ISSUE START BIT

	I2CONSET=1<<STA_BIT;

	//CLEAR SI_BIT

	I2CONCLR=1<<SIC_BIT;

	//WAIT FOR START BIT STATUS

	while(((I2CONSET>>SI_BIT)&1)==0);

	//CLEAR START CONDITION

	I2CONCLR=1<<STAC_BIT;

}

void I2C_Write(u8 dat)

{

	//PUT DAT IN DATA REGISTERR

	I2DAT=dat;

	//CLEAR SI_BIT

	I2CONCLR=1<<SIC_BIT;

	//WAIT FOR START BIT STATUS

	while(((I2CONSET>>SI_BIT)&1)==0);

}

void I2C_Stop(void)

{

	//ISSUE STOP BIT

	I2CONSET=1<<STO_BIT;

	//CLEAR SI_BIT

	I2CONCLR=1<<SI_BIT;

	//WAIT FOR STOP BIT STATUS

	while((I2CONCLR>>STO_BIT)&1);

}

u8 I2C_NACK(void)

{

	I2CONCLR=1<<AAC_BIT;

	I2CONCLR=1<<SIC_BIT;

	while(((I2CONSET>>SI_BIT)&1)==0);

	return I2DAT;

}

u8 I2C_MACK(void)

{

	I2CONSET=1<<AA_BIT;
	I2CONCLR=1<<SIC_BIT;
	while(((I2CONSET>>SI_BIT)&1)==0);
	I2CONCLR=1<<AAC_BIT;
	return I2DAT;
}
