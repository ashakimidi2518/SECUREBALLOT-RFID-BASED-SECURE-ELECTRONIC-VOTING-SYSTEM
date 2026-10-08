//we provide some default value for rtc time and date before editing its values
#include <LPC21xx.h>
#include "rtc_defines.h"
#include "lcd.h"
void Default_Rtc_time(void)
{
	CCR=1<<1; //reset 
	PREINT=PREINT_VALUE;
	PREFRAC=PREFRAC_VALUE;
	CCR=1<<0;//ctc is enable
	SEC=59;
	MIN=35;
	HOUR=2;
	DOM=3;
	MONTH=10;
	YEAR=2026;
}

//this function is used for displaying current time 
void Rtc_Time_Display(void)
{
	//HH:MM:SS
		CharLCD(HOUR/10+'0');
		CharLCD(HOUR%10+'0');
		CharLCD(':');
		CharLCD(MIN/10+'0');
		CharLCD(MIN%10+'0');
		CharLCD(':');
		CharLCD(SEC/10+'0');
		CharLCD(SEC%10+'0');
		StrLCD("            ");
}

//this function is used for displaying date 
void Rtc_Date_Display(void)
{
   	//day/month/year
		CharLCD(DOM/10+'0');
		CharLCD(DOM%10+'0');
		CharLCD('/');
		CharLCD(MONTH/10+'0');
		CharLCD(MONTH%10+'0');
		CharLCD('/');
		U32LCD(YEAR);
		StrLCD("          ");
}

