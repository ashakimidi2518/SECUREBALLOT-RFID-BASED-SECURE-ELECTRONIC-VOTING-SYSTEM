#include <lpc21xx.h>
#include "types.h"
#include "uart_defines.h"
#include "lcd.h"
#include "delay.h"

u8 Rfid_buf[CARD_LEN+1];
vu32 Rx_index=0;
vu32 Frame_active=0;
vu32 Rfid_ready=0;

void UART1_ISR(void)__irq
{
    u8 iir1;
    u8 data;

    iir1 = U1IIR;

    if(Rfid_ready == 0)
    {
        if(iir1 & 0x04)
        {
            data = U1RBR;

            /* Start of RFID frame */
            if(data == 0x02)
            {
                Rx_index = 0;
                Frame_active = 1;
                Rfid_ready = 0;
            }

            /* End of RFID frame */
            else if(data == 0x03)
            {
                if(Frame_active &&
                   Rx_index == CARD_LEN)
                {
                    Rfid_buf[CARD_LEN] = '\0';
                    Rfid_ready = 1;
                }

                Frame_active = 0;
            }

            /* RFID data */
            else if(Frame_active &&
                    Rx_index < CARD_LEN)
            {
                Rfid_buf[Rx_index] = data;
                Rx_index++;
            }
        }
    }

    VICVectAddr = 0;
}


void Init_UART1(void)
{
	//set p0.0 and p0.1 as Tx and Rx pin
	PINSEL0&=~(15<<8);
	PINSEL0|=0x00050000;
	//enable the dlab bit and set the values of wordlen
	U1LCR=(1<<DLAB_BIT)|WORDLEN;
	//put divisor values in U0DLL and DLM
	U1DLL=DIVISOR;
	U1DLM=DIVISOR>>8;
	//clear dlab bit
	U1LCR&=~(1<< DLAB_BIT);

	U1FCR=0x07;			//enable and reset FIFOs
	U1IER=0x01;		   //enable RDA intrrupt

	VICVectAddr0=(u32)UART1_ISR;
	VICIntEnable=(1<<U1_VIC_CHNO);
	VICVectCntl0=(1<<5)|U1_VIC_CHNO;
}

