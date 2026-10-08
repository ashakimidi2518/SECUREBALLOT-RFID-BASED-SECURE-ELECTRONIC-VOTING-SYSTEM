#include "types.h"
#include "password.h"
#include "lcd.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"
#include <lpc21xx.h>
#include "check.h"
#include "data_location_defines.h"
#include "i2c_eeprom_defines.h"
#include "i2c_eeprom.h"
#include "my_str_func.h"
#include "uart_defines.h"
#include "uart.h"

u32 save_pass;
extern u8 valid_officer;
extern u16 candidate_memory_cnt;
extern s8 Rfid_buf[CARD_LEN+1];
extern u8 voting_start;
u8 key;
u8 voting_Start_Hour;
u8 voting_Start_Minutes;
u8 voting_Stop_Hour;
u8 voting_Stop_Minutes;
u8 officer_verify=0;

u8 minute(void)
{
        s32 min;
        // ---- Set MINUTE ----
                                Cmd_LCD(CLEAR_LCD);
                                StrLCD("Enter Minutes :");
                                Cmd_LCD(GOTO_LINE2_POS0);
                                min = Read_Num();

                                if(min == 'c')
                                {
                                        return 0;
                                }

                                if(min < 0 || min >= 60)  // Validate 0-59 minute range
                                {
                                        Cmd_LCD(CLEAR_LCD);
                                        StrLCD("Invalid Minutes");
                                        delay_ms(1000);
                                        return 0;
                                }
                                else
                                {
                                        return min;            //return minutes
                                }
}
u8 hour(void)
{
        s32 hour;
        // ---- Set HOUR ----
                                Cmd_LCD(CLEAR_LCD);
                                StrLCD("Enter Hour :");
                                Cmd_LCD(GOTO_LINE2_POS0);
                                hour = Read_Num();          // Read numeric input from keypad

                                if(hour == 'c')             // 'c' = user pressed Cancel/Clear
                                {
                                        return 0;                   // Abort this entry, return to menu
                                }

                                if(hour < 0 || hour >= 24)  // Validate 24-hour format range
                                {
                                        Cmd_LCD(CLEAR_LCD);
                                        StrLCD("Invalid Hours");
                                        delay_ms(1000);
                                        return 0;          // Show error message briefly
                                }
                                else
                                {
                                        return hour;
                                }
}
void Time(void)
{
        u8 key;
        s32 temp;

        while(1)
        {
                // Display the Time-setting menu options
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.HOUR");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.MINUTE");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.EXIT");

                // Wait for a keypad press and act on the selected menu option
                key = Key_Scan();
                switch(key)
                {
                        case '1':       if((temp=hour())>0)
                                                                {
                                                                        HOUR=temp;
                                                                        SEC=0;
                                                                }
                                                                break;

                        case '2':       if((temp=minute())>0)
                                                                {
                                                                        MIN=temp;
                                                                        SEC=0;
                                                                }
                                                                break;

                        case '3':
                                // ---- EXIT ----
                                return;                      // Leave the Time-setting menu
                }
        }
}

/* ========================================================================
 * Function: DATE
 * Purpose : Menu-driven function to set the RTC DATE, MONTH, and YEAR.
 *           Loops until the user selects EXIT.
 * ====================================================================== */
void DATE(void)
{
        u8 key;
        s32 temp;

        while(1)
        {
                // Display the Date-setting menu options
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.DATE");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.MONTH");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.YEAR");
                Cmd_LCD(GOTO_LINE4_POS0);
                StrLCD("4.EXIT");

                key = Key_Scan();
                switch(key)
                {
                        case '1':
                                // ---- Set DAY OF MONTH ----
                                Cmd_LCD(CLEAR_LCD);
                                StrLCD("Enter DATE");
                                Cmd_LCD(GOTO_LINE2_POS0);
                                temp = Read_Num();

                                if(temp == 'c')
                                {
                                        break;
                                }

                                if(temp < 1 || temp > 31)   // Validate day range (1-31)
                                {
                                        Cmd_LCD(CLEAR_LCD);
                                        StrLCD("Invalid DATE");
                                        delay_ms(1000);
                                }
                                else
                                {
                                        DOM = temp;              // Commit valid day-of-month
                                }
                                break;

                        case '2':
                                // ---- Set MONTH ----
                                Cmd_LCD(CLEAR_LCD);
                                StrLCD("Enter MONTH");
                                Cmd_LCD(GOTO_LINE2_POS0);
                                temp = Read_Num();

                                if(temp == 'c')
                                {
                                        break;
                                }

                                if(temp < 1 || temp > 12)   // Validate month range (1-12)
                                {
                                        Cmd_LCD(CLEAR_LCD);
                                        StrLCD("Invalid MONTH");
                                        delay_ms(1000);
                                }
                                else
                                {
                                        MONTH = temp;            // Commit valid month
                                }
                                break;

                        case '3':
                                // ---- Set YEAR ----
                                Cmd_LCD(CLEAR_LCD);
                                StrLCD("Enter YEAR");
                                Cmd_LCD(GOTO_LINE2_POS0);
                                temp = Read_Num();

                                if(temp == 'c')
                                {
                                        break;
                                }

                                if(temp < 1 || temp > 4055) // Validate year range (RTC hardware limit)
                                {
                                        Cmd_LCD(CLEAR_LCD);
                                        StrLCD("Invalid YEAR");
                                        delay_ms(1000);
                                }
                                else
                                {
                                        YEAR = temp;             // Commit valid year
                                }
                                break;

                        case '4':
                                // ---- EXIT (handled below after switch) ----
                                break;
                }

                if(key == '4')
                        break;                            // Leave the Date-setting menu
        }
}
/* ========================================================================
 * Function: SET RTC TIME
 * Purpose : Mnue for showing and setting real time clock
 *
 * ====================================================================== */
void set_Rtc_Time(void)
{
        while(1)
        {
                // Display Time/Date edit menu
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.TIME");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.DATE");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.EXIT");
                key = Key_Scan();
                switch(key)
                {
                        case '1':Time();                      // Go to Time-setting sub-menu
                                                        break;
                        case '2':DATE();                      // Go to Date-setting sub-menu
                                                        break;
                        case '3': return;                      // Exit back to caller (EINT0_RISE)
                }
        }
}
void voting_Start_Time(void)
{
        u8 temp;
        while(1)
        {
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.HOUR");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.MINUTE");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.EXIT");
                key = Key_Scan();
                switch(key)
                {
                        case '1':       if((temp=hour())>0)                             // Go to Time-setting sub-menu
                                                                {
                                                                        voting_Start_Hour=temp;
                                                                }
                                                                break;
                        case '2':       if((temp=minute())>0)                           // Go to Date-setting sub-menu
                                                                {
                                                                        voting_Start_Minutes=temp;
                                                                }
                                                                break;
                        case '3': return;                      // Exit
                }
        }
}
void voting_Stop_Time(void)
{
        u8 temp;
        while(1)
        {
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.HOUR");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.MINUTE");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.EXIT");
                key = Key_Scan();
                switch(key)
                {
                        case '1':if((temp=hour())>0)  // Go to Time-setting sub-menu
						         {                                    
                                       voting_Stop_Hour=temp;
                                 }
                                  break;
                        case '2':if((temp=minute())>0)   // Go to Date-setting sub-menu
                                 {
                                        voting_Stop_Minutes=temp;
                                 }
                                 break;
                        case '3': return;                      // Exit
                }
        }
}
void set_Voting_Time(void)
{
        while(1)
        {
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.VOTING START TIME");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.VOTING STOP TIME");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.EXIT");
                key=Key_Scan();
                switch(key)
                {
                        case '1':voting_Start_Time();
                                 break;
                        case '2': voting_Stop_Time();
                                  break;
                        case '3': return;
                }
        }
}
void edit_Time(void)
{
        while(1)
        {
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.SET RTC TIME");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.SET VOTING TIME");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.EXIT");
                key=Key_Scan();
                switch(key)
                {
                        case '1':set_Rtc_Time();
                                 break;
                        case '2':set_Voting_Time();
                                 break;
                        case '3': return;
                }
        }
}
void remove_Voter_Card(void)
{
    u8 total_voter;
    u8 i;
    u8 status_flag;
    u8 stored_id[10];
    u8 entered_id_str[10];
    u32 entered_id;
    u16 user_addr;

    total_voter =i2c_eeprom_read(I2C_EEPROM_SA1,TOTAL_VOTERS_ADDR);
    if(total_voter == 0)
    {
        Cmd_LCD(CLEAR_LCD);
	    Cmd_LCD(GOTO_LINE2_POS0+5);
        StrLCD("No Voters");
        delay_ms(1000);
        return;
    }
    /* Enter Voter ID */
    Cmd_LCD(CLEAR_LCD);
    StrLCD("Enter Voter ID:");
    Cmd_LCD(GOTO_LINE2_POS0);
    entered_id = Read_Num();
    if(entered_id == 'c')
        return;
    /* Voter ID must be 8 digits */
    if(entered_id < 10000000 ||
       entered_id > 99999999)
    {
        Cmd_LCD(CLEAR_LCD);
	    Cmd_LCD(GOTO_LINE2_POS0);
        StrLCD("Invalid Voter ID");
        delay_ms(1000);
        return;
    }
    /* Convert entered ID to string */
    my_itoa((s32)entered_id,(s8 *)entered_id_str,10);
    entered_id_str[8] = '\0';
    /* Search all voter records */
    for(i = 0; i < total_voter; i++)
    {
        user_addr = VOTER1_BASE_ADDR +(i * VOTER_RECORD_SIZE);
        /* Read stored voter ID */
        i2c_eeprom_seq_read(I2C_EEPROM_SA1,user_addr,stored_id,9);
        stored_id[9] = '\0';
        /* Compare entered ID with stored ID */
        if(my_strcmp((cs8 *)entered_id_str,(cs8 *)stored_id) == 0)
        {
            /* Check whether voter is already removed */
            status_flag =i2c_eeprom_read(I2C_EEPROM_SA1,user_addr + 0x0F);
            if(status_flag == VOTER_REMOVED)
            {
                Cmd_LCD(CLEAR_LCD);
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("Voter Already Remove");
                delay_ms(1500);
                return;
            }
            /* Mark voter as removed */
            status_flag = VOTER_REMOVED;
            i2c_eeprom_write(I2C_EEPROM_SA1,user_addr + 0x0F,status_flag);
            Cmd_LCD(CLEAR_LCD);
            StrLCD("Voter Removed");
            delay_ms(1500);
            return;
        }
    }
    /* ID not found */
    Cmd_LCD(CLEAR_LCD);
	Cmd_LCD(GOTO_LINE2_POS0);
    StrLCD("Voter ID Not Found");
    delay_ms(1500);
}
void add_Voter_Card(void)
{
    u8 total_voter;
    u8 i;
    u8 newstrID[10];
    u8 password[5];
    u8 voter_flag;
    u8 status_flag;
    u8 stored_id[10];
    u32 NewVoterID;
    u32 new_password;
    u16 user_addr;
    /* Read current voter count */
    total_voter =i2c_eeprom_read(I2C_EEPROM_SA1,TOTAL_VOTERS_ADDR);
    /* -------------------------------
       Enter Voter ID
       ------------------------------- */
    Cmd_LCD(CLEAR_LCD);
    StrLCD("Enter Voter ID:");
    Cmd_LCD(GOTO_LINE2_POS0);
    NewVoterID = Read_Num();
    if(NewVoterID == 'c')
        return;
    /* Check 8 digit ID */
    if(NewVoterID < 10000000 ||
       NewVoterID > 99999999)
    {
        Cmd_LCD(CLEAR_LCD);
		Cmd_LCD(GOTO_LINE2_POS0);
        StrLCD("Invalid Voter ID");
        delay_ms(1000);
        return;
    }
    /* Convert entered ID to string */
    my_itoa((s32)NewVoterID,(s8 *)newstrID,10);
    newstrID[8] = '\0';

    /* --------------------------------
       Search existing voter IDs
       -------------------------------- */

    for(i = 0; i < total_voter; i++)
    {
        user_addr = VOTER1_BASE_ADDR +(i * VOTER_RECORD_SIZE);

        i2c_eeprom_seq_read(I2C_EEPROM_SA1,user_addr,stored_id,9);
        stored_id[9] = '\0';

        if(my_strcmp((cs8 *)newstrID,(cs8 *)stored_id) == 0)
        {
            /* Same ID found */

            status_flag =i2c_eeprom_read(I2C_EEPROM_SA1,user_addr + 0x0F);
            /* -------------------------------
               Already active
               ------------------------------- */

            if(status_flag == VOTER_ACTIVE)
            {
                Cmd_LCD(CLEAR_LCD);
				Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("Voter Already Exists");
                delay_ms(1500);
                return;
            }
            /* -------------------------------
               Previously removed
               Reactivate same voter
               ------------------------------- */

            Cmd_LCD(CLEAR_LCD);
            StrLCD("Enter Password:");
            Cmd_LCD(GOTO_LINE2_POS0);

            new_password = Read_Num();

            if(new_password == 'c')
                return;


            if(new_password < 1000 ||
               new_password > 9999)
            {
                Cmd_LCD(CLEAR_LCD);
				Cmd_LCD(GOTO_LINE2_POS0 + 2);
                StrLCD("Invalid Password");
                delay_ms(1000);
                return;
            }

            my_itoa((s32)new_password,(s8 *)password,10);

            /* Save new password */

            i2c_eeprom_pageWrite(I2C_EEPROM_SA1,user_addr + 0x0A,password,4);

            /* Reset voted flag */

            voter_flag = VOTER_NOT_VOTED;

            i2c_eeprom_write(I2C_EEPROM_SA1,user_addr + 0x0E,voter_flag);

            /* Make voter active */

            status_flag = VOTER_ACTIVE;

            i2c_eeprom_write(I2C_EEPROM_SA1,user_addr + 0x0F,status_flag);

            Cmd_LCD(CLEAR_LCD);
			Cmd_LCD(GOTO_LINE2_POS0 + 1);
            StrLCD("Voter Added Again");

            delay_ms(1500);

            return;
        }
    }


    /* --------------------------------
       New voter
       -------------------------------- */

    if(total_voter >= 8)
    {
        Cmd_LCD(CLEAR_LCD);
		Cmd_LCD(GOTO_LINE2_POS0 + 5);
        StrLCD("Voter Limit Full");

        delay_ms(1000);

        return;
    }


    /* New voter will use next record */

    total_voter++;

    user_addr =
        VOTER1_BASE_ADDR +
        ((total_voter - 1) * VOTER_RECORD_SIZE);


    /* Save voter ID */

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,user_addr,newstrID,9);

    /* -------------------------------
       Password
       ------------------------------- */

    Cmd_LCD(CLEAR_LCD);
    StrLCD("Enter Password:");
    Cmd_LCD(GOTO_LINE2_POS0);

    new_password = Read_Num();

    if(new_password == 'c')
        return;

    if(new_password < 1000 ||new_password > 9999)
    {
        Cmd_LCD(CLEAR_LCD);
		Cmd_LCD(GOTO_LINE2_POS0 + 2);
        StrLCD("Invalid Password");

        delay_ms(1000);
        return;
    }

    my_itoa((s32)new_password,(s8 *)password,10);

    i2c_eeprom_pageWrite(I2C_EEPROM_SA1,user_addr + 0x0A,password,4);

    /* Voter has not voted */

    voter_flag = VOTER_NOT_VOTED;

    i2c_eeprom_write(I2C_EEPROM_SA1,user_addr + 0x0E,voter_flag);

    /* Voter is active */

    status_flag = VOTER_ACTIVE;

    i2c_eeprom_write(I2C_EEPROM_SA1,user_addr + 0x0F,status_flag);

    /* Update total voter count */

    i2c_eeprom_write(I2C_EEPROM_SA1,TOTAL_VOTERS_ADDR,total_voter);

    Cmd_LCD(CLEAR_LCD);
	Cmd_LCD(GOTO_LINE2_POS0 + 4);
    StrLCD("Voter Added");

    delay_ms(1500);
}

void edit_Voter_Cards(void)
{
        while(1)
        {
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.ADD VOTER CARD");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.REMOVE VOTER CARD");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.EXIT");
                key=Key_Scan();
                switch(key)
                {
                        case '1': add_Voter_Card();
                                                                break;
                        case '2': remove_Voter_Card();
                                                                break;
                        case '3': return;
                }
        }
}

void reset_Election(void)
{
    u8 i;
    u8 total_voter;
    u32 zero = 0;

    total_voter =i2c_eeprom_read(I2C_EEPROM_SA1,TOTAL_VOTERS_ADDR);

    /* Reset voted flag of all voters */
    for(i = 0; i < total_voter; i++)
    {
        i2c_eeprom_write(I2C_EEPROM_SA1,VOTER1_FLAG_ADDR +(i * VOTER_RECORD_SIZE),VOTER_NOT_VOTED);
    }

    /* Reset party vote counts */
    for(i = 0; i < 8; i++)
    {
        i2c_eeprom_pageWrite(I2C_EEPROM_SA1,PARTY1_COUNT_ADDR + (i * 4),(u8 *)&zero,4);
    }

    Cmd_LCD(CLEAR_LCD);
	Cmd_LCD(GOTO_LINE2_POS0 + 3);
    StrLCD("Election Reset");
    U0_TxDate();
        U0_Tx(',');
        U0_TxTime();
        U0_Tx(',');
        U0_TxStr(" RESET_ELECTION");
        U0_Tx(',');
        U0_TxStr((s8 *)Rfid_buf);
        U0_Tx(',');
	   U0_TxStr(" SUCCESS");
        U0_Tx(',');
	   U0_TxStr(" Election_Reset");
	   U0_TxStr("\n\r");
    delay_ms(1000);
}

void view_Result(void)
{
    u8 i;
    u8 tie_count;
    u32 party_vote;
    u32 winner_vote_cnt = 0;
    u8 winner[8];

    /* -------------------------------
       UART Result Log
       ------------------------------- */

    U0_TxDate();
    U0_Tx(',');
    U0_TxTime();
    U0_Tx(',');
    U0_TxStr(" VIEW_RESULT");
    U0_Tx(',');
    U0_TxStr((s8 *)Rfid_buf);
    U0_Tx(',');

    /* -------------------------------
       First pass:
       Find highest vote count
       ------------------------------- */

    winner_vote_cnt = 0;

    for(i = 0; i < 8; i++)
    {
        party_vote = 0;

        i2c_eeprom_seq_read(I2C_EEPROM_SA1,PARTY1_COUNT_ADDR + (i * 4),(u8 *)&party_vote,4);

        if(party_vote > winner_vote_cnt)
        {
            winner_vote_cnt = party_vote;
        }
    }

    /* -------------------------------
       Find all parties having
       highest vote count
       ------------------------------- */

    tie_count = 0;

    for(i = 0; i < 8; i++)
    {
        party_vote = 0;

        i2c_eeprom_seq_read(I2C_EEPROM_SA1,PARTY1_COUNT_ADDR + (i * 4),(u8 *)&party_vote,4);

        if(party_vote == winner_vote_cnt)
        {
            winner[tie_count] = i;
            tie_count++;
        }
    }

    /* -------------------------------
       Display all party results
       ------------------------------- */

    for(i = 0; i < 8; i++)
    {
        party_vote = 0;

        i2c_eeprom_seq_read(I2C_EEPROM_SA1,PARTY1_COUNT_ADDR + (i * 4),(u8 *)&party_vote,4);

        Cmd_LCD(CLEAR_LCD);

        Cmd_LCD(GOTO_LINE2_POS0 + 5);
	    CharLCD(i);
        StrLCD(" PARTY ");
        U32LCD(i + 1);

        Cmd_LCD(GOTO_LINE3_POS0 + 5);

        StrLCD("Votes: ");
        U32LCD(party_vote);

        delay_ms(1500);
    }

    /* -------------------------------
       Display winner / tie
       ------------------------------- */

    if(tie_count == 1)
    {
        Cmd_LCD(CLEAR_LCD);

        Cmd_LCD(GOTO_LINE2_POS0 + 7);
        StrLCD("WINNER");
        Cmd_LCD(GOTO_LINE3_POS0 + 5);
		CharLCD(winner[0]);
        StrLCD(" PARTY ");
        U32LCD(winner[0] + 1);
        delay_ms(3000);
    }
    else
    {
        Cmd_LCD(CLEAR_LCD);
        Cmd_LCD(GOTO_LINE2_POS0 + 5);
        StrLCD("RESULT TIE");
        delay_ms(2000);

        /* Display tied parties */

        for(i = 0; i < tie_count; i++)
        {
            Cmd_LCD(CLEAR_LCD);
            Cmd_LCD(GOTO_LINE2_POS0 + 5);
            StrLCD("TIED PARTY ");
            U32LCD(winner[i] + 1);
            delay_ms(1500);
        }
    }
    U0_TxStr(" SUCCESS");
    U0_Tx(',');
    U0_TxStr(" Result Viewed");
    U0_TxStr("\n\r");

	StrLCD("Voting Started");
	U0_TxDate();
     U0_Tx(',');
     U0_TxTime();
     U0_Tx(',');
     U0_TxStr(" STOP_VOTING");
     U0_Tx(',');
     U0_TxStr(Rfid_buf);
     U0_Tx(',');
     U0_TxStr(" SUCCESS");
     U0_Tx(',');
     U0_TxStr(" Voting Stopped");
     U0_TxStr("\n\r");
    //After seeing the result stop the election
    voting_Start_Hour=0;
    voting_Start_Minutes=0;
    voting_Stop_Hour=0;
    voting_Stop_Minutes=0;
    voting_start=0;
}

void Officer_Menue(void)
{
	u8 new_pass[4];
	u32 pass;
        while(1)
        {
                Cmd_LCD(CLEAR_LCD);
                StrLCD("1.Edit Time");
                Cmd_LCD(GOTO_LINE2_POS0);
                StrLCD("2.View Result");
                Cmd_LCD(GOTO_LINE3_POS0);
                StrLCD("3.Edit Voter Cards");
                Cmd_LCD(GOTO_LINE4_POS0);
                StrLCD("4.Next");
                key=Key_Scan();
                switch(key)
                {
                        case '1' :edit_Time();
                                                        break;
                        case '2' :view_Result();
                                                        break;
                        case '3' :edit_Voter_Cards();
                                                        break;
                        case '4' :Cmd_LCD(CLEAR_LCD);
                                                        StrLCD("5.Reset Election");
                                                        Cmd_LCD(GOTO_LINE2_POS0);
                                                        StrLCD("6.Edit Password");
                                                        Cmd_LCD(GOTO_LINE3_POS0);
                                                        StrLCD("7.Back");
                                                        Cmd_LCD(GOTO_LINE4_POS0);
                                                        StrLCD("8.EXIT");
                                                        key=Key_Scan();
                                                        switch(key)
                                                        {
                                                                case '5' : reset_Election();
                                                                           break;
								case '6':

    									pass = edit_password();

    									if(pass >= 1000 && pass <= 9999)
    									{
        									my_itoa((s32)pass,(s8 *)new_pass,10);
        									i2c_eeprom_pageWrite(I2C_EEPROM_SA1,OFFICER_PASS_ADDR,new_pass,4);
    									}

    									   break;
                                                                case '7' : break;
                                                                case '8' :Cmd_LCD(CLEAR_LCD);
				    					return;
                                                        }
                                                        break;
                }
        }
}

void officer(void)
{
    u8 pass;
    u8 temp_pass[5];

    if(valid_officer)
    {
        valid_officer = 0;

        /* Read exactly 4 password bytes */
        i2c_eeprom_seq_read(I2C_EEPROM_SA1,OFFICER_PASS_ADDR,temp_pass,4);

        /* Add NULL only in RAM */
        temp_pass[4] = '\0';

        save_pass = my_atoi((cs8 *)temp_pass);

        Cmd_LCD(CLEAR_LCD);
        Cmd_LCD(GOTO_LINE2_POS0 + 2);
        StrLCD("WELCOME OFFICER");
	    U0_TxDate();
        U0_Tx(',');
        U0_TxTime();
        U0_Tx(',');
        U0_TxStr(" OFFICER_LOGIN");
        U0_Tx(',');
        U0_TxStr((s8 *)Rfid_buf);
        U0_Tx(',');

        delay_ms(1000);

        pass = check_password();

        if(pass == 1)
        {
            U0_TxStr(" SUCCESS");
            U0_Tx(',');
            U0_TxStr(" Officer authenticated");
            U0_TxStr("\n\r");

            Officer_Menue();

            officer_verify = 1;
        }
        else
        {
	    	U0_TxStr(" FAILED");
            U0_Tx(',');
            U0_TxStr(" Wrong_Password");
            U0_TxStr("\n\r");
            return;
        }
    }
    Cmd_LCD(CLEAR_LCD);
}
