#include "graphics.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void stretch_walk(graphics_buffer_t source, graphics_buffer_t target) {
	static int walk_x = 0;
	static int direction = 1;
	if(walk_x == 0) {
		direction = 1;
	}
	if(walk_x == source.width - 1) {
		direction = -1;
	}
	graphics_buffer_t temp_buffer = graphics_create_buffer(source.width, source.height);
	for(int y = 0; y < source.height; y++) {
		float normalized_y = (float)y / (float)source.height;
		uint32_t scaled_width = (float)source.width * normalized_y;
		for(int x = 0; x < scaled_width; x++) {
			uint32_t offset_x = walk_x * (1 - normalized_y);
			float normalized_x = (float)x / (float)scaled_width;
			uint32_t interpolated_x = source.width * normalized_x;
			uint32_t interpolated_y = source.height * normalized_y;
			graphics_putpixel_buffer(x + offset_x, y, source.pixels[interpolated_y * source.height + interpolated_x], temp_buffer);
		}
	}
	for(int y = 0; y < temp_buffer.height; y++) {
		for(int x = 0; x < temp_buffer.width; x++) {
			target.pixels[y * temp_buffer.height + x] = temp_buffer.pixels[y * temp_buffer.height + x];
		}
	}
	walk_x += direction;
	graphics_destroy_buffer(temp_buffer);
}
void shift_x(graphics_buffer_t buffer) {
	for(int y = 0; y < buffer.height; y++) {
		for(int x = 0; x < buffer.width; x++) {
			uint32_t index = y * buffer.height + x;
			uint32_t new_x = x + 1;
			uint32_t temp = buffer.pixels[index];
			if(new_x >= buffer.width) {
				new_x = 0;
			}
			uint32_t new_index = y * buffer.height + new_x;
			buffer.pixels[index] = buffer.pixels[new_index];
			buffer.pixels[new_index] = temp;
		}
	}
}
void shift_y(graphics_buffer_t buffer) {
	for(int y = 0; y < buffer.height; y++) {
		for(int x = 0; x < buffer.width; x++) {
			uint32_t index = y * buffer.height + x;
			uint32_t new_y = y + 1;
			uint32_t temp = buffer.pixels[index];
			if(new_y >= buffer.height) {
				new_y = 0;
			}
			uint32_t new_index = new_y * buffer.height + x;
			buffer.pixels[index] = buffer.pixels[new_index];
			buffer.pixels[new_index] = temp;
		}
	}
}
// --- Simple BMP loader ---
graphics_buffer_t loadBMP(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if(!file) { printf("Failed to open BMP\n"); exit(1); }
    uint8_t header[54];
    fread(header, sizeof(uint8_t), 54, file);
    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
    int dataSize = width * height * 3;
    uint8_t* data = malloc(dataSize);
    fread(data, sizeof(uint8_t), dataSize, file);
    fclose(file);
    uint32_t* pixels = malloc(sizeof(uint32_t) * width * height);
    // BMP stores BGR
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            int i = (y*width + x)*3;
            uint8_t b = data[i+0];
            uint8_t g = data[i+1];
            uint8_t r = data[i+2];
            pixels[(width*height) - (y*width + (width - x))] = (r<<16) | (g<<8) | b;
        }
    }
    free(data);
    graphics_buffer_t tex = {width, height, pixels};
    return tex;
}

int main(void){
	graphics_buffer_t image = loadBMP("image.bmp");
	graphics_buffer_t frame = graphics_create_buffer(image.width, image.height);
	graphics_init();
	int running = 1;
	while(running) {
		shift_y(image);
		shift_x(image);
		stretch_walk(image, frame);
		graphics_present_buffer(frame);
		usleep(10000);
	}
	return 0;
}

