#ifndef CUSTOM_H
#define CUSTOM_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

/*
COLORS FORMAT:
    FG: foreground
    BG: background
    COLOR: color
    D: dark
    L: light
*/

// FOREGROUND COLORS

#define FG_COLOR_DEFAULT "\033[39m"
#define FG_COLOR_BLACK "\033[30m"
#define FG_COLOR_DRED "\033[31m"
#define FG_COLOR_DGREEN "\033[32m"
#define FG_COLOR_DYELLOW "\033[33m"
#define FG_COLOR_DBLUE "\033[34m"
#define FG_COLOR_DMAGENTA "\033[35m"
#define FG_COLOR_DCYAN "\033[36m"
#define FG_COLOR_LGRAY "\033[37m"
#define FG_COLOR_DGRAY "\033[90m"
#define FG_COLOR_RED "\033[91m"
#define FG_COLOR_GREEN "\033[92m"
#define FG_COLOR_YELLOW "\033[93m"
#define FG_COLOR_BLUE "\033[94m"
#define FG_COLOR_MAGENTA "\033[95m"
#define FG_COLOR_CYAN "\033[96m"
#define FG_COLOR_WHITE "\033[97m"






// BACKGROUND COLORS

#define BG_COLOR_DEFAULT "\033[49m"
#define BG_COLOR_BLACK "\033[40m"
#define BG_COLOR_DRED "\033[41m"
#define BG_COLOR_DGREEN "\033[42m"
#define BG_COLOR_DYELLOW "\033[43m"
#define BG_COLOR_DBLUE "\033[44m"
#define BG_COLOR_DMAGENTA "\033[45m"
#define BG_COLOR_DCYAN "\033[46m"
#define BG_COLOR_LGRAY "\033[47m"
#define BG_COLOR_DGRAY "\033[100m"
#define BG_COLOR_RED "\033[101m"
#define BG_COLOR_GREEN "\033[102m"
#define BG_COLOR_ORANGE "\033[103m"
#define BG_COLOR_BLUE "\033[104m"
#define BG_COLOR_MAGENTA "\033[105m"
#define BG_COLOR_CYAN "\033[106m"
#define BG_COLOR_WHITE "\033[107m"




#define COLOR_RESET "\033[0m"



void CAprintf(const char* FG, const char* BG, const char* format, ...); // prints all
void CFprintf(const char* FG, const char* format, ...); // prints only foreground
void CBprintf(const char* BG, const char* format, ...); // prints only background


#endif /*CUSTOM_H*/