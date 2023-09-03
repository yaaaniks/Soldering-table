#include "tempControl.h"
#include "constants.h"

uint16_t userTemperature;
float currentTemperature;
const int WINDOW_SIZE = 5; // Размер окна скользящего среднего

uint16_t slidingAverage(const uint16_t data[], int dataSize, int currentIndex) {
    uint32_t sum = 0;
    for (int i = currentIndex; i > currentIndex - WINDOW_SIZE; --i) {
        if (i >= 0) {
            sum += data[i];
        }
    }
    uint16_t average = sum / WINDOW_SIZE;
    return average;
}


void updateTemp(volatile uint16_t *adcData) { 
    currentTemperature = slidingAverage((const uint16_t*)adcData, 16, 1);
    currentTemperature = currentTemperature / MAX_RAW_VALUE * 3.3;
    fifoPush(&fsm, EV_ADC);
}

void updateScreen(void) {
    char str[20];
    // Display ADC value
    // snprintf(str, sizeof(str), "%.2f", currentTemperature);
    sprintf(str, "ADC:%.6f", currentTemperature);
    ssd1306_SetCursor(5, 20);
    ssd1306_WriteString(str, Font_7x10, White);
    char str1[20];
    // Display user temperature
    sprintf(str1, "User:%d", userTemperature);
    ssd1306_SetCursor(5, 40);
    ssd1306_WriteString(str1, Font_7x10, White);

    ssd1306_UpdateScreen();
}