#include "menu.h"

static enum MenuState current = ST_INFO; 
static uint32_t timestamp = 0;

void createControlFrame();
void createInfoFrame();

void (*stateHandler[])() = {
	[ST_CONTROL] = createControlFrame,
	[ST_INFO] = createInfoFrame
};

void createControlFrame() {
	return;
}

void createInfoFrame() {
	ssd1306_Fill(Black);
    ssd1306_DrawRectangle(1, 1, 127, 63, White);
	ssd1306_SetCursor(6, 15);
    ssd1306_WriteString("Temperature: ", Font_6x8, White);

	ssd1306_DrawCircle(105, 15, 1, White);
    ssd1306_SetCursor(110, 15);
    ssd1306_WriteChar('C', Font_6x8, White);

	ssd1306_SetCursor(6, 30);
	ssd1306_WriteString("User: ", Font_6x8, White);

	ssd1306_DrawCircle(105, 30, 1, White);
    ssd1306_SetCursor(110, 30);
    ssd1306_WriteChar('C', Font_6x8, White);
	ssd1306_UpdateScreen();
}

void menuInit() {
    ssd1306_Init();
    createInfoFrame();
}

void drawWallpaper() {
	for (uint8_t i = 0; i < 9; ++i) {
		ssd1306_Fill(Black);
    	ssd1306_DrawBitmap(0, 0, monkeyAnimation[i], 128, 64, White);
		ssd1306_UpdateScreen();
		delay_ms(20);
	}
}

void processClicked(enum MenuButton clicked) {
	switch (clicked) {
		case EV_PLUS:
		{
			if (current == ST_CONTROL) {
				
			}
			break;
		}
		case EV_MINUS:
		{
			break;
		}

		default:
		{
			if (getTimestamp() - timestamp <= STATE_SW_TIME) {
				current ^= ST_INFO; // switch to info or control
				(*stateHandler[current])();
			} 
			timestamp = getTimestamp();
			break;
		}
	}
}