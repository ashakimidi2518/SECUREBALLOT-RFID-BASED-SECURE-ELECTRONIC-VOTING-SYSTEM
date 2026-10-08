//uart defines 
#define TxD0_PIN 0x01
#define RxD0_PIN 0x04
#define FOSC 	 12000000
#define CCLK	 (FOSC*5)
#define PCLK	 (CCLK/4)
#define BAUD 	 9600
#define DIVISOR	 (PCLK/(16*BAUD))

// defines for UxLCR
#define DLAB_BIT 7
#define _8BIT 	 3
#define WORDLEN  _8BIT

//defines for UxLSR
#define TEMT_BIT 6
#define DR_BIT	 0

//defines for uart intrrupt

// define for UxIER SFR
#define RDA_INIT_EN_BIT  0
#define THRE_INIT_EN_BIT 1

// defines for UxIIR Sfr
#define THRE_INT 1
#define RDA_INT  2

//channels define 
#define U0_VIC_CHNO	6
#define U1_VIC_CHNO	7

//uart1 
#define U1_TX_INT_EN	1
#define U1_RX_INT_EN	0

#define CARD_LEN 8
