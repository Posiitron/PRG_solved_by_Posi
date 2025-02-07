/*
 * Author Posiitron
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#include <stdio.h>
#include <stdlib.h>

#include "event_queue.h"
#include "keyboard.h"
#include "utils.h"

void *keyboard_thread(void *d)
{
    debug("Keyboard_thread - start\n");
    call_termios(0); // raw mode
    int c;
    event ev;
    while ((c = getchar()) != 'q') {

        info("Unknown command. 🤔 Are you looking to quit? 👉 Press 'q' to "
             "quit.\n");
    }
    info("Quitting... \n");
    set_quit();
    ev.type = EV_QUIT;
    queue_push(ev);
    call_termios(1); // normal mode
    debug("Keyboard_thread - end\n");
    return NULL;
}
