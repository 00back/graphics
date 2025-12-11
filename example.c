#include "graphics.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {
    int width;
    int height;
    uint32_t* pixels;
} Image;
// --- Simple BMP loader ---
Image loadBMP(const char* filename) {
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
            pixels[y*width + x] = (r<<16) | (g<<8) | b;
        }
    }
    free(data);
    Image tex = {width, height, pixels};
    return tex;
}

int main(void){
	int width = 1920;
	int height = 1080;
	int position_x = 0;
	int running = 1;
	Image image = loadBMP("image.bmp");
	graphics_init();
	graphics_buffer_t frame = graphics_create_buffer(width, height);
	while(running) {
		for(int y = height; y >= 0; y--) {
			float normalized_y = (float)y / (float)height;
			float scaled_width = (width - 1) - normalized_y * (float)width;
			for(int x = 0; x < scaled_width; x++) {
				int x_offset = position_x * normalized_y;
				float normalized_x = (float)x / (float)scaled_width;
				int image_x = image.width * normalized_x;
				int image_y = image.height * normalized_y;
				graphics_putpixel_buffer(x_offset + x, y, image.pixels[(image.width * image.height) - (image_y * image.height + (image.width - 1 - image_x))], frame);
			}
		}
		position_x++;
		if(position_x >= width) {
			position_x = 0;
		}
		graphics_present_buffer(frame);
		graphics_clear_buffer(0x000000, frame);
	}
	graphics_destroy_buffer(frame);
	return 0;
}

