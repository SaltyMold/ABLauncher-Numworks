# A numworks launcher that let you switch between userland in slot A and B

https://github.com/user-attachments/assets/ca0fedec-e78b-4b76-bc48-4b52275f2a6a

## Install the app

To install this app, you'll need to:
1. Download the `ABLauncher.nwa` file, which you can download from the [Releases](https://github.com/SaltyMold/ABLauncher-Numworks/releases).
2. Connect your NumWorks calculator to your computer using a USB cable.
3. Head to [my.numworks.com/apps](https://my.numworks.com/apps) to send the `nwa` file on your calculator.

## How to use the app

| Key   | Action        |
|-------|---------------|
| EXE  | Switch userland slot      |
| Back  | Quit      |

## Docs

The app works by changing the stack pointer and jumping to the correct address:
```
"ldrd r3, r2, [r0] \n"  // r3 = stack pointer, r2 = start address
"msr  MSP, r3      \n"  // set stack pointer
"bx   r2           \n"  // jump to start address
```
Slot B:
```
// 0x90400000 : start of slot B
// 0x10000    : userland header size
// 0x30       : slot info size
// 0x90410030 : target address
```
Slot A:
```
// 0x90000000 : start of slot A
// 0x20000    : userland headersize
// 0x30       : slot info size
// 0x90020030 : target address
```
It detects the slot your on:
```
unsigned long here = (unsigned long)&current_slot;

if (here >= SLOT_A_START && here < SLOT_A_START + SLOT_SIZE) return SLOT_A;
if (here >= SLOT_B_START && here < SLOT_B_START + SLOT_SIZE) return SLOT_B;
```

See more at: https://nwagyu.org/reference/firmware/slots.html

## Build the app

I made tutorials here :
- [C-App-Guide-for-Numworks](https://github.com/SaltyMold/C-App-Guide-for-Numworks)