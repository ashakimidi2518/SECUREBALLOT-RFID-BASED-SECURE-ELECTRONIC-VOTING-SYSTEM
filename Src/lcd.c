#include<lpc21xx.h>
#include"types.h"
#include"lcd_defines.h"
#include"defines.h"
#include"delay.h"

void WriteLCD(u8 byte)
{
// write to data pins
//WRITEBYTE(IOPIN1,LCD_DATA,byte);
IOPIN1=((IOPIN1&~((u32)255<<LCD_DATA)) | ((u32)byte<<LCD_DATA));
//IOCLR0=1<<LCD_RW;
//select write operation
IOSET0=1<<LCD_EN;
delay_us(1);
IOCLR0=1<<LCD_EN;
delay_ms(2);
}

void Cmd_LCD(u8 opcode)
{
	//clr rs pin for cmd reg select
	IOCLR0=1<<LCD_RS;
	//write to cmd register via d0 to d7
	WriteLCD(opcode);
}

void InitLCD(void)
{
	// cfg p0.8-0.15,rs,rw,en as gpio out pins
	IODIR1 |=((u32)0xff<<LCD_DATA);
	IODIR0 |=(1<<LCD_RS)|(1<<LCD_EN);

	delay_ms(15);
	Cmd_LCD(0x30);
	delay_ms(4);
	delay_us(100);
	Cmd_LCD(0x30);
	delay_us(100);
	Cmd_LCD(0x30);
	Cmd_LCD(MODE_8BIT_2LINE);
	Cmd_LCD(DSP_ON_CUR_OFF);
	Cmd_LCD(CLEAR_LCD);
	Cmd_LCD(SHIFT_CUL_RIGHT);
}

void CharLCD(u8 asciiVal)
{
	//set rs pin for data register   select
	IOSET0=1<<LCD_RS;
	//WRITE TO DDRAM VIA DATA REG VIA DATA PINS
	WriteLCD(asciiVal);
}

void StrLCD(s8 *str)
{
	while(*str)
          CharLCD(*str++);
}

void U32LCD(u32 num)
{
u8 a[10];
s32 i=0;
if(num==0)
	CharLCD('0');
else 
{	
	while(num>0)
	{
		a[i++]=(num%10)+48;
		num/=10;
	}
	for(--i;i>=0;i--)
		CharLCD(a[i]);
}
}
void S32LCD(s32 num)
{
	if(num<0)
	{
		CharLCD('-');
		num=-num;
	}
	U32LCD(num);
}

void F32LCD(f32 fNum,u32 nDP)
{
	u32 num;
	s32 i;
	if(fNum<0.0)
	{
		CharLCD('-');
		fNum=-fNum;
	}
	num=fNum;
	U32LCD(num);
	CharLCD('.');
	for(i=0;i<nDP;i++)
	{
		fNum=(fNum-num)*10;
		num=fNum;
		CharLCD(num+48);
	}
}

void BuildCGRAM(s8 *p,u32 nBytes)
{
	u32 i;
	//goto cgram start
	Cmd_LCD(GOTO_CGRAM_START);
	//SELECT DATA REG & WRITE OPERATION
	IOSET0=1<<LCD_RS;
//	IOCLR0=1<<LCD_RW;
	//WRITE TO CGRAM VIA DATA REG VIA PINS
	for(i=0;i<nBytes;i++)
	{
		WriteLCD(p[i]);
	}
	//goto ddram again
	Cmd_LCD(GOTO_LINE1_POS0);
}
