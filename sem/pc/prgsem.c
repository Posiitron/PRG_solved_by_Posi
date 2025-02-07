/*
 * Author Posiitron
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"
#include "computation.h"
#include "event_queue.h"
#include "gui.h"
#include "main.h"
#include "prg_io_nonblock.h"
#include "utils.h"

#ifndef IO_READ_TIMEOUT_MS
#define IO_READ_TIMEOUT_MS 10
#endif

// Default values
#define DEFAULT_N 60
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_PIPE_IN "/tmp/computational_module.out"
#define DEFAULT_PIPE_OUT "/tmp/computational_module.in"

void check_and_normalize_n(int *n, bool is_n);
void print_help(void);

void *read_pipe_thread(void *d);

int main(int argc, char *argv[])
{

    if (argc == 2 &&
        (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help();
    } else if (argc > 2) {
    }

    int ret = EXIT_SUCCESS;

    int n = argc > 1 ? atoi(argv[1]) : DEFAULT_N;
    int width = argc > 2 ? atoi(argv[2]) : DEFAULT_WIDTH;
    int height = argc > 3 ? atoi(argv[3]) : DEFAULT_HEIGHT;
    const char *fname_pipe_in = argc > 5 ? argv[5] : DEFAULT_PIPE_IN;
    const char *fname_pipe_out = argc > 4 ? argv[4] : DEFAULT_PIPE_OUT;

    check_and_normalize_n(&n, true);
    check_and_normalize_n(&width, false);
    check_and_normalize_n(&height, false);

    int pipe_in = io_open_read(fname_pipe_in);
    int pipe_out = io_open_write(fname_pipe_out);

    // initialize computation, visualize
    computation_init(n, width, height);
    gui_init();

    printf(MAGENTA_COLOR "DEBUG" RESET_COLOR ": pipe in %d\n", pipe_out);
    my_assert(pipe_in != -1 && pipe_out != -1, __func__, __LINE__, __FILE__);

    enum { READ_PIPE_THRD, MAIN_THRD, WIN_THRD, NUM_THREADS };
    const char *thrd_names[] = {"ReadPipe", "Main", "GuiWin"};
    void *(*thrd_functions[])(void *) = {read_pipe_thread, main_thread,
                                         gui_win_thread};
    pthread_t threads[NUM_THREADS] = {};
    void *thrd_data[NUM_THREADS] = {};
    thrd_data[READ_PIPE_THRD] = &pipe_in;
    thrd_data[MAIN_THRD] = &pipe_out;

    for (int i = 0; i < NUM_THREADS; ++i) {
        int r =
            pthread_create(&threads[i], NULL, thrd_functions[i], thrd_data[i]);
        printf(MAGENTA_COLOR "DEBUG" RESET_COLOR ": Create thread '%s' %s\r\n",
               thrd_names[i],
               (r == 0 ? GREEN_COLOR "OK" RESET_COLOR
                       : RED_COLOR "FAIL" RESET_COLOR));
    }

    int *ex;
    for (int i = 0; i < NUM_THREADS; ++i) {
        printf(MAGENTA_COLOR "DEBUG" RESET_COLOR
                             ": Call join to the thread %s\r\n",
               thrd_names[i]);
        int r = pthread_join(threads[i], (void *)&ex);
        printf(MAGENTA_COLOR "DEBUG" RESET_COLOR
                             ": Joining the thread %s has been %s\r\n",
               thrd_names[i],
               (r == 0 ? GREEN_COLOR "OK" RESET_COLOR
                       : RED_COLOR "FAIL" RESET_COLOR));
    }

    io_close(pipe_in);
    io_close(pipe_out);

    // cleanup computation, visualize
    computation_cleanup();
    gui_cleanup();

    return ret;
}

void *read_pipe_thread(void *d)
{
    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_in = *(int *)d;
    debug("sent date to pipe_out\\n");
    debug("Read_pipe_thread - start\n");
    bool end = false;
    uint8_t msg_buf[sizeof(message)];
    size_t i = 0;
    int len = 0;

    unsigned char c;
    while (io_getc_timeout(pipe_in, IO_READ_TIMEOUT_MS, &c) > 0) {
    } // discard garbage

    while (!end) {
        int r = io_getc_timeout(pipe_in, IO_READ_TIMEOUT_MS,
                                &c); // nacitame pomoci io_getc_timeout
        if (r > 0) {                 // char has been read
            if (i == 0) {
                len = 0;
                if (get_message_size(c, &len)) {
                    msg_buf[i++] = c;
                } else {
                    fprintf(stderr,
                            "⛔ " RED_COLOR " ERROR" RESET_COLOR
                            " : unknown message type 0x%x\n",
                            c);
                }
            } else {
                // read remaining bytes of the message
                msg_buf[i++] = c;
            }
            if (len > 0 && i == len) {
                message *msg = my_alloc(sizeof(message));
                if (parse_message_buf(msg_buf, len, msg)) {
                    event ev = {.type = EV_PIPE_IN_MESSAGE};
                    ev.data.msg = msg;
                    queue_push(ev);
                } else {
                    fprintf(stderr,
                            "⛔ " RED_COLOR " ERROR" RESET_COLOR
                            " : cannot parse message type %d\n",
                            msg_buf[0]);
                    free(msg);
                }
                i = len = 0;
            }
        } else if (r == 0) {
            // TODO timeout
        } else {
            error("ERROR: reading from pipe\n");
            set_quit();
            event ev = {.type = EV_QUIT};
            queue_push(ev);
        }
        end = is_quit();
    }
    debug("Read_pipe_thread - finished\n");
    return NULL;
}

void check_and_normalize_n(int *n, bool is_n)
{
    // case n
    if (is_n) {
        if (*n <= 0) {
            *n = 1;
        } else if (*n > 1000) {
            *n = 1000;
        }
    }
    // case size
    else {
        if (*n > 1500) {
            *n = 1500;
        } else if (*n < 50) {
            *n = 50;
        } else {
            *n /= 10;
            *n *= 10;
        }
    }
}

/* end of prgsem.c*/