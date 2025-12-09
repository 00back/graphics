#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>
void graphics_putpixel(uint32_t x, uint32_t y, uint32_t color);
void graphics_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void graphics_init(void);
void graphics_clear(uint32_t color);
#endif
