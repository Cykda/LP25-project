#ifndef ERRORS_H
#define ERRORS_H

#include <stdarg.h>

typedef enum ErrorType errtype;
typedef enum ErrorType {WARNING = 0, ERROR = 1, CRITICAL_ERROR = 2};

void throwError(enum ErrorType t, const char* err, ...);



#endif /*ERRORS_H*/