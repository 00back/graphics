//graphics.c
#include "graphics.h"
#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
static const char* FB_PATH = "/dev/fb0";
static uint8_t* framebuffer;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
graphics_buffer_t graphics_create_buffer(uint32_t buffer_width, uint32_t buffer_height) {
	graphics_buffer_t buffer;
	buffer.pixels = malloc(sizeof(uint32_t) * buffer_width * buffer_height);
	buffer.width = buffer_width;
	buffer.height = buffer_height;
	return buffer;
}
void graphics_destroy_buffer(graphics_buffer_t buffer) {
	for(uint32_t y = 0; y < buffer.height; ++y) {
		for(uint32_t x = 0; x < buffer.width; ++x) {
			free(&buffer.pixels[y * buffer.width + x]);
		}
	}
	free(buffer.pixels);
}
void graphics_putpixel(uint32_t x, uint32_t y, uint32_t color) {
	long y_offset = y + vinfo.yoffset;
	long x_offset = x + vinfo.xoffset;
	int bytes_per_pixel = vinfo.bits_per_pixel / 8;
	long offset = y_offset * finfo.line_length + x_offset * bytes_per_pixel;
	*(uint32_t *)(framebuffer + offset) = color;
}
void graphics_putpixel_buffer(uint32_t x, uint32_t y, uint32_t color, graphics_buffer_t buffer) {
	buffer.pixels[y * buffer.width + x] = color;
}
static void draw_line_low(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int yi = 1;
	if(dy < 0) {
		yi = -1;
		dy = -dy;
	}
	int D = (2 * dy) - dx;
	int y = y0;
	for(int x = x0; x <= x1; x++) {
		graphics_putpixel(x, y, color);
		if(D > 0) {
			y = y + yi;
			D = D + (2 * (dy - dx));
		}
		else {
			D = D + 2 * dy;
		}
	}

}
static void draw_line_high(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xi = 1;
	if(dx < 0) {
		xi = -1;
		dx = -dx;
	}
	int D = (2 * dx) - dy;
	int x = x0;
	for(int y = y0; y <= y1; y++) {
		graphics_putpixel(x, y, color);
		if(D > 0) {
			x = x + xi;
			D = D + (2 * (dx - dy));
		}
		else {
			D = D + 2 * dx;
		}
	}

}
void graphics_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
	if(abs(y1 - y0) < abs(x1 - x0)) {
		if(x0 > x1) {
			draw_line_low(x1, y1, x0, y0, color);
		}
		else {
			draw_line_low(x0, y0, x1, y1, color);
		}
	}
	else {
		if(y0 > y1) {
			draw_line_high(x1, y1, x0, y0, color);
		}
		else {
			draw_line_high(x0, y0, x1, y1, color);
		}
	}
}
void graphics_clear(uint32_t color) {
	int bytes_per_pixel = vinfo.bits_per_pixel / 8;
	for(uint32_t y = 0; y < vinfo.yres; ++y) {
		for(uint32_t x = 0; x < vinfo.xres; ++x) {
			long offset = (y + vinfo.yoffset) * finfo.line_length + (x + vinfo.xoffset) * bytes_per_pixel;
			*(uint32_t *)(framebuffer + offset) = color;
		}
	}
}
void graphics_clear_buffer(uint32_t color, graphics_buffer_t buffer) {
	for(uint32_t y = 0; y < buffer.height; ++y) {
		for(uint32_t x = 0; x < buffer.width; ++x) {
			buffer.pixels[y * buffer.width + x] = color;
		}
	}
}
void graphics_present_buffer(graphics_buffer_t buffer) {
	if(!framebuffer || buffer.width > vinfo.xres || buffer.height > vinfo.yres) {
		return;
	}
	int bytes_per_pixel = vinfo.bits_per_pixel / 8;
	for(uint32_t y = 0; y < buffer.height; ++y) {
		long offset = (y + vinfo.yoffset) * finfo.line_length + vinfo.xoffset * bytes_per_pixel;
		memcpy(framebuffer + offset, buffer.pixels + y * buffer.width, buffer.width * sizeof(uint32_t));
	}
}
void graphics_init(void) {
	int fb = open(FB_PATH, O_RDWR);
	if(fb < 0) {
		perror("Error opening framebuffer");
		return;
	}
	if(ioctl(fb, FBIOGET_VSCREENINFO, &vinfo)) {
		perror("Error reading variable screen information");
		close(fb);
		return;
	}
	if(ioctl(fb, FBIOGET_FSCREENINFO, &finfo)) {
		perror("Error reading fixed screen information");
		close(fb);
		return;
	}
	framebuffer = mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
	if((intptr_t)framebuffer == -1) {
		perror("Error mmapping framebuffer");
		close(fb);
		return;
	}
}
