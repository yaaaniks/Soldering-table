#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include <stddef.h>

enum MenuButton {
    EV_PLUS,
    EV_MINUS,
    EV_CONTROL
};

enum MenuState {
    ST_INFO = 0,
    ST_CONTROL,
    ST_WALLPAPER
};

#endif // EVENT_H