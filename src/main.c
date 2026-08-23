#include "libs/eadk.h"

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "ABLauncher";
const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

#define SLOT_B_TARGET 0x90410030UL   // slot B (0x90400000) + offset 0x10030

__attribute__((naked)) void jump(unsigned long addr) {
    __asm volatile (
        "ldrd r3, r2, [r0] \n"
        "msr  MSP, r3      \n"
        "bx   r2           \n"
    );
}

#define AIRCR (*(volatile uint32_t*)0xE000ED0C)
#define AIRCR_VECTKEY        0x05FA0000UL
#define AIRCR_SYSRESETREQ    (1UL << 2)

void system_reset(void) {
    AIRCR = AIRCR_VECTKEY | AIRCR_SYSRESETREQ;
    while (1) {}
}

#define SLOT_A_START 0x90000000UL
#define SLOT_B_START 0x90400000UL
#define SLOT_SIZE    0x00400000UL

typedef enum { SLOT_A, SLOT_B, SLOT_UNKNOWN } slot_t;

slot_t current_slot(void) {
    unsigned long here = (unsigned long)&current_slot;

    if (here >= SLOT_A_START && here < SLOT_A_START + SLOT_SIZE) {
        return SLOT_A;
    } else if (here >= SLOT_B_START && here < SLOT_B_START + SLOT_SIZE) {
        return SLOT_B;
    }
    return SLOT_UNKNOWN;
}

/*--------------------------------------------------------------*/

#define MAX_BUFFER_NB_LINES 13
#define MAX_SEGMENTS 8
#define PADDING 14

typedef struct {
    const char *text;
    eadk_color_t color;
} text_segment_t;

typedef struct {
    text_segment_t segments[MAX_SEGMENTS];
    uint8_t segment_count;
    uint16_t wait_time;
} line_buffer_t;

#define SEG(text, color) {text, color}
#define LINE(wait, ...) \
    { .segments = { __VA_ARGS__ }, \
      .segment_count = sizeof((text_segment_t[]){__VA_ARGS__}) / sizeof(text_segment_t), \
      .wait_time = wait }

line_buffer_t screen_buf[MAX_BUFFER_NB_LINES] = {0};

void clear_buffer(void) {
    eadk_display_push_rect_uniform(
        (eadk_rect_t){0, 0, 320, MAX_BUFFER_NB_LINES * PADDING},
        eadk_color_black
    );
}

void display_buffer(void) {
    for (int i = 0; i < MAX_BUFFER_NB_LINES; i++) {
        int x = 5;

        for (int j = 0; j < screen_buf[i].segment_count; j++) {
            text_segment_t *segment = &screen_buf[i].segments[j];

            eadk_display_draw_string(
                segment->text,
                (eadk_point_t){x, i * PADDING},
                false,
                segment->color,
                eadk_color_black
            );

            x += strlen(segment->text) * 7;
        }
    }
}

void shift_screen_buf(void) {
    for (int i = 0; i < MAX_BUFFER_NB_LINES - 1; i++) {
        screen_buf[i] = screen_buf[i + 1];
    }

    screen_buf[MAX_BUFFER_NB_LINES - 1] = (line_buffer_t){0};
}

#define FASTFETCH_NB_LINES 12

line_buffer_t fastfetch_buf[FASTFETCH_NB_LINES] = {
    LINE(20, SEG("{Jz<          ^nC(  ", eadk_color_orange),   SEG("u0_a437", eadk_color_green),SEG("@", eadk_color_green),SEG("Numworks", eadk_color_white)),
    LINE(20, SEG("W@@&cl        +@@W  ", eadk_color_orange),   SEG("", eadk_color_green)),
    LINE(20, SEG("M@#&@$j       +@@*  ", eadk_color_orange),   SEG("OS: ", eadk_color_green), SEG("Epsilon", eadk_color_white)),
    LINE(20, SEG("M@@]x&@a1     +@@M  ", eadk_color_orange),   SEG("Kernel: ", eadk_color_green), SEG("Super cool kernel", eadk_color_white)),
    LINE(20, SEG("M@@_ iU@@w]   _@@M  ", eadk_color_orange),   SEG("CPU: ", eadk_color_green), SEG("216 MHz ARMv7 CPU", eadk_color_white)),
    LINE(20, SEG("M@@-   _Z@@L< ~@@M  ", eadk_color_orange),   SEG("GPU: ", eadk_color_green), SEG("Yes", eadk_color_white)),
    LINE(20, SEG("M@@-     [b@@c]#@*  ", eadk_color_orange),   SEG("RAM: ", eadk_color_green), SEG("128KB ", eadk_color_white)),
    LINE(20, SEG("M@@-       /M@#8@*  ", eadk_color_orange),   SEG("Screen: ", eadk_color_green), SEG("320x240, rgb565", eadk_color_white)),
    LINE(20, SEG("M@@-        ^n%@@W  ", eadk_color_orange),   SEG("Battery: ", eadk_color_green), SEG("1450 mAh", eadk_color_white)),
    LINE(20, SEG("{Yj^          iuU1  ", eadk_color_orange),   SEG("Uptime: ", eadk_color_green), SEG("Flemme", eadk_color_white))
};

line_buffer_t messages[15] = {
    LINE(1000, SEG("> ", eadk_color_white),   SEG("Starting the launcher...", eadk_color_white)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Launcher ready", eadk_color_green)),
    LINE(1200, SEG("> ", eadk_color_white),   SEG("Slot A detected", eadk_color_white)),
    LINE(1200, SEG("> ", eadk_color_white),   SEG("Slot B detected", eadk_color_white)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Press EXE to switch to slot A", eadk_color_yellow)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Press EXE to switch to slot B", eadk_color_yellow)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("It will erase all of your data", eadk_color_red)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Waiting...", eadk_color_white)),
    LINE(50, SEG("$ ", eadk_color_white),   SEG("EXE pressed", eadk_color_yellow)),
    LINE(1500, SEG("> ", eadk_color_white),   SEG("Switching to slot A...", eadk_color_green)),
    LINE(1500, SEG("> ", eadk_color_white),   SEG("Switching to slot B...", eadk_color_green)),
};

void display_message(int index) {
    if (index < 0 || index >= sizeof(messages) / sizeof(messages[0])) {
        return;
    }

    screen_buf[MAX_BUFFER_NB_LINES - 1] = messages[index];
    clear_buffer();
    display_buffer();
    eadk_timing_msleep(messages[index].wait_time);
    shift_screen_buf();
}

int main(void) {
    slot_t slot = current_slot();

    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_black);

    for (int i = 0; i < FASTFETCH_NB_LINES; i++) {
        screen_buf[i] = fastfetch_buf[i];
        display_buffer();
        eadk_timing_msleep(fastfetch_buf[i].wait_time);
    }

    display_message(0);
    display_message(1);

    if (slot == SLOT_A) {
        display_message(2);
        display_message(5);
    } else if (slot == SLOT_B) {
        display_message(3);
        display_message(4);
    }
    display_message(6);
    display_message(7);


    while (1){
        eadk_keyboard_state_t state = eadk_keyboard_scan();
        if (eadk_keyboard_key_down(state, eadk_key_back)) break;

        if (eadk_keyboard_key_down(state, eadk_key_exe)) {
            if (slot == SLOT_A) {
                display_message(8);
                display_message(10);
                jump(SLOT_B_TARGET);
                while (1) {}
            } else if (slot == SLOT_B) {
                display_message(8);
                display_message(9);
                system_reset();
            }
        }
    }
}