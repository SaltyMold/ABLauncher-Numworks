#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "libs/eadk.h"

#define SLOT_B_TARGET 0x90410030UL   // slot B (0x90400000) + offset 0x10030 = 0x90410030
#define SLOT_A_TARGET 0x90010030UL   // slot A (0x90000000) + offset 0x10030 = 0x90010030

#define AIRCR (*(volatile uint32_t*)0xE000ED0C)
#define AIRCR_VECTKEY        0x05FA0000UL
#define AIRCR_SYSRESETREQ    (1UL << 2)

__attribute__((naked)) void jump(unsigned long addr);

void system_reset(void);

#define SLOT_A_START 0x90000000UL
#define SLOT_B_START 0x90400000UL
#define SLOT_SIZE    0x00400000UL

typedef enum { SLOT_A, SLOT_B, SLOT_UNKNOWN } slot_t;

slot_t current_slot(void);

void sleep(int ms, bool *quit);

#endif