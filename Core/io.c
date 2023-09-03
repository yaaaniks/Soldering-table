#include "io.h"

/* Send a char through ITM */
void ITM_SendChar(uint8_t ch) {
    // read FIFO status in bit [0]:
    while(!(ITM_STIMULUS_PORT0 & 1));
    // write to ITM stimulus port0
    ITM_STIMULUS_PORT0 = ch;
}

/* Override low-level _write system call */
int _write(int file, char *ptr, int len) {
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        ITM_SendChar(*ptr++);
    }
    return len;
}
