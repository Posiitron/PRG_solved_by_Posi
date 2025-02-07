/*
 * Author Amir Akrami
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */

#include <stdio.h>
#include <termio.h>
#include <unistd.h> // for STDIN_FILENO

#include "colors.h"
#include "utils.h"

void print_help(void)
{
    FILE *file = fopen("README.md", "r");
    if (file == NULL) {
        error("README file not found.\n");
    }

    int c;
    while ((c = fgetc(file)) != EOF) {
        putchar(c);
    }

    fclose(file);
}

void my_assert(bool r, const char *fcname, int line, const char *fname)
{
    if (!r) {
        fprintf(stderr, "Error: my_assert failed:%s() line %d in %s\n", fcname,
                line, fname);
        exit(105);
    }
}
void *my_alloc(size_t size)
{
    void *ret = malloc(size);
    if (!ret) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(101);
    }
    return ret;
}

// - function -----------------------------------------------------------------
void call_termios(int reset)
{
    static struct termios tio, tioOld;
    tcgetattr(STDIN_FILENO, &tio);
    if (reset) {
        tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
    } else {
        tioOld = tio; // backup
        cfmakeraw(&tio);
        tio.c_oflag |= OPOST; // enable output processing
        tcsetattr(STDIN_FILENO, TCSANOW, &tio);
    }
}

void info(const char *str)
{
    fprintf(stderr, BLUE_COLOR "🛈 INFO" RESET_COLOR ": %s\n", str);
}

void debug(const char *str)
{
    fprintf(stderr, CYAN_COLOR "DEBUG" RESET_COLOR ": %s\n", str);
}

void error(const char *str)
{
    fprintf(stderr, "⛔ " RED_COLOR "ERROR" RESET_COLOR " : %s\n", str);
}

void warn(const char *str)
{
    fprintf(stderr, "⚠️ " YELLOW_COLOR " WARNING" RESET_COLOR ": %s\n", str);
}
