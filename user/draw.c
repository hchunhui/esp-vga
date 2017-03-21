#include "ets_sys.h"
#include "osapi.h"
#include "driver/vga.h"
#include "asc16.h"
#include "hzk16.h"
#include "draw.h"

void ICACHE_FLASH_ATTR draw_asc(int x, int y, int c)
{
	int i, j;
	unsigned char p[16];
	unsigned char ch;

	os_memcpy(p, ___asc16 + c * 16, 16);

	for( j = 0; j < 16; j++) {
		ch = p[j];
		for( i = 0; i < 8; i++) {
			if(ch & 1)
				vga_set_pixel(x + (7 - i), y + j);
			else
				vga_clear_pixel(x + (7 - i), y + j);
			ch >>= 1;
		}
	}
}

void ICACHE_FLASH_ATTR draw_gb(int x, int y, int fst, int snd)
{
	int i, j;
	unsigned char p[32];
	unsigned int ch;
	int loc = (94 * (fst - 0xa0 - 1) + (snd - 0xa0 - 1)) * 32;

	os_memcpy(p, ___hzk16 + loc, 32);

	for( j = 0; j < 16; j++) {
		ch = ((p[j * 2]) << 8) | p[j * 2 + 1];
		for( i = 0; i < 16; i++) {
			if(ch & 1)
				vga_set_pixel(x + (15 - i), y + j);
			else
				vga_clear_pixel(x + (15 - i), y + j);
			ch >>= 1;
		}
	}
}
