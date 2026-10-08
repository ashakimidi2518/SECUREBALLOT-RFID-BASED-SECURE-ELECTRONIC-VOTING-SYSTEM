#include "types.h"


void Init_I2C(void);

void I2C_Start(void);

void I2C_Restart(void);

void I2C_Write(u8 dat);

void I2C_Stop(void);

u8 I2C_NACK(void);

u8 I2C_MACK(void);
