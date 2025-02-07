#ifndef __UTILLS_H__
#define __UTILLS_H__

#include <stdbool.h>
#include <stdlib.h>

void my_assert(bool r, const char *fcname, int line, const char *fname);
void * my_alloc(size_t size);

void call_termios(int reset);

void info(const char *str);
void debug(const char *str);
void error(const char *str);
void warn(const char *str);

#endif 