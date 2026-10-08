//uart.c

#include "types.h"
#include "uart_defines.h"
#include <lpc21xx.h>

extern u8 Rfid_buf[CARD_LEN + 1];

void Init_UART(void)
{
	//set p0.0 and p0.1 as Tx and Rx pin
	PINSEL0&=~(15<<0);
	PINSEL0|=TxD0_PIN|RxD0_PIN;
	//enable the dlab bit and set the values of wordlen
	U0LCR=(1<<DLAB_BIT)|WORDLEN;
	//put divisor values in U0DLL and DLM
	U0DLL=DIVISOR;
	U0DLM=DIVISOR>>8;
	//clear dlab bit
	U0LCR&=~(1<< DLAB_BIT);
}
void U0_Tx(u8 sByte)
{
	U0THR=sByte;
	while(((U0LSR>>TEMT_BIT)&1)==0);
}
u8 U0_Rx(void)
{
	while(((U0LSR>>DR_BIT)&1)==0);
	return U0RBR;
}
void U0_TxStr(s8 *str)
{
	while(*str)
	{
		U0_Tx(*str++);
	}
}
/*void U0_TxU32(u32 num)
{
	s8 i=0;
	u8 a[10];
	while(num)
	{
		a[i++]=(num%10)+48;
		num/=10;
	}
	for(--i;i>=0;i--)
	{
		U0_Tx(a[i]);
	}
}*/
void U0_TxU32(u32 num)
{
    s8 i = 0;
    u8 a[10];

    if(num == 0)
    {
        U0_Tx('0');
        return;
    }

    while(num)
    {
        a[i++] = (num % 10) + '0';
        num /= 10;
    }

    for(--i; i >= 0; i--)
    {
        U0_Tx(a[i]);
    }
}
void U0_TxS32(s32 num)
{
	if(num<0)
	{
		U0_Tx('-');
		num=-num;
	}
	U0_TxU32(num);
}

void U0_TxF32(f32 fNum, u8 nDp)
{
    u32 num;
    u8 i;

    if(fNum < 0.0)
    {
        U0_Tx('-');
        fNum = -fNum;
    }

    num = fNum;

    U0_TxU32(num);
    U0_Tx('.');

    for(i = 0; i < nDp; i++)
    {
        fNum = (fNum - num) * 10;
        num = fNum;

        U0_Tx(num + '0');
    }
}
void U0_TxDate(void)
{
                U0_TxU32(DOM/10);
				U0_TxU32(DOM%10);
                U0_Tx('-');
                U0_TxU32(MONTH/10);
				U0_TxU32(MONTH%10);
                U0_Tx('-');
                U0_TxU32(YEAR);
}
void U0_TxTime(void)
{
                U0_TxU32(HOUR/10);
				U0_TxU32(HOUR%10);
                U0_Tx(':');
                U0_TxU32(MIN/10);
				U0_TxU32(MIN%10);
                U0_Tx(':');
                U0_TxU32(SEC/10);
				U0_TxU32(SEC%10);
}

void voting_default_data(void)
{
		U0_TxStr("=============================================================================\n\r");
                U0_TxStr("SecureBallot : RFID Based Secure Electronic Voting System Election Audit Log\n\r");
                U0_TxStr("=============================================================================\n\r");
                U0_TxStr("Election ID           : EC2026-001\n\r");
                U0_TxStr("Polling Booth         : Booth-08\n\r");
                U0_TxStr("Polling Officer       : 12611820\n\r");
                U0_TxStr("DATE                  : ");
                U0_TxDate();
                U0_TxStr("\n\r");
                U0_TxStr("\n\r");
                U0_TxStr("------------------------------------------------------------------------------\n\r");
                U0_TxStr("\n\r");
       
                U0_TxDate();
                U0_Tx(',');
                U0_TxTime();
                U0_Tx(',');
                U0_TxStr("SYSTEM_START,-,SUCCESS,System Initialized\n\r");
}

void U0_Voting_Stop(void)
{
				U0_TxDate();
        		U0_Tx(',');
        		U0_TxTime();
        		U0_Tx(',');
        		U0_TxStr(" STOP_VOTING");
        		U0_Tx(',');
        		U0_TxStr((s8*)Rfid_buf);
        		U0_Tx(',');
        		U0_TxStr(" SUCCESS");
        		U0_Tx(',');
       		 	U0_TxStr(" Voting Stopped");
        		U0_TxStr("\n\r");
}
void U0_Voting_Start(void)
{
				U0_TxDate();
        		U0_Tx(',');
        		U0_TxTime();
        		U0_Tx(',');
        		U0_TxStr(" START_VOTING");
        		U0_Tx(',');
        		U0_TxStr((s8*)Rfid_buf);
        		U0_Tx(',');
        		U0_TxStr(" SUCCESS");
        		U0_Tx(',');
       		 	U0_TxStr(" Voting Enabled");
        		U0_TxStr("\n\r\n\r");
}
void U0_Invalid_Card(void)
{
	U0_TxStr(" FAILED");
    U0_Tx(',');
    U0_TxStr(" Invalid Card");
    U0_TxStr("\n\r");
}
void U0_Authentication(void)
{
	U0_TxDate();
    U0_Tx(',');
    U0_TxTime();
    U0_Tx(',');
    U0_TxStr(" AUTHENTICATION");
    U0_Tx(',');
    U0_TxStr((s8*)Rfid_buf);
    U0_Tx(',');
}
