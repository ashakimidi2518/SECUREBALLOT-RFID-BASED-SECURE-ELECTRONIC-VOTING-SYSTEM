// this function used to check the password is correct or not
// if same return 1 if not then return 0.
#include "types.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "delay.h"

extern u32 save_pass;   // globally stored password to compare against

u8 password(void)
{
    u32 temp = 0;     // holds the digits entered by the user (built up digit by digit)
    u32 count = 4;     // number of digits still needed (password is 4 digits long)
    u32 temp1 = 0;     // holds the key that was just pressed

    Cmd_LCD(CLEAR_LCD);          // clear the LCD screen
    StrLCD("Enter the pin:");   // prompt user to enter pin

    // ---- Loop to collect up to 4 digits of the PIN ----
    while(count)
    {
        temp1 = Key_Scan();   // read a key press from the keypad

        // only process the key if it's a digit, cancel, or backspace
        if((temp1 >= '0' && temp1 <= '9') || temp1 == 'c' || temp1 == '+')
        {
            if(temp1 == 'c')
            {
                return 'c';   // 'c' pressed -> cancel, exit function immediately
            }

            // '+' is used as backspace, only allowed if at least one digit
            // has already been entered (count < 4) and not all digits used up (count > 0)
            if(temp1 == '+' && (count < 4 && count > 0))
            {
                again:                     // label used by goto below (from confirm loop)
                Cmd_LCD(0x10);              // move LCD cursor one position back
                CharLCD(' ');              // overwrite the last '*' with a blank space
                Cmd_LCD(0x10);              // move cursor back again to sit on the blank
                temp /= 10;                // remove the last entered digit from temp
                count++;                   // one less digit entered, so increment count back
            }
            else if(temp1 >= '0' && temp1 <= '9')
            {
                count--;                   // one more digit entered, decrement remaining count
                CharLCD('*');              // show '*' on LCD instead of the actual digit (mask input)
                temp = temp * 10 + temp1 - '0';   // shift existing digits left and add new digit
            }
        }
    }

    // ---- After 4 digits entered, wait for user to press '=' to confirm ----
    while((temp1 = Key_Scan()) != '=')
    {
        if(temp1 == '+')
        {
            goto again;   // jump back up to backspace handling code
                       
        }
        if(temp1 == 'c')
        {
            return 'c';   // 'c' pressed while waiting for '=' -> cancel/go back
        }
    }

    // ---- Compare entered PIN with the saved password ----
    if(temp == save_pass)
    {
        Cmd_LCD(GOTO_LINE2_POS0);        // move cursor to line 2
        StrLCD("valid password");       // show success message
        delay_ms(1000);                 // pause so user can read the message
        return 1;                       // password correct
    }

    return 0;   // password incorrect
}

// It provides to reset the password

u32 edit_password(void)
{
    u32 key;

    Cmd_LCD(CLEAR_LCD);
    StrLCD("Enter New Password");

    Cmd_LCD(GOTO_LINE2_POS0);
    StrLCD("4 digit num: ");

    key = Read_Num();

    if(key == 'c')
    {
        Cmd_LCD(CLEAR_LCD);
        return 'c';
    }

    if(key >= 1000 && key <= 9999)
    {
        save_pass = key;

        Cmd_LCD(CLEAR_LCD);
        Cmd_LCD(GOTO_LINE2_POS0 + 6);
        StrLCD("Password");

        Cmd_LCD(GOTO_LINE3_POS0);
        StrLCD("Updated successfully");

        delay_ms(1000);
        Cmd_LCD(CLEAR_LCD);

        return key;
    }

    Cmd_LCD(CLEAR_LCD);
    StrLCD("Invalid Password");

    delay_ms(1000);
    Cmd_LCD(CLEAR_LCD);

    return 0;
}

u8 check_password(void)
{
	u8 pass_valid=0;	//to check pass is valid or not
	s8 attempts=2;	//attempts
	do
	{
		pass_valid=password();
		if(pass_valid=='c')
		{
			return 0;
		}
		if(pass_valid==1)
		{
			return 1;
		}
		if(pass_valid==0)
		{
			Cmd_LCD(CLEAR_LCD);
			attempts--;                // Decrement remaining attempts
			StrLCD("you have ");
			U32LCD(attempts);           // Display remaining attempt count
			StrLCD(" attempts");
			Cmd_LCD(GOTO_LINE2_POS0);
			StrLCD("wait 3 seconds");
			delay_ms(3000);
		}
		if(attempts == 0)            // No attempts left
		{
				return 0;                 // Deny access, return to caller
		}
	}while(attempts);
	return 0;
}

