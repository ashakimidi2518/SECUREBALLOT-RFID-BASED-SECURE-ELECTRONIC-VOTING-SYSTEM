//rtc_defines.h

#define PCLK1 15000000

#define PREINT_VALUE ((PCLK1/32768)-1)

#define PREFRAC_VALUE (PCLK1-((PREINT_VALUE+1)*32768))

