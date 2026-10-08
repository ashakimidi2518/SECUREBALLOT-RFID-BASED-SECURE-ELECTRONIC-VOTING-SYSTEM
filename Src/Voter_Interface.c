#include "types.h"
#include "password.h"
#include "lcd.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"
#include <lpc21xx.h>
#include "uart_defines.h"
#include "check.h"
#include "data_location_defines.h"
#include "i2c_eeprom_defines.h"
#include "i2c_eeprom.h"
#include "my_str_func.h"
#include "uart.h"

#define BUZZER 14

u16 user_addr;
extern s8 Rfid_buf[CARD_LEN+1];
extern u32 save_pass;
s8 cgramLUT[64]={
                	0x04,0x0A,0x15,0x0E,0x1F,0x04,0x0E,0x00, /* Lotus   */
			0x04,0x0E,0x15,0x15,0x1F,0x04,0x04,0x00, /* Hand    */
                  0x00,0x00,0x0A,0x1F,0x0E,0x0A,0x11,0x00, /* Bicycle */
                  0x0E,0x11,0x15,0x17,0x11,0x0E,0x00,0x00, /* Clock   */
                  0x04,0x15,0x0E,0x1F,0x0E,0x15,0x04,0x00, /* Star    */
                  0x04,0x0E,0x1F,0x0E,0x04,0x04,0x0E,0x00, /* Tree    */
                  0x04,0x0E,0x1F,0x04,0x04,0x0E,0x0E,0x00, /* Lamp    */
                  0x04,0x06,0x1F,0x06,0x04,0x00,0x00,0x00  /* Arrow   */
                 };
void Init_buzzer(void)
{
        IODIR0|=(IODIR0&~(1<<BUZZER))|(1<<BUZZER);
}

void Init_Cgram(void)
{
        BuildCGRAM(cgramLUT,64);
}

u8 conform_Vote(u8 vote)
{
    u32 vote_cnt;
    u8 vote_cast;
    u16 party_addr;

    /*
     * vote contains numeric party number:
     * 1 to 8
     */
    if(vote < 1 || vote > 8)
    {
        return 0;
    }

    party_addr =
        PARTY1_COUNT_ADDR +
        ((vote - 1) * 4);
	 vote_cnt=0;
    /* Read 4-byte vote count */
    i2c_eeprom_seq_read(I2C_EEPROM_SA1,party_addr,(u8 *)&vote_cnt,4);

    /* Increment vote */
    vote_cnt++;

    /* Store 4-byte vote count */
    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,party_addr,(u8 *)&vote_cnt,4);

    /* Mark voter as already voted */
    vote_cast = VOTER_ALREADY_VOTED;

    i2c_eeprom_write(I2C_EEPROM_SA1,user_addr + 0x0E,vote_cast);

    return 1;
}

u8 party_Menue(void)
{
        u8 vote,key,confirm_key;
        Cmd_LCD(CLEAR_LCD);
        Cmd_LCD(GOTO_LINE2_POS0+1);
        StrLCD("Put Your Valuable");
        Cmd_LCD(GOTO_LINE3_POS0+8);
        StrLCD("vote");
        delay_ms(1000);

		U0_TxDate();
        U0_Tx(',');
        U0_TxTime();
        U0_Tx(',');
        U0_TxStr(" VOTE_CAST ");
        U0_Tx(',');
        U0_TxStr(Rfid_buf);
        U0_Tx(',');

	//here we are showing the interface of parties with there symbols
        Cmd_LCD(CLEAR_LCD);
        CharLCD(0);
        StrLCD("1.PARTY1 ");
        CharLCD(1);
        StrLCD("2.PARTY2");			
        Cmd_LCD(GOTO_LINE2_POS0);
        CharLCD(2);
        StrLCD("3.PARTY3 ");
        CharLCD(3);
        StrLCD("4.PARTY4");
        Cmd_LCD(GOTO_LINE3_POS0);
        CharLCD(4);
        StrLCD("5.PARTY5 ");
        CharLCD(5);
        StrLCD("6.PARTY6");
        Cmd_LCD(GOTO_LINE4_POS0);
        CharLCD(6);
        StrLCD("7.PARTY7 ");
        CharLCD(7);
        StrLCD("8.PARTY8");
        key=Key_Scan();

		if(key>='1' && key<='8')
		{
			vote=key-'0';
		}
		else if(key=='c'||key==12)
		{
			return 0;
		}
		else
		{
			Cmd_LCD(CLEAR_LCD);
			StrLCD("Invalid Choise");
			delay_ms(1000);
			return 0;
		}

		Cmd_LCD(CLEAR_LCD);
		StrLCD("Conform vote?");
		Cmd_LCD(GOTO_LINE2_POS0);
		StrLCD("1.Yes");
		Cmd_LCD(GOTO_LINE3_POS0);
		StrLCD("2.No");

		confirm_key=Key_Scan();

		//yes
		if(confirm_key=='1')
		{
			if(conform_Vote(vote))
			{
				Cmd_LCD(CLEAR_LCD);
				StrLCD("Vote Successful");
	
     			 U0_TxStr(" SUCCESS");
        			 U0_Tx(',');
       		      U0_TxStr(" Vote Recorded");
        			 U0_TxStr("\n\r");
 
				delay_ms(1500);
				return 1;
			}
			else
			{
				Cmd_LCD(CLEAR_LCD);
				StrLCD("Vote Failed");
				delay_ms(1500);
				return 0;
			}
		}
		//N0
		else if(confirm_key == '2')
		{
			Cmd_LCD(CLEAR_LCD);
			StrLCD("Vote Cancelled");

			U0_TxStr(" FAILED");
	        	U0_Tx(',');
       		U0_TxStr(" Vote Cancelled");
      	  	U0_TxStr("\n\r");
			delay_ms(1000);
			return 0;
		}
		//Invalid conformation key
		else
		{
			Cmd_LCD(CLEAR_LCD);
			StrLCD("Invalid Choise");
			delay_ms(1000);
			return 0;
		}
}

u8 voter_Menue(void)
{
    u8 key;
    u8 voted_flag;
    u32 new_pass;
    u8 pass_str[5];

    while(1)
    {
        /* Check whether voter has already voted */
        voted_flag = i2c_eeprom_read(
                        I2C_EEPROM_SA1,
                        user_addr + 0x0E);

        if(voted_flag == VOTER_ALREADY_VOTED)
        {
            Cmd_LCD(CLEAR_LCD);

            StrLCD("Already Voted");

        U0_TxStr(" FAILED");
        U0_Tx(',');
        U0_TxStr(" Already_Voted");
        U0_TxStr("\n\r");
            delay_ms(2000);

            return 0;
        }

        Cmd_LCD(CLEAR_LCD);

        StrLCD("1.VOTE");

        Cmd_LCD(GOTO_LINE2_POS0);
        StrLCD("2.CHANGE PASSWORD");

        Cmd_LCD(GOTO_LINE3_POS0);
        StrLCD("3.EXIT");

        key = Key_Scan();

        switch(key)
        {
            /* ---------------- VOTE ---------------- */

            case '1':

                /* Check again before voting */
                voted_flag = i2c_eeprom_read(
                                I2C_EEPROM_SA1,
                                user_addr + 0x0E);

                if(voted_flag == VOTER_ALREADY_VOTED)
                {
                    Cmd_LCD(CLEAR_LCD);

                    StrLCD("Already Voted");

                    delay_ms(2000);

                    return 0;
                }

                /*
                 * Open party selection menu
                 */
                if(party_Menue()==1)
                {
                    return 1;
                }

                break;


            /* -------- CHANGE PASSWORD -------- */

            case '2':

                Cmd_LCD(CLEAR_LCD);

                new_pass = edit_password();

                /*
                 * Only save valid 4-digit password
                 */
                if(new_pass >= 1000 &&
                   new_pass <= 9999)
                {
                    my_itoa(
                        (s32)new_pass,
                        (s8 *)pass_str,
                        10);

                    /*
                     * Store only 4 bytes.
                     *
                     * Example:
                     * 1234 -> '1','2','3','4'
                     */
                    i2c_eeprom_pageWrite(
                        I2C_EEPROM_SA1,
                        user_addr + 0x0A,
                        pass_str,
                        4);

                    Cmd_LCD(CLEAR_LCD);

                    StrLCD("Password Saved");

                    delay_ms(1000);
                }

                break;


            /* ---------------- EXIT ---------------- */

            case '3':

                Cmd_LCD(CLEAR_LCD);

                return 0;
        }
    }
}


u8 Valid_voter(u16 address)
{
    u8 temp_pass[5];
    u8 pass;
    u8 valid;

    user_addr = address;

    /* Check whether voter has already voted */
    valid = i2c_eeprom_read(
                I2C_EEPROM_SA1,
                user_addr + 0x0E);

    if(valid == VOTER_ALREADY_VOTED)
    {
        Cmd_LCD(CLEAR_LCD);
        StrLCD("Already Voted");
	   U0_TxDate();
        U0_Tx(',');
        U0_TxTime();
        U0_Tx(',');
        U0_TxStr(" DUPLICATE_VOTE");
        U0_Tx(',');
        U0_TxStr(Rfid_buf);
        U0_Tx(',');
        U0_TxStr(" BLOCKED");
        U0_Tx(',');
        U0_TxStr(" Already Voted");
        U0_TxStr("\n\r");
        
        delay_ms(2000);

        return 0;
    }

    /* Read 4-byte password */
    i2c_eeprom_seq_read(
        I2C_EEPROM_SA1,
        user_addr + 0x0A,
        temp_pass,
        4);

    temp_pass[4] = '\0';

    save_pass = my_atoi((cs8 *)temp_pass);

    Cmd_LCD(CLEAR_LCD);
    StrLCD("WELCOME VOTER");
	U0_TxDate();
        U0_Tx(',');
        U0_TxTime();
        U0_Tx(',');
        U0_TxStr(" PASSWORD_CHECK ");
        U0_Tx(',');
        U0_TxStr(Rfid_buf);
        U0_Tx(',');

    delay_ms(1000);

    pass = check_password();

    if(pass == 1)
    {
		U0_TxStr(" SUCCESS");
        U0_Tx(',');
        U0_TxStr(" Valid_Password");
        U0_TxStr("\n\r");
        voter_Menue();
        return 1;
    }
    else
    {
        U0_TxStr(" FAILED");
        U0_Tx(',');
        U0_TxStr(" Wrong_Password");
        U0_TxStr("\n\r");
	}
    return 0;
}
void Invalid_voter(void)
{
	U0_TxDate();
        U0_Tx(',');
        U0_TxTime();
        U0_Tx(',');
        U0_TxStr(" AUTHENTICATION ");
        U0_Tx(',');
        U0_TxStr(Rfid_buf);
        U0_Tx(',');
        U0_TxStr(" FAILED");
        U0_Tx(',');
        U0_TxStr(" Invalid card");
        U0_TxStr("\n\r");
        Cmd_LCD(CLEAR_LCD);
        Cmd_LCD(GOTO_LINE2_POS0+4);
        StrLCD("CARD DETECTED");
        delay_s(1);
        Cmd_LCD(CLEAR_LCD);
        Cmd_LCD(GOTO_LINE2_POS0+3);
        StrLCD("CARD NOT FOUND");
        delay_s(1);
        Cmd_LCD(CLEAR_LCD);
        Cmd_LCD(GOTO_LINE2_POS0+3);
        StrLCD("ACCESS DENIED");
        IOSET0=1<<BUZZER;
        delay_s(3);
        IOCLR0=1<<BUZZER;
        Cmd_LCD(CLEAR_LCD);
}

