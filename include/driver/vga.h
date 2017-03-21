#ifndef _VGA_H_
#define _VGA_H_

#include "ets_sys.h"

#define VGA_HSYNC_PIN 4
#define VGA_VSYNC_PIN 5

#define VGA_WIDTH 512
#define VGA_HEIGHT 480

void vga_scroll_down(int d);
void vga_set_pixel(int x, int y);
void vga_clear_pixel(int x, int y);

void vga_init(void);

#endif /* _VGA_H_ */
