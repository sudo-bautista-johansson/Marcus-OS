#include <stdint.h>

#define FRAMEBUFFER ((volatile uint8_t *)0xa0000)
#define WIDTH 320
#define HEIGHT 200

static void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
    for (uint16_t row = y; row < y + height && row < HEIGHT; ++row) {
        for (uint16_t column = x; column < x + width && column < WIDTH; ++column) {
            FRAMEBUFFER[row * WIDTH + column] = color;
        }
    }
}

static void draw_frame(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    fill_rect(x, y, width, 1, 9);
    fill_rect(x, y + height - 1, width, 1, 3);
    fill_rect(x, y, 1, height, 9);
    fill_rect(x + width - 1, y, 1, height, 3);
}

int kernel_framebuffer_test(void) {
    fill_rect(0, 0, WIDTH, HEIGHT, 1);
    fill_rect(0, 0, WIDTH, 14, 0);
    fill_rect(0, 14, 28, HEIGHT - 14, 0);
    fill_rect(28, 14, 1, HEIGHT - 14, 9);
    draw_frame(42, 28, 100, 100);
    draw_frame(184, 28, 118, 70);
    draw_frame(184, 106, 118, 78);
    fill_rect(48, 36, 88, 2, 15);
    fill_rect(48, 44, 62, 2, 7);
    fill_rect(192, 38, 80, 3, 9);
    fill_rect(192, 50, 70, 5, 7);
    fill_rect(192, 62, 52, 5, 7);
    fill_rect(192, 74, 88, 5, 7);
    for (uint16_t ring = 0; ring < 3; ++ring) {
        uint16_t left = 78 - ring * 8;
        uint16_t top = 70 - ring * 8;
        fill_rect(left, top, 2, 34 + ring * 16, 9);
        fill_rect(left + 34 + ring * 16, top, 2, 34 + ring * 16, 9);
        fill_rect(left, top, 36 + ring * 16, 2, 9);
        fill_rect(left, top + 34 + ring * 16, 36 + ring * 16, 2, 9);
    }
    return FRAMEBUFFER[0] == 0 && FRAMEBUFFER[20 * WIDTH + 40] == 1;
}