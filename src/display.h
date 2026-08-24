#ifndef DISPLAY_H
#define DISPLAY_H

#include "libs/eadk.h"

#define SCREEN_NB_LINES 17
#define NB_MESSAGES 22 

#define MAX_SEGMENTS 8

#define NB_MESSAGES_FASTFETCH 11

#define CHAR_WIDTH_SMALL 7
#define CHAR_HEIGHT_SMALL 14

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
    { \
        .segments = { __VA_ARGS__ }, \
        .segment_count = sizeof((text_segment_t[]){__VA_ARGS__}) / sizeof(text_segment_t), \
        .wait_time = wait, \
    }

extern const line_buffer_t messages[NB_MESSAGES];

void display_line(eadk_point_t point, const line_buffer_t *line);

int first_empty_line(line_buffer_t *screen_buffer);

void print_message(line_buffer_t *screen_buffer, int *idx, int msg_index, bool *quit);

#endif