# SecureBallot: RFID-Based Secure Electronic Voting System

A secure, tamper-evident electronic voting prototype built on the **NXP LPC2148 (ARM7TDMI-S)** in **Embedded C** (Keil µVision). A voter is authenticated with an **RFID card + 4-digit PIN**, votes once, and the vote is stored in a **non-volatile I2C EEPROM**. An **election officer** card unlocks an admin menu. Every event is time-stamped by the on-chip **RTC** and streamed as an **audit log** to a PC over UART, **without ever recording whom a voter chose**.

---

## Table of contents
1. [Features](#1-features)
2. [Block diagram](#2-block-diagram)
3. [Kit / hardware diagram](#3-kit--hardware-diagram)
4. [Pin mapping](#4-pin-mapping)
5. [How the system works](#5-how-the-system-works)
6. [EEPROM memory map](#6-eeprom-memory-map)
7. [Audit log format](#7-audit-log-format)
8. [Build and flash](#8-build-and-flash)
9. [How to use (demo walkthrough)](#9-how-to-use-demo-walkthrough)
10. [Known limitations and future work](#10-known-limitations-and-future-work)
11. [Skills demonstrated](#11-skills-demonstrated)

---

## 1. Features
- **Two-factor voter authentication:** RFID card (something you have) + keypad PIN (something you know), shown as `*` on the LCD.
- **One voter, one vote:** a per-voter "voted" flag in EEPROM blocks duplicate voting.
- **Ballot secrecy by design:** EEPROM stores only a voted flag per voter and a total per party; the log records `VOTE_CAST` but never the party.
- **Time-controlled voting:** the RTC opens and closes the voting window; voter cards are rejected outside it.
- **Officer console:** edit RTC/voting time, view result (winner or tie), add/remove voter cards, reset election, change officer password.
- **Interrupt-driven RFID reception:** UART1 interrupt parses the 10-byte reader frame (`STX` + 8 ASCII digits + `ETX`).
- **Serial audit log to PC:** `DATE, TIME, EVENT, CARD_ID, STATUS, DESCRIPTION` over UART0.
- **Party symbols** drawn with LCD **CGRAM** custom characters.
- **Modular drivers:** LCD, keypad, UART0/UART1, I2C, EEPROM, RTC, each tested alone before integration.

## 2. Block diagram

![SecureBallot block diagram: LPC2148 with keypad, RFID reader, LCD, LEDs/buzzer, EEPROM, RTC and PC](block_diagram.png)

*Block diagram of SecureBallot: inputs (4x4 keypad, RFID reader on UART1) on the left, outputs (LCD, green LED, red LED/buzzer) and the AT24C256 EEPROM (I2C) on the right, UART0 to the PC through MAX232, and the on-chip RTC.*

**Data flow in one line:** card tap, then UART1 ISR, then main loop checks EEPROM + RTC, then keypad PIN, then party choice, then EEPROM update, then UART0 audit log.

## 3. Kit / hardware diagram

![Hardware setup on the Vector India Advanced Development Board for ARM7 with the RFID reader and audit log in HyperTerminal](kit_setup.png)

*Working kit: Vector India Advanced Development Board for ARM7 (LPC2148) with the RFID reader wired by jumper wires, the 20x4 LCD, 4x4 keypad and buzzer on the board, and the audit log (`SYSTEM_START ... System Initialized`) running in HyperTerminal.*

**Hardware list:** LPC2148 board, RFID reader + cards (8-digit ID, 10-byte frame), 20x4 LCD, 4x4 keypad, buzzer/LEDs, AT24C256, MAX232, USB-to-UART converter, jumper wires.
**Software list:** Keil µVision (Embedded C), Flash Magic (ISP programming), HyperTerminal or any serial terminal.

## 4. Pin mapping

| Function | LPC2148 pin(s) | Notes |
|---|---|---|
| LCD data D0-D7 | P1.24 - P1.31 | 8-bit mode |
| LCD RS / EN | P0.16 / P0.17 | RW tied to GND |
| Keypad rows (outputs) | P1.16 - P1.19 | driven low one at a time |
| Keypad columns (inputs) | P1.20 - P1.23 | pull-ups, read for key press |
| UART0 TxD0 / RxD0 | P0.0 / P0.1 | log to PC via MAX232; also ISP port |
| UART1 TxD1 / RxD1 | P0.8 / P0.9 | RFID reader DOUT to RxD1 |
| I2C0 SCL / SDA | P0.2 / P0.3 | AT24C256, 4.7 k pull-ups |
| Buzzer | P0.14 | note: P0.14 is also the ISP-entry pin |

Clock: 12 MHz crystal, PLL x5 gives **CCLK 60 MHz**, **PCLK 15 MHz**. UART 9600 baud, I2C 100 kHz.

## 5. How the system works

### 5.1 Power-on sequence (`main.c`)
1. Initialise keypad, LCD, I2C, UART0, UART1 (with interrupt), buzzer, CGRAM party symbols.
2. Seed EEPROM data (officer, voters, counters) and set the RTC.
3. Print the log header and `SYSTEM_START` over UART0.
4. Show the project name on the LCD for 2 s.
5. Enter the main loop: show RTC date/time and **"Waiting For Card"**.

### 5.2 RFID reception (UART1 interrupt)
1. Reader sends 10 bytes at 9600 baud: `0x02` + 8 ASCII digits + `0x03` (example: card `12345678` gives `02 31 32 33 34 35 36 37 38 03`).
2. The ISR waits for `STX`, stores 8 bytes, accepts only if `ETX` follows, adds a NUL and sets `Rfid_ready`.
3. The main loop sees the flag and starts processing. The ISR only collects data; all slow work is in `main()`.

### 5.3 Voter flow
1. Scan card: log `CARD_SCAN`.
2. If the card is the officer card, go to the officer menu (6.4).
3. If voting is not active (outside the RTC window): "Voting Not Started".
4. Search the 8 voter records in EEPROM. No match: "Invalid Voter", buzzer, log `AUTHENTICATION FAILED`.
5. Voter removed: "Voter Removed". Already voted: "Already Voted", log `DUPLICATE_VOTE, BLOCKED`.
6. Enter 4-digit PIN (masked, 2 attempts). Log `PASSWORD_CHECK` (the PIN is never logged).
7. Voter menu: **1 Vote, 2 Change PIN, 3 Exit**.
8. Party menu (8 parties with CGRAM symbols), then confirm Yes/No.
9. On confirm: read the party counter from EEPROM, add 1, write it back, set the voter's voted flag. Display "Vote Successful" and log `VOTE_CAST` (no party).

### 5.4 Officer flow
1. Officer card scanned, then PIN checked, then `OFFICER_LOGIN` logged.
2. Menu page 1: **1 Edit Time, 2 View Result, 3 Edit Voter Cards, 4 Next**. Page 2: **5 Reset Election, 6 Edit Password, 7 Back, 8 Exit**.
3. **Edit Time** sets RTC date/time and the voting start and stop time (hour/minute) with validation.
4. **View Result** shows each party's count, the winner or a tie, logs `VIEW_RESULT`, then closes voting (`STOP_VOTING`).
5. **Edit Voter Cards** adds or removes (soft-deletes) voters, up to 8.
6. **Reset Election** clears all voted flags and zeroes the party counters.

### 5.5 Voting window
Voting opens when the RTC hour and minute equal the configured start time and closes at the stop time. Outside the window voter cards are rejected; the officer card still works.

### 5.6 Keypad keys
| Key | Meaning |
|---|---|
| `0`-`9` | Digits / menu choice |
| `=` | Enter / confirm |
| `c` | Cancel |
| `+` | Backspace |

## 6. EEPROM memory map

| Address | Size | Content |
|---|---|---|
| `0x0000` | 9 B | Officer RFID (8 digits + NUL) |
| `0x0010` | 4 B | Officer password |
| `0x0020` | 1 B | Voting flag (reserved) |
| `0x0021` | 1 B | Total voters |
| `0x0030` / `0x0040` | reserved | Start / end time (reserved) |
| `0x0050` - `0x006F` | 8 x 4 B | Party 1-8 vote counters (u32, little-endian) |
| `0x0100 + 20*n` | 20 B | Voter record n (max 8) |

Voter record: `+0x00` card ID (9 B), `+0x0A` PIN (4 B), `+0x0E` voted flag (0/1), `+0x0F` status (1 = active, 0 = removed).

## 7. Audit log format

Header:
```
SecureBallot - RFID Based Secure Electronic Voting System Election Audit Log
Election ID     : EC2026-001
Polling Booth   : Booth-08
Polling Officer : 12611820
Date            : DD-MM-YYYY
DATE, TIME, EVENT, CARD_ID, STATUS, DESCRIPTION
```
Sample lines:
```
03-10-2026, 02:35:59, SYSTEM_START, -, SUCCESS, System Initialized
14-07-2026, 09:01:12, CARD_SCAN, 12345678, RECEIVED, RFID detected
14-07-2026, 09:01:36, VOTE_CAST, 12345678, SUCCESS, Vote recorded
14-07-2026, 09:35:18, DUPLICATE_VOTE, 12345678, BLOCKED, already voted
```
Events: `SYSTEM_START, CARD_SCAN, AUTHENTICATION, PASSWORD_CHECK, VOTE_CAST, DUPLICATE_VOTE, OFFICER_LOGIN, VIEW_RESULT, RESET_ELECTION, START_VOTING, STOP_VOTING`.

## 8. Build and flash
1. Clone: `git clone https://github.com/ashakimidi2518/<your-repo-name>.git`
2. Open `project_files.uvproj` in **Keil µVision**, target device LPC2148.
3. Build (**F7**). Expect `0 Error(s), 0 Warning(s)` and `major_project.hex`.
4. Wire the board as in section 3/4 and connect UART0 (DB9 or USB-serial) to the PC.
5. Open **Flash Magic**: device LPC2148, select the COM port, baud 9600 or higher, oscillator 12 MHz, choose `major_project.hex`, tick "Erase blocks used by Hex", then **Start**.
6. Open HyperTerminal (or any serial terminal) at **9600, 8-N-1**, no flow control.
7. Reset the board. The LCD shows the project name, then "Waiting For Card", and the terminal prints `SYSTEM_START`.

## 9. How to use (demo walkthrough)
1. Tap a **registered voter card**, enter the PIN, choose **1 Vote**, pick a party, confirm.
2. Tap the same card again: **Already Voted**.
3. Tap an **unregistered card**: **Invalid Voter** with the buzzer.
4. Tap the **officer card**, enter the officer PIN, and use the menu to set time, view results or reset.
5. Watch each action appear as a time-stamped line in the terminal.

> **Demo data:** officer card, voter cards and default PINs are seeded in `DATA.c`. They are for demonstration only; change them before any real use and do not publish real card numbers.

## 10. Known limitations and future work
- EEPROM data is re-seeded on each power-up; add an "initialised" marker so votes persist across resets.
- Voting start/stop times are held in RAM; store them in EEPROM at the reserved addresses.
- RTC is set at boot; use a 32.768 kHz crystal and VBAT cell and initialise only when stopped.
- Vote update is not atomic; write the voted flag first or use a commit record and CRC.
- Use a 32-byte voter record stride to avoid 64-byte EEPROM page wrap-around.
- Add I2C status checks and time-outs, a watchdog, PIN lockout and hashed PINs.
- Planned: external-interrupt officer mode, LED indicators, PC logger with per-session CSV, stronger cards (MIFARE) or fingerprint.

## 11. Skills demonstrated
Embedded C, LPC2148 register-level programming, GPIO, UART with VIC interrupts, I2C master driver, EEPROM data layout, RTC, HD44780 LCD and CGRAM, matrix keypad scanning, state-based application design, serial audit logging, hardware/software integration and debugging.

---
*Platform: NXP LPC2148 | IDE: Keil µVision | Programming: Flash Magic | Training: Vector India*
