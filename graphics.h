//graphics.h
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>
typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t* pixels;
} graphics_buffer_t;
graphics_buffer_t graphics_create_buffer(uint32_t width, uint32_t height);
void graphics_destroy_buffer(graphics_buffer_t);
void graphics_putpixel(uint32_t x, uint32_t y, uint32_t color);
void graphics_putpixel_buffer(uint32_t x, uint32_t y, uint32_t color, graphics_buffer_t buffer);
void graphics_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void graphics_init(void);
void graphics_clear(uint32_t color);
void graphics_clear_buffer(uint32_t color, graphics_buffer_t buffer);
void graphics_present_buffer(graphics_buffer_t buffer);
#endif
