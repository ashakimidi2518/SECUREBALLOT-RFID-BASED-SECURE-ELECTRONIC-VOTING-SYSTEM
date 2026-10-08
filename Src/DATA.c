//DATA.c

#include "i2c_eeprom.h"
#include "data_location_defines.h"
#include "i2c_eeprom_defines.h"

void Officer_data(void)
{
    u8 total_voter = 3;

    /* Officer RFID */
    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,OFFICER_RFID_ADDR,(u8 *)"12611820",9);

    /* Officer password = exactly 4 bytes */
    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,OFFICER_PASS_ADDR,(u8 *)"3231",4);

    /* Total voters */
    i2c_eeprom_write(I2C_EEPROM_SA1,TOTAL_VOTERS_ADDR,total_voter);
}

void Voter_cards(void)
{
    u8 flag = VOTER_NOT_VOTED;
    u8 status = VOTER_ACTIVE;

    /* ---------------- VOTER 1 ---------------- */

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,VOTER1_BASE_ADDR,(u8 *)"12531874",9);

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,VOTER1_PASS_ADDR,(u8 *)"1234",4);

    i2c_eeprom_write(I2C_EEPROM_SA1,VOTER1_FLAG_ADDR,flag);

    i2c_eeprom_write(I2C_EEPROM_SA1,VOTER1_STATUS_ADDR,status);


    /* ---------------- VOTER 2 ---------------- */

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,VOTER2_BASE_ADDR,(u8 *)"12556450",9);

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,VOTER2_PASS_ADDR,(u8 *)"1234",4);

    i2c_eeprom_write(I2C_EEPROM_SA1,VOTER2_FLAG_ADDR,flag);

    i2c_eeprom_write(I2C_EEPROM_SA1,VOTER2_STATUS_ADDR,status);


    /* ---------------- VOTER 3 ---------------- */

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,VOTER3_BASE_ADDR,(u8 *)"12533660",9);

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,VOTER3_PASS_ADDR,(u8 *)"1234",4);

    i2c_eeprom_write(I2C_EEPROM_SA1,VOTER3_FLAG_ADDR,flag);

    i2c_eeprom_write(I2C_EEPROM_SA1,VOTER3_STATUS_ADDR,status);
}

void setting_votes(void)
{
    u8 i;
    u32 zero = 0;

    for(i = 0; i < 8; i++)
    {
        i2c_eeprom_pageWrite(I2C_EEPROM_SA1,PARTY1_COUNT_ADDR + (i * 4),(u8 *)&zero,4);
    }
}

