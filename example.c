#include "graphics.h"
#include <unistd.h>
#include <stdio.h>
int main() {
	graphics_init();
	graphics_draw_line(0,0,512,512, 0xFF00FF);
}
