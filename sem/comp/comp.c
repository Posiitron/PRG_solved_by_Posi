/*
 * Author Amir Akrami
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "colors.h"
#include "event_queue.h"
#include "keyboard.h"
#include "main.h"
#include "prg_io_nonblock.h"
#include "utils.h"

#ifndef IO_READ_TIMEOUT_MS
#define IO_READ_TIMEOUT_MS 100
#endif

// Defaults
#define DEFAULT_PIPE_OUT "/tmp/computational_module.out"
#define DEFAULT_PIPE_IN "/tmp/computational_module.in"

void *read_pipe_thread(void *d);

int main(int argc, char *argv[])
{
    int ret = EXIT_SUCCESS;
    const char *fname_pipe_in = argc > 1 ? argv[1] : DEFAULT_PIPE_IN;
    const char *fname_pipe_out = argc > 2 ? argv[2] : DEFAULT_PIPE_OUT;
    int pipe_in = io_open_read(fname_pipe_in);
    int pipe_out = io_open_write(fname_pipe_out);
    printf(CYAN_COLOR "DEBUG" RESET_COLOR ": pipe in %d\n", pipe_out);
    my_assert(pipe_in != -1 && pipe_out != -1, __func__, __LINE__, __FILE__);

    enum { KEYBOARD_THRD, READ_PIPE_THRD, MAIN_THRD, NUM_THREADS };
    const char *thrd_names[] = {"Keyboard", "ReadPipe", "Main"};
    void *(*thrd_functions[])(void *) = {keyboard_thread, read_pipe_thread,
                                         main_thread};
    pthread_t threads[NUM_THREADS] = {};
    void *thrd_data[NUM_THREADS] = {};
    thrd_data[READ_PIPE_THRD] = &pipe_in;
    thrd_data[MAIN_THRD] = &pipe_out;

    for (int i = 0; i < NUM_THREADS; ++i) {
        int r =
            pthread_create(&threads[i], NULL, thrd_functions[i], thrd_data[i]);
        printf(CYAN_COLOR "DEBUG" RESET_COLOR ": Create thread '%s' %s\r\n",
               thrd_names[i],
               (r == 0 ? GREEN_COLOR "OK" RESET_COLOR
                       : RED_COLOR "FAIL" RESET_COLOR));
    }

    int *ex;
    for (int i = 0; i < NUM_THREADS; ++i) {
        printf(CYAN_COLOR "DEBUG" RESET_COLOR
                          ": Call join to the thread %s\r\n",
               thrd_names[i]);
        int r = pthread_join(threads[i], (void *)&ex);
        printf(CYAN_COLOR "DEBUG" RESET_COLOR
                          ": Joining the thread %s has been %s\r\n",
               thrd_names[i],
               (r == 0 ? GREEN_COLOR "OK" RESET_COLOR
                       : RED_COLOR "FAIL" RESET_COLOR));
    }

    io_close(pipe_in);
    io_close(pipe_out);

    return ret;
}

void *read_pipe_thread(void *d)
{
    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_in = *(int *)d;
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

/* end of prgsem.c*/