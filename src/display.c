#include "display.h"
#include "libs/eadk.h"
#include "functions.h"

const line_buffer_t messages[NB_MESSAGES] = {
    LINE(20, SEG("{Jz<          ^nC(  ", eadk_color_orange),   SEG("u0_a437", eadk_color_green),SEG("@", eadk_color_green),SEG("Numworks", eadk_color_white)),
    LINE(20, SEG("W@@&cl        +@@W  ", eadk_color_orange),   SEG("", eadk_color_green)),
    LINE(20, SEG("M@#&@$j       +@@*  ", eadk_color_orange),   SEG("OS: ", eadk_color_green), SEG("Epsilon", eadk_color_white)),
    LINE(20, SEG("M@@]x&@a1     +@@M  ", eadk_color_orange),   SEG("Kernel: ", eadk_color_green), SEG("Super cool kernel", eadk_color_white)),
    LINE(20, SEG("M@@_ iU@@w]   _@@M  ", eadk_color_orange),   SEG("CPU: ", eadk_color_green), SEG("216 MHz ARMv7 CPU", eadk_color_white)),
    LINE(20, SEG("M@@-   _Z@@L< ~@@M  ", eadk_color_orange),   SEG("GPU: ", eadk_color_green), SEG("Yes", eadk_color_white)),
    LINE(20, SEG("M@@-     [b@@c]#@*  ", eadk_color_orange),   SEG("RAM: ", eadk_color_green), SEG("128KB ", eadk_color_white)),
    LINE(20, SEG("M@@-       /M@#8@*  ", eadk_color_orange),   SEG("Screen: ", eadk_color_green), SEG("320x240, rgb565", eadk_color_white)),
    LINE(20, SEG("M@@-        ^n%@@W  ", eadk_color_orange),   SEG("Battery: ", eadk_color_green), SEG("1450 mAh", eadk_color_white)),
    LINE(20, SEG("{Yj^          iuU1  ", eadk_color_orange),   SEG("Uptime: ", eadk_color_green), SEG("Flemme", eadk_color_white)),
    LINE(20, SEG(" ", eadk_color_white)),
    LINE(800, SEG("> ", eadk_color_white),   SEG("Starting the launcher...", eadk_color_white)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Launcher ready", eadk_color_green)),
    
    LINE(800, SEG("> ", eadk_color_white),   SEG("Slot A detected", eadk_color_green)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Press EXE to switch to slot B", eadk_color_yellow)),

    LINE(800, SEG("> ", eadk_color_white),   SEG("Slot B detected", eadk_color_green)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Press EXE to switch to slot A", eadk_color_yellow)),
    
    LINE(50, SEG("> ", eadk_color_white),   SEG("It will erase all of your data", eadk_color_red)),
    LINE(50, SEG("> ", eadk_color_white),   SEG("Waiting...", eadk_color_white)),
    LINE(50, SEG("$ ", eadk_color_white),   SEG("EXE pressed", eadk_color_yellow)),
    
    LINE(1500, SEG("> ", eadk_color_white),   SEG("Switching to slot B...", eadk_color_green)),
    
    LINE(1500, SEG("> ", eadk_color_white),   SEG("Switching to slot A...", eadk_color_green)),
};

void display_line(eadk_point_t point, const line_buffer_t *line) {
    uint16_t x = point.x;
    uint16_t y = point.y;

    for (uint8_t s = 0; s < line->segment_count; s++) {
        const char *text = line->segments[s].text;
        eadk_color_t color = line->segments[s].color;

        eadk_display_draw_string(text, (eadk_point_t){x, y}, false, color, eadk_color_black);
        x += (uint16_t)(strlen(text) * CHAR_WIDTH_SMALL);
    }
}

void shift_buffer(line_buffer_t *screen_buf) {
    for (int i = 0; i < SCREEN_NB_LINES - 1; i++) {
        screen_buf[i] = screen_buf[i + 1];
    }

    screen_buf[SCREEN_NB_LINES - 1] = (line_buffer_t){0};
}

static bool is_line_empty(const line_buffer_t *line) {
    if (line->segment_count == 0) return true;
    for (uint8_t i = 0; i < line->segment_count; i++) {
        if (line->segments[i].text != NULL && line->segments[i].text[0] != '\0') {
            return false;
        }
    }
    return true;
}

int first_empty_line(line_buffer_t *screen_buffer) {
    for (int i = 0; i < SCREEN_NB_LINES; i++) {
        if (is_line_empty(&screen_buffer[i])) {
            return i;
        }
    }
    return -1;
}


static void redraw_screen(line_buffer_t *screen_buffer) {
    eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_black);
    for (int i = 0; i < SCREEN_NB_LINES; i++) {
        if (!is_line_empty(&screen_buffer[i])) {
            display_line((eadk_point_t){5, i * CHAR_HEIGHT_SMALL}, &screen_buffer[i]);
        }
    }
}

void print_message(line_buffer_t *screen_buffer, int *idx, int msg_index, bool *quit) {
    if (*idx < 0 || *idx >= SCREEN_NB_LINES) {
        shift_buffer(screen_buffer);
        redraw_screen(screen_buffer);
        *idx = SCREEN_NB_LINES - 1;
    }

    screen_buffer[*idx] = messages[msg_index];
    display_line((eadk_point_t){5, *idx * CHAR_HEIGHT_SMALL}, &screen_buffer[*idx]);
    sleep(messages[msg_index].wait_time, quit);
    *idx = first_empty_line(screen_buffer);
}