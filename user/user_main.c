#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "driver/vga.h"
#include "draw.h"
#include "net.h"

static void ICACHE_FLASH_ATTR delayf()
{
	int i, j;
	os_printf("init...\n");
	vga_init();
	net_init();
	for (j = 0; j < 26; j++) {
		draw_asc(j * 8, 0, 'a' + j);
		draw_asc(j * 8, 16, 'A' + j);
	}
	for (j = 0; j < 32; j++) {
		draw_gb(j * 16, 32, 0xce, 0xd0 + j);
	}
}

static os_timer_t delayt;

void ICACHE_FLASH_ATTR user_init(void)
{
	struct station_config sta_conf = {
		.ssid = "",
		.password = "",
		.bssid_set = 0,
	};

	struct softap_config ap_conf = {
		.ssid = "esp",
		.password = "12345678",
		.ssid_len = 3,
		.authmode = AUTH_WPA_WPA2_PSK,
		.max_connection = 4,
	};

	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	gpio_init();
	os_printf("\nuser_init...\n");

	wifi_set_opmode(SOFTAP_MODE);
	//wifi_station_set_config(&sta_conf);
	wifi_softap_set_config(&ap_conf);
	//wifi_station_set_auto_connect(false);

	os_timer_setfn(&delayt, delayf, NULL);
	os_timer_arm(&delayt, 5000, 0);
}
