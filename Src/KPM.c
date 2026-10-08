#include "kpm_defines.h"
#include "delay.h"
#include <lpc21xx.h>

#include "types.h"

#include "lcd.h"
//u32 KpmLut[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
u8 KpmLut[4][4]={{'7','8','9','/'},{'4','5','6','*'},{'1','2','3','-'},{'c','0','=','+'}};

//inti the pins

void Init_KPM(void)
{
   IODIR1|=15<<ROW0;
}

//colscan function
u32 colscan(void)
{
   if(((IOPIN1>>COL0)&15)<15)
   {
     return 0;
   }
   return 1;
}

//row_check function

u32 Row_Check(void)
{
   int rno;
   for(rno=0;rno<4;rno++)
   {
    	IOPIN1=(IOPIN1&~(0xFF<<ROW0))|((~(1<<rno))<<ROW0);
		 if(colscan()==0)
            {
                  break;
            }
        }
        IOCLR1=15<<ROW0;
        return rno;
}

 //col_check function
 u32 Col_Check(void)

 {
   int cno;
   for(cno=0;cno<4;cno++)
   {
      if(((IOPIN1>>(COL0+cno))&1)==0)
          {
            break;
          }
   }
   return cno;
 }


 //keyscan function
u32 Key_Scan(void)
{
   u32 key,cno,rno;
   while(colscan());
    rno=Row_Check();
        cno=Col_Check();
        key=KpmLut[rno][cno];
	  while(!colscan());
	      delay_ms(100);
        return key;
}

u32 Read_Num(void)
{
  u8 key;
  s8 count=0;
  u32 sum=0;
  while(1)
  {
		key=Key_Scan();
		if(key=='c')
		{
			return 'c';
		}
  		 if(key>='0' && key<='9')
	    {
			 count++;
      		 sum=(sum*10)+(key-48);
			// cmdLcd(0xc0);
     		 CharLCD(key);
   }
	 if((key=='+') && (count>0))
	 {
		 count--;
		 Cmd_LCD(0x10);
		 CharLCD(' ');
		 sum=sum/10;
		 Cmd_LCD(0x10);
	 }
   if(key=='=')
    {
          return sum;
    }
   }
}
