#define OFFICER_RFID_ADDR       0x0000
#define OFFICER_PASS_ADDR       0x0010

#define VOTING_FLAG_ADDR        0x0020
#define TOTAL_VOTERS_ADDR       0x0021

#define START_TIME_ADDR         0x0030
#define END_TIME_ADDR           0x0040

#define PARTY1_COUNT_ADDR       0x0050
#define PARTY2_COUNT_ADDR       0x0054
#define PARTY3_COUNT_ADDR       0x0058
#define PARTY4_COUNT_ADDR       0x005C
#define PARTY5_COUNT_ADDR       0x0060
#define PARTY6_COUNT_ADDR       0x0064
#define PARTY7_COUNT_ADDR       0x0068
#define PARTY8_COUNT_ADDR       0x006C

#define VOTER_RECORD_SIZE       0x14

/*#define VOTER1_BASE_ADDR        0x0100
#define VOTER2_BASE_ADDR        0x0114
#define VOTER3_BASE_ADDR        0x0128
#define VOTER4_BASE_ADDR        0x013C
#define VOTER5_BASE_ADDR        0x0150*/

#define VOTER_NOT_VOTED         0
#define VOTER_ALREADY_VOTED     1

#define VOTER_REMOVED           0
#define VOTER_ACTIVE            1
/*#ifndef DATA_LOCATION_DEFINES_H
#define DATA_LOCATION_DEFINES_H

// Officer information 
#define OFFICER_RFID_ADDR       0x0000
#define OFFICER_PASS_ADDR       0x0010

// Voting information 
#define VOTING_FLAG_ADDR        0x0020
#define TOTAL_VOTERS_ADDR       0x0021

// RTC information 
#define START_TIME_ADDR         0x0030
#define END_TIME_ADDR           0x0040

// Party vote counts - each u32 = 4 bytes 
#define PARTY1_COUNT_ADDR       0x0050
#define PARTY2_COUNT_ADDR       0x0054
#define PARTY3_COUNT_ADDR       0x0058
#define PARTY4_COUNT_ADDR       0x005C
#define PARTY5_COUNT_ADDR       0x0060
#define PARTY6_COUNT_ADDR       0x0064
#define PARTY7_COUNT_ADDR       0x0068
#define PARTY8_COUNT_ADDR       0x006C

// Voter records 
#define VOTER_RECORD_SIZE  0x14    // 20 bytes */

#define VOTER1_BASE_ADDR   0x0100
#define VOTER1_PASS_ADDR   0x010A
#define VOTER1_FLAG_ADDR   0x010E
#define VOTER1_STATUS_ADDR 0x010F

#define VOTER2_BASE_ADDR   0x0114
#define VOTER2_PASS_ADDR   0x011E
#define VOTER2_FLAG_ADDR   0x0122
#define VOTER2_STATUS_ADDR 0x0123

#define VOTER3_BASE_ADDR   0x0128
#define VOTER3_PASS_ADDR   0x0132
#define VOTER3_FLAG_ADDR   0x0136
#define VOTER3_STATUS_ADDR 0x0137

#define VOTER4_BASE_ADDR   0x013C
#define VOTER4_PASS_ADDR   0x0146
#define VOTER4_FLAG_ADDR   0x014A
#define VOTER4_STATUS_ADDR 0x014B

#define VOTER5_BASE_ADDR   0x0150
#define VOTER5_PASS_ADDR   0x015A
#define VOTER5_FLAG_ADDR   0x015E
#define VOTER5_STATUS_ADDR 0x015F

/*#define VOTER_NOT_VOTED    0
#define VOTER_ALREADY_VOTED 1

#define VOTER_REMOVED      0
#define VOTER_ACTIVE       1

#endif	*/

