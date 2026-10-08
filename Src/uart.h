//uart.h

#include "types.h"
void Init_UART(void);
void U0_Tx(u8 sByte);
u8 U0_Rx(void);
void U0_TxStr(s8 *str);
void U0_TxU32(u32 num);
void U0_TxS32(s32 num);
void U0_TxF32(f32 fNum,u8 nDp);
void U0_TxDate(void);
void U0_TxTime(void);
void voting_default_data(void);
void U0_Voting_Stop(void);
void U0_Voting_Start(void);
void U0_Invalid_Card(void);
void U0_Authentication(void);
