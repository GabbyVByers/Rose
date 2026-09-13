
/*
 *   Header File [core.h]
 */

#ifndef ROSE_CORE_HEADER_GUARD
#define ROSE_CORE_HEADER_GUARD

#include "SDL3/SDL.h"
#include "stb_image.h"

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"

#define ROSE_MIN_WIDTH  ((size_t)32)
#define ROSE_MIN_HEIGHT ((size_t)32)

#define ROSE_COLOR_WHITE  ((ROSE_Color) { 1.0f, 1.0f, 1.0f, 1.0f })
#define ROSE_COLOR_BLACK  ((ROSE_Color) { 0.0f, 0.0f, 0.0f, 1.0f })
#define ROSE_COLOR_RED    ((ROSE_Color) { 1.0f, 0.0f, 0.0f, 1.0f })
#define ROSE_COLOR_GREEN  ((ROSE_Color) { 0.0f, 1.0f, 0.0f, 1.0f })
#define ROSE_COLOR_BLUE   ((ROSE_Color) { 0.0f, 0.0f, 1.0f, 1.0f })
#define ROSE_COLOR_PURPLE ((ROSE_Color) { 1.0f, 0.0f, 1.0f, 1.0f })
#define ROSE_COLOR_YELLOW ((ROSE_Color) { 1.0f, 1.0f, 0.0f, 1.0f })
#define ROSE_COLOR_CYAN   ((ROSE_Color) { 0.0f, 1.0f, 1.0f, 1.0f })

typedef enum ROSE_MOUSE_BUTTON {
    NULL_BUTTON,
	ROSE_MOUSE_LEFT,
	ROSE_MOUSE_MIDDLE,
	ROSE_MOUSE_RIGHT,
	ROSE_MOUSE_X1,
	ROSE_MOUSE_X2,
} ROSE_MOUSE_BUTTON;

typedef enum ROSE_KEYBOARD_BUTTON {
    ROSE_KEY_A = 4,
    ROSE_KEY_B = 5,
    ROSE_KEY_C = 6,
    ROSE_KEY_D = 7,
    ROSE_KEY_E = 8,
    ROSE_KEY_F = 9,
    ROSE_KEY_G = 10,
    ROSE_KEY_H = 11,
    ROSE_KEY_I = 12,
    ROSE_KEY_J = 13,
    ROSE_KEY_K = 14,
    ROSE_KEY_L = 15,
    ROSE_KEY_M = 16,
    ROSE_KEY_N = 17,
    ROSE_KEY_O = 18,
    ROSE_KEY_P = 19,
    ROSE_KEY_Q = 20,
    ROSE_KEY_R = 21,
    ROSE_KEY_S = 22,
    ROSE_KEY_T = 23,
    ROSE_KEY_U = 24,
    ROSE_KEY_V = 25,
    ROSE_KEY_W = 26,
    ROSE_KEY_X = 27,
    ROSE_KEY_Y = 28,
    ROSE_KEY_Z = 29,
    
    ROSE_KEY_1 = 30,
    ROSE_KEY_2 = 31,
    ROSE_KEY_3 = 32,
    ROSE_KEY_4 = 33,
    ROSE_KEY_5 = 34,
    ROSE_KEY_6 = 35,
    ROSE_KEY_7 = 36,
    ROSE_KEY_8 = 37,
    ROSE_KEY_9 = 38,
    ROSE_KEY_0 = 39,

    ROSE_KEY_F1 = 58,
    ROSE_KEY_F2 = 59,
    ROSE_KEY_F3 = 60,
    ROSE_KEY_F4 = 61,
    ROSE_KEY_F5 = 62,
    ROSE_KEY_F6 = 63,
    ROSE_KEY_F7 = 64,
    ROSE_KEY_F8 = 65,
    ROSE_KEY_F9 = 66,
    ROSE_KEY_F10 = 67,
    ROSE_KEY_F11 = 68,
    ROSE_KEY_F12 = 69,

    ROSE_KEY_ENTER = 40,
    ROSE_KEY_ESCAPE = 41,
    ROSE_KEY_BACKSPACE = 42,
    ROSE_KEY_TAB = 43,
    ROSE_KEY_SPACE = 44,
    ROSE_KEY_MINUS = 45,
    ROSE_KEY_EQUALS = 46,
    ROSE_KEY_OPEN_BRACKET = 47,
    ROSE_KEY_CLOSE_BRACKET = 48,
    ROSE_KEY_BACKSLASH = 49,
    ROSE_KEY_SEMICOLON = 51,
    ROSE_KEY_APOSTROPHE = 52,
    ROSE_KEY_GRAVE = 53,
    ROSE_KEY_COMMA = 54,
    ROSE_KEY_PERIOD = 55,
    ROSE_KEY_SLASH = 56,
    ROSE_KEY_CAPSLOCK = 57,
    ROSE_KEY_PRINTSCREEN = 70,
    ROSE_KEY_SCROLLLOCK = 71,
    ROSE_KEY_PAUSE = 72,
    ROSE_KEY_INSERT = 73,
    ROSE_KEY_HOME = 74,
    ROSE_KEY_PAGE_UP = 75,
    ROSE_KEY_DELETE = 76,
    ROSE_KEY_END = 77,
    ROSE_KEY_PAGE_DOWN = 78,
    ROSE_KEY_RIGHT_ARROW = 79,
    ROSE_KEY_LEFT_ARROW = 80,
    ROSE_KEY_DOWN_ARROW = 81,
    ROSE_KEY_UP_ARROW = 82,
    ROSE_KEY_LEFT_CTRL = 224,
    ROSE_KEY_LEFT_SHIFT = 225,
    ROSE_KEY_LEFT_ALT = 226,
    ROSE_KEY_RIGHT_CTRL = 228,
    ROSE_KEY_RIGHT_SHIFT = 229,
    ROSE_KEY_RIGHT_ALT = 230,
} ROSE_KEYBOARD_BUTTON;

#endif /* ROSE_CORE_HEADER_GUARD */

