#include "colors.h"


void CAprintf(const char* FG, const char* BG, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    printf("%s%s", FG, BG);
    vprintf(format, args);
    printf("%s", COLOR_RESET);
    va_end(args);
}



void CFprintf(const char* FG, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    printf("%s", FG);
    vprintf(format, args);
    printf("%s", COLOR_RESET);
    va_end(args);
}



void CBprintf(const char* BG, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    printf("%s", BG);
    vprintf(format, args);
    printf("%s", COLOR_RESET);
    va_end(args);
}