#include "libs/eadk.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "ABLauncher";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

#define SLOT_B_TARGET 0x90410030UL   // slot B (0x90400000) + offset 0x10030

__attribute__((naked)) void jump(unsigned long addr) {
    __asm volatile (
        "ldrd r3, r2, [r0] \n"
        "msr  MSP, r3       \n"
        "bx   r2            \n"
    );
}

#define AIRCR (*(volatile uint32_t*)0xE000ED0C)
#define AIRCR_VECTKEY        0x05FA0000UL
#define AIRCR_SYSRESETREQ    (1UL << 2)

void system_reset(void) {
    AIRCR = AIRCR_VECTKEY | AIRCR_SYSRESETREQ;
    while (1) {}
}

int main(void) {
    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_white);
    
    eadk_display_draw_string("Appuyer sur 1 pour lancer le slot A", (eadk_point_t){0, 0}, false, eadk_color_black, eadk_color_white);
    eadk_display_draw_string("Appuyer sur 2 pour lancer le slot B", (eadk_point_t){0, 20}, false, eadk_color_black, eadk_color_white);

    while (1){
        eadk_keyboard_state_t state = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(state, eadk_key_one)){
            eadk_display_draw_string("Lancement slot A...", (eadk_point_t){0, 0}, false, eadk_color_black, eadk_color_white);
            eadk_timing_msleep(500);
            system_reset();
        }
        else if (eadk_keyboard_key_down(state, eadk_key_two)){
            eadk_display_draw_string("Lancement slot B...", (eadk_point_t){0, 0}, false, eadk_color_black, eadk_color_white);
            eadk_timing_msleep(500);
            jump(SLOT_B_TARGET);
            while (1) {}
        }
    }

}