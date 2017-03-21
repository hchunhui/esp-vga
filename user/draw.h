#ifndef _DRAW_H_
#define _DRAW_H_

#include "ets_sys.h"
#include "osapi.h"

void ICACHE_FLASH_ATTR draw_asc(int x, int y, int c);
void ICACHE_FLASH_ATTR draw_gb(int x, int y, int fst, int snd);

#endif /* _DRAW_H_ */
