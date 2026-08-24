#include "libs/eadk.h"
#include "display.h"
#include "functions.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "ABLauncher";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

int main(void) {
    bool quit = false;
    slot_t slot = current_slot();
    
    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_black);
    line_buffer_t *screen_buffer = calloc(SCREEN_NB_LINES, sizeof(line_buffer_t));
    if (screen_buffer == NULL) return 1;
    
    int idx = 0;
    
    for (int i = 0; i < NB_MESSAGES_FASTFETCH + 2; i++) {
        print_message(screen_buffer, &idx, i, &quit);
    }
    if (quit) return 0;
    
    if (slot == SLOT_A) {
        print_message(screen_buffer, &idx, 13, &quit);
        print_message(screen_buffer, &idx, 14, &quit);
    } else if (slot == SLOT_B) {
        print_message(screen_buffer, &idx, 15, &quit);
        print_message(screen_buffer, &idx, 16, &quit);
    }
    if (quit) return 0;
    
    for (int i = 17; i < 19; i++) {
        print_message(screen_buffer, &idx, i, &quit);
    }
    if (quit) return 0;
    
    while (1) {
        eadk_keyboard_state_t state = eadk_keyboard_scan();
        
        if (eadk_keyboard_key_down(state, eadk_key_exe)) {
            print_message(screen_buffer, &idx, 19, &quit);
            
            if (slot == SLOT_A) {
                print_message(screen_buffer, &idx, 20, &quit);
                free(screen_buffer);
                if (quit) return 0;
                jump(SLOT_B_TARGET);
                while (1) {}
            } else if (slot == SLOT_B) {
                print_message(screen_buffer, &idx, 21, &quit);
                free(screen_buffer);
                if (quit) return 0;
                jump(SLOT_A_TARGET);
                while (1) {}
            } else {
                break;
            }
        }
        
        if (eadk_keyboard_key_down(state, eadk_key_back)) break;
    }
    
    free(screen_buffer);
    return 0;
}