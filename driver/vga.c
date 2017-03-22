#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "driver/spi.h"
#include "driver/vga.h"

#define US_TO_RTC_TIMER_TICKS(t) \
	((t) ? \
	 (((t) > 0x35A) ? \
	  (((t)>>2) * ((APB_CLK_FREQ>>4)/250000) + ((t)&0x3) * ((APB_CLK_FREQ>>4)/1000000)) : \
	  (((t) *(APB_CLK_FREQ>>4)) / 1000000)) : \
	 0)

#define FRC1_ENABLE_TIMER BIT7
#define FRC1_AUTO_LOAD BIT6

//TIMER PREDIVED MODE
typedef enum {
	DIVDED_BY_1 = 0,
	DIVDED_BY_16 = 4,
	DIVDED_BY_256 = 8,
} TIMER_PREDIVED_MODE;

typedef enum {
	TM_LEVEL_INT = 1,
	TM_EDGE_INT = 0,
} TIMER_INT_MODE;

static void spi_mast_send1(uint8 spi_no, void *data, int len)
{
	while(READ_PERI_REG(SPI_CMD(spi_no)) & SPI_USR);
	os_memcpy((void *)SPI_W0(spi_no), data, len);
}

static void spi_mast_send2(uint8 spi_no, int len)
{
	int bitlen = len << 3;
	// default disable COMMAND, ADDR, MOSI, DUMMY, MISO, and DOUTDIN (aka full-duplex)
	CLEAR_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_COMMAND|SPI_USR_ADDR|SPI_USR_MOSI|SPI_USR_DUMMY|SPI_USR_MISO|SPI_DOUTDIN);
	// default set bit lengths
	WRITE_PERI_REG(SPI_USER1(spi_no),
		       ((0 - 1) & SPI_USR_ADDR_BITLEN) << SPI_USR_ADDR_BITLEN_S |
		       ((bitlen - 1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S |
		       ((0 - 1) & SPI_USR_DUMMY_CYCLELEN) << SPI_USR_DUMMY_CYCLELEN_S |
		       ((0 - 1) & SPI_USR_MISO_BITLEN) << SPI_USR_MISO_BITLEN_S);
	// set MOSI
	SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_USR_MOSI);
	// start transaction
	SET_PERI_REG_MASK(SPI_CMD(spi_no), SPI_USR);
}

static int cur_y, scroll;
static unsigned char fb[480][64];
static unsigned char empty[64];
static unsigned char *line;
static int vsync;

static void vga_handler(void)
{
	GPIO_OUTPUT_SET(VGA_HSYNC_PIN, 0);
	spi_mast_send1(HSPI, line, 64);
	os_delay_us(2);
	GPIO_OUTPUT_SET(VGA_HSYNC_PIN, 1);
	spi_mast_send2(HSPI, 64);

	GPIO_OUTPUT_SET(VGA_VSYNC_PIN, vsync);

	asm("":::"memory");

	cur_y++;
	switch(cur_y) {
	case 525: cur_y = 0; break;
	case 490: vsync = 0; break;
	case 492: vsync = 1; break;
	}

	line = (cur_y < 480) ? fb[(cur_y + scroll) % 480] : empty;
}

void vga_scroll_down(int d)
{
	int i;
	int s = scroll;

	for (i = 0; i < d; i++) {
		os_memset(fb[s], 0, 64);
		s++;
		if (s >= 480)
			s -= 480;
	}

	scroll = s;
}

void vga_set_pixel(int x, int y)
{
	int i = x >> 3;
	int b = 7 - (x & 7);
	int m = 1 << b;
	fb[(y + scroll) % 480][i] |= m;
}

void vga_clear_pixel(int x, int y)
{
	int i = x >> 3;
	int b = 7 - (x & 7);
	int m = 1 << b;
	fb[(y + scroll) % 480][i] &= ~m;	
}

void vga_init(void)
{
	line = fb[0];
	vsync = 1;
	spi_master_init(HSPI, 0, 0, 4);
	spi_mast_byte_order(HSPI, SPI_ORDER_LSB);

	// setup timer
	RTC_REG_WRITE(FRC1_CTRL_ADDRESS,
		      FRC1_AUTO_LOAD | DIVDED_BY_16 | FRC1_ENABLE_TIMER | TM_EDGE_INT);
        ETS_FRC_TIMER1_NMI_INTR_ATTACH(vga_handler);
	RTC_REG_WRITE(FRC1_LOAD_ADDRESS, US_TO_RTC_TIMER_TICKS(32));
	TM1_EDGE_INT_ENABLE();
	ETS_FRC1_INTR_ENABLE();
}
