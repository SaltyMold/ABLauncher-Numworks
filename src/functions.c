#include "functions.h"
#include "libs/eadk.h"

__attribute__((naked)) void jump(unsigned long addr) {
    __asm volatile (
        "ldrd r3, r2, [r0] \n"  // r3 = stack pointer, r2 = start address
        "msr  MSP, r3      \n"  // set stack pointer
        "bx   r2           \n"  // jump to start address
    );
}

slot_t current_slot(void) {
    unsigned long here = (unsigned long)&current_slot;

    if (here >= SLOT_A_START && here < SLOT_A_START + SLOT_SIZE) return SLOT_A;
    if (here >= SLOT_B_START && here < SLOT_B_START + SLOT_SIZE) return SLOT_B;

    return SLOT_UNKNOWN;
}

void sleep(int ms, bool *quit) {
    for (int i = 0; i < ms / 10; i++) {
        eadk_keyboard_state_t state = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(state, eadk_key_back)) *quit = true;
        eadk_timing_msleep(10);
        if (*quit) break;
    }
}