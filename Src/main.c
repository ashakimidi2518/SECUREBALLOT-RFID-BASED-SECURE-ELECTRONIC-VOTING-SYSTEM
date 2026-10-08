#include<lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "i2c.h"
#include "i2c_eeprom.h"
#include "uart.h"
#include "uart_defines.h"
#include "uart1_intrrupt.h"
#include "data.h"
#include "interfaces.h"
#include "rtc_defaults.h"
#include "check.h"
#include "data_location_defines.h"
#include "i2c_eeprom_defines.h"

/* -------------------------------------------------
   Global variables
   ------------------------------------------------- */

u8 valid_officer = 0;
u8 valid_voter = 0;
u32 candidate_memory_cnt = 140;
u8 voting_start = 0;
u8 p[10];


/* RTC voting time variables are defined in officer.c */
extern u8 voting_Start_Hour;
extern u8 voting_Start_Minutes;
extern u8 voting_Stop_Hour;
extern u8 voting_Stop_Minutes;


/* RFID variables are defined in UART1.c */
extern vu32 Rfid_ready;
extern vu32 Rx_index;
extern vu32 Frame_active;
extern u8 Rfid_buf[CARD_LEN + 1];


/* -------------------------------------------------
   MAIN
   ------------------------------------------------- */

int main(void)
{
    u8 i;
    u8 voter_found;

    // Hardware initialization
     
    Init_KPM();
    InitLCD();
    Init_I2C();
    Init_UART();
    Init_UART1();
    Init_buzzer();
    Init_Cgram();

    Officer_data();
    Voter_cards();
    setting_votes();

    /*
     * RTC initialization
     * This sets the default RTC value.
	*/
    Default_Rtc_time();

    /*
     * Send system-start audit information
     */
     voting_default_data();
	Cmd_LCD(GOTO_LINE1_POS0+3);
     StrLCD("SECUREBALLOT :");
     Cmd_LCD(GOTO_LINE2_POS0+1);
     StrLCD("RFID-BASED SECURE");
     Cmd_LCD(GOTO_LINE3_POS0+5);
     StrLCD("ELECTRONIC");
     Cmd_LCD(GOTO_LINE4_POS0+3);
     StrLCD("VOTING SYSTEM");
     delay_ms(2000);
     Cmd_LCD(CLEAR_LCD);

     while(1)
     {
		Cmd_LCD(0x80);
          Rtc_Time_Display();
          Cmd_LCD(0xc0);
          Rtc_Date_Display();
          Cmd_LCD(0x94);
          StrLCD("Waiting For Card....");
        /* =========================================
           RFID CARD PROCESSING
           ========================================= */

          if(Rfid_ready)
          {
            /*
             * Prevent another RFID frame from
             * being accepted while processing
             * current card.
             */
      		Rfid_ready = 0;
			U0_TxDate();
        U0_Tx(',');
        U0_TxTime();
        U0_Tx(',');
        U0_TxStr(" CARD_SCAN ");
        U0_Tx(',');
        U0_TxStr((s8*)Rfid_buf);
        U0_Tx(',');
        U0_TxStr(" RECEIVED");
        U0_Tx(',');
        U0_TxStr(" RFID Detected");
        U0_TxStr("\n\r");

            /* -------------------------------------
               Check Officer RFID
               ------------------------------------- */

            	if(check_bytes(OFFICER_RFID_ADDR,Rfid_buf,9))
               {
               	valid_officer = 1;
                	officer();
            	}

            /* -------------------------------------
               Check Voter RFID
               ------------------------------------- */

               else if(voting_start)
   	          {
      	          voter_found = 0;
				  U0_Authentication();

                /*
                 * Search all voter records.
                 * candidate_memory_cnt = 140
                 * Record size = 20 bytes
                 * Therefore:
                 *
                 * 0
                 * 20
                 * 40
                 * ...
                 * 140
                 */
                for(i = 0;i <= candidate_memory_cnt;i += VOTER_RECORD_SIZE)
                {
                    /*
                     * First check RFID
                     */
                    if(check_bytes(VOTER1_BASE_ADDR + i,Rfid_buf,9))
                    {
                        voter_found =1;
                        /*
                         * RFID matched.
                         *
                         * Now check whether voter
                         * account is active.
                         */
                        if(i2c_eeprom_read(I2C_EEPROM_SA1,VOTER1_STATUS_ADDR + i)== VOTER_ACTIVE)
                        {
							U0_TxStr(" SUCCESS");
        					U0_Tx(',');
        					U0_TxStr(" Voter Authenticated");
        					U0_TxStr("\n\r");
                            /*
                             * Active voter.
                             *
                             * Authentication will
                             * be handled inside
                             * Valid_voter().
                             */
                            Valid_voter(VOTER1_BASE_ADDR + i);
                        }
                        else if(voter_found==1)
                        {
							U0_TxStr(" FAILED");
        					U0_Tx(',');
        					U0_TxStr(" Voter Removed");
        					U0_TxStr("\n\r");
                            /*
                             * RFID belongs to a
                             * removed voter.
                             */
                            Cmd_LCD(CLEAR_LCD);
                            StrLCD("Voter Removed");
                            delay_ms(1500);
                        }

                        /*
                         * RFID found, so stop search.
                         */
                        break;
                    }
                }


                /*
                 * RFID was not present in any
                 * voter record.
                 */
                if(voter_found == 0)
                {
                    Cmd_LCD(CLEAR_LCD);
                    StrLCD("Invalid Voter");

        			U0_Invalid_Card();
                    delay_ms(1500);
                }
            }


            /* -------------------------------------
               Voting has not started
               ------------------------------------- */

            else
            {
                Cmd_LCD(CLEAR_LCD);

                StrLCD("Voting Not Started");

                delay_ms(1500);
            }


            /*
             * Clear RFID buffer after processing
             */
            for(i = 0; i < CARD_LEN + 1; i++)
            {
                Rfid_buf[i] = 0;
            }

            /*
             * Reset RFID reception variables
             */
            Rx_index = 0;
            Frame_active = 0;
			U0_TxStr("\n\r");
        }

        /* =========================================
           VOTING START TIME
           ========================================= */

        if(voting_start == 0)
        {
			Cmd_LCD(GOTO_LINE4_POS0);
			StrLCD("Voting Not Started");
            /*
             * Check whether current RTC time
             * matches voting start time.
             */
            if((HOUR == voting_Start_Hour) &&
               (MIN == voting_Start_Minutes))
            {
                voting_start = 1;

                Cmd_LCD(CLEAR_LCD);

                StrLCD("Voting Started");
				
				U0_Voting_Start();

                delay_ms(1000);
            }
        }


        /* =========================================
           VOTING STOP TIME
           ========================================= */

        if(voting_start == 1)
        {
		            Cmd_LCD(GOTO_LINE4_POS0);
				  StrLCD("Voting Started");
            /*
             * Check whether current RTC time
             * matches voting stop time.
             */
            if((HOUR == voting_Stop_Hour) &&
               (MIN == voting_Stop_Minutes))
            {
                voting_start = 0;

                Cmd_LCD(CLEAR_LCD);

                StrLCD("Voting Stopped");
				U0_Voting_Stop();
                delay_ms(1000);
            }
        }
    }
}
