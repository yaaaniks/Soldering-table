#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "event.h"
#include "timer.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "bitmap.h"

#define STATE_SW_TIME   200U

void createInfoFrame(void);
void menuInit(void);
void drawWallpaper(void);
void processClicked(enum MenuButton clicked);

#endif // MENU_H