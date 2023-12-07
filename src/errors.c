#include "errors.h"
#include "colors.h"
#include <stdio.h>
#include <stdarg.h>



void throwError(enum ErrorType t, const char* err, ...)
{
    
    va_list ptr; 
    va_start(ptr, err);
    
    
    
    switch (t)
    {
        case WARNING:
            printf("%s", FG_COLOR_MAGENTA);
            printf("WARNING: ");
            break;
        
        case ERROR:
            printf("%s", FG_COLOR_DYELLOW);
            printf("ERROR: ");
            break;
        
        case CRITICAL_ERROR:
            printf("%s", FG_COLOR_DRED);
            printf("CRITICAL ERROR: ");
            break;
        
        default:
            break;
    }
    

    printf("%s", COLOR_RESET);
    
    vprintf(err, ptr);
    
    
    
    va_end(ptr);
    
    
    
    
    
}
