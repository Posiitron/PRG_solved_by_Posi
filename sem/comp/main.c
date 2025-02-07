/*
 * Author Amir Akrami
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "colors.h"
#include "computation.h"
#include "event_queue.h"
#include "main.h"
#include "messages.h"
#include "utils.h"

static void process_pipe_message(event *const ev);

void *main_thread(void *d)
{
    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_out = *(int *)d;
    message msg;
    uint8_t msg_buf[sizeof(message)];
    int msg_len;
    bool quit = false;
    event newev;

    do {
        event ev = queue_pop();
        msg.type = MSG_NBR;

        switch (ev.type) {
        case EV_QUIT:
            set_quit();
            debug("Quit received!");
            break;
        case EV_GET_VERSION:
            msg.type = MSG_VERSION;
            msg.data.version.major = 3;
            msg.data.version.minor = 3;
            msg.data.version.patch = 3;
            break;
        case EV_SET_COMPUTE:
            msg.type = MSG_OK;
            break;
        case EV_COMPUTE:
            enable_comp();
            if (!compute(&msg)) {
                // debug("Current chunk computed");
                msg.type = MSG_DONE; // no need to continue
            } else {
                msg.type = MSG_COMPUTE_DATA; // enque next comp chunk
                newev.type = EV_COMPUTE; // Requeue ev to continue computation
                queue_push(newev);
            }
            break;
        case EV_ABORT:
            last_chunk(&msg);
            msg.type = MSG_OK;
            break;
        case EV_PIPE_IN_MESSAGE:
            process_pipe_message(&ev);
            break;
        default:
            break;
        }
        if (msg.type != MSG_NBR) {
            my_assert(
                fill_message_buf(&msg, msg_buf, sizeof(msg_buf), &msg_len),
                __func__, __LINE__, __FILE__);
            if (write(pipe_out, msg_buf, msg_len) == msg_len) {
                // debug("WRITE to pipe_out\n");
            } else {
                error("Failed to send message\n");
            }
        }
        quit = is_quit();
    } while (!quit);
    return NULL;
}

void process_pipe_message(event *const ev)
{
    my_assert(ev != NULL && ev->type == EV_PIPE_IN_MESSAGE && ev->data.msg,
              __func__, __LINE__, __FILE__);
    ev->type = EV_TYPE_NUM;
    const message *msg = ev->data.msg;
    switch (msg->type) {
    case MSG_SET_COMPUTE: // set compute
        info("SET_COMPUTE_from module");
        printf(CYAN_COLOR
               "DEBUG" RESET_COLOR
               ": Initialized computation at: c_re: %f, c_im: %f, d_re: %f, "
               "d_im: %f\n",
               msg->data.set_compute.c_re, msg->data.set_compute.c_im,
               msg->data.set_compute.d_re, msg->data.set_compute.d_im);

        set_compute(&(msg->data.set_compute));
        queue_push((event){.type = EV_SEND_OK});
        break;
    case MSG_GET_VERSION:
        queue_push((event){.type = EV_GET_VERSION});
        break;
    case MSG_ABORT:
        info("Aborting computation");
        queue_push((event){.type = EV_ABORT});
        abort_comp();
        break;
    case MSG_COMPUTE:
        update_data(&(msg->data.compute));
        // debug("Compute_message received");
        queue_push((event){.type = EV_COMPUTE});
        break;
    default:
        fprintf(stderr, "Unhandled pipe message type %d\n", msg->type);
        break;
    }
    free(ev->data.msg);
    ev->data.msg = NULL;
}

/* end of b3b36prg-sem/main.c */