/*
 * Author Posiitron
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
#include "gui.h"
#include "main.h"
#include "messages.h"
#include "utils.h"
#include "xwin_sdl.h"

static void process_pipe_message(event *const ev);

void *main_thread(void *d)
{
    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_out = *(int *)d;
    message msg;
    uint8_t msg_buf[sizeof(message)];
    int msg_len;
    bool quit = false;

    do {
        event ev = queue_pop();
        msg.type = MSG_NBR;
        switch (ev.type) {
        case EV_QUIT:
            set_quit();
            debug("Quit received");
            break;
        case EV_GET_VERSION:
            msg.type = MSG_GET_VERSION;
            break;
        case EV_SET_COMPUTE:
            info(set_compute(&msg) ? "Set compute" : "Set compute failed");
            break;
        case EV_COMPUTE:
            set_pc_mode_state(false);
            if (is_abort()) enable_comp();
            if (!is_set_compute()) {
                info("Computation initial state was not set, setting to "
                     "defaults");
                info(set_compute(&msg) ? "Set compute" : "Set compute failed");
                queue_push((event){.type = EV_COMPUTE});
                break;
            }
            compute(&msg);
            break;
        case EV_ABORT:
            if (is_computing()) {
                msg.type = MSG_ABORT;
                info("Computation aborted");
                abort_comp();
            }
            break;
        case EV_PIPE_IN_MESSAGE:
            process_pipe_message(&ev);
            break;
        case EV_COMPUTE_PC:
            enable_comp();
            if (!is_pc_mode()) {
                set_pc_mode_state(true);
                info("Entering PC mode...");
            }
            if (compute_on_pc(&msg)) {
                info("Compute PC");
                gui_refresh();
            } else
                info("Compute PC failed");
            break;
        case EV_REFRESH:
            if (is_pc_mode()) {
                info("Resetting view");
                reset_pc_view();
                compute_on_pc(&msg);
                gui_refresh();
            } else {
                info("Resetting chunk ID");
                reset_cid();
            }
            break;
        case EV_CLEAR_BUFFER:
            info("Clearing buffer");
            set_pc_mode_state(false);
            clear_buffer();
            gui_refresh();
            break;
        case EV_ZOOM:
            if (zoom(ev.data.param)) {
                compute_on_pc(&msg);
                gui_refresh();
            }
            break;
        case EV_MOVE:
            if (move(ev.data.param)) {
                compute_on_pc(&msg);
                gui_refresh();
            }
            break;
        case EV_PAN:
            pan();
            compute_on_pc(&msg);
            gui_refresh();
            break;
        case EV_VARIATE_C:
            if (is_variating()) {
                queue_push((event){.type = EV_VARIATE_C});
            }
            variate_c_const();
            compute_on_pc(&msg);
            gui_refresh();
            sleep_ms(150);
            break;
        case EV_HELP:
            print_help();
            break;
        case EV_PNG:
            save_to_png();
            break;
        case EV_JPG:
            save_to_jpg();
            break;
        default:
            break;
        } // switch end
        if (msg.type != MSG_NBR) {
            my_assert(
                fill_message_buf(&msg, msg_buf, sizeof(msg_buf), &msg_len),
                __func__, __LINE__, __FILE__);

            if (write(pipe_out, msg_buf, msg_len) == msg_len) {
                // debug("sent date to pipe_out\n");
                /*printf("MSG set comp: c_re: %f, c_im: %f, d_re: %f, d_im:
       %f\n", msg.data.set_compute.c_re, msg.data.set_compute.c_im,
       msg.data.set_compute.d_re,
       msg.data.set_compute.d_im);*/
            } else
                error("failed to send\n");
        }
        quit = is_quit();
    } while (!quit);
    printf("\n");
    info("Quitting...");
    return NULL;
}

void process_pipe_message(event *const ev)
{
    my_assert(ev != NULL && ev->type == EV_PIPE_IN_MESSAGE && ev->data.msg,
              __func__, __LINE__, __FILE__);
    ev->type = EV_TYPE_NUM;
    const message *msg = ev->data.msg;
    switch (msg->type) {
    case MSG_OK:
        info(GREEN_COLOR "OK" RESET_COLOR);
        break;
    case MSG_COMPUTE_DATA:
        if (!is_abort()) {
            update_data(&(msg->data.compute_data));
        }
        break;
    case MSG_DONE:
        gui_refresh();
        if (!is_abort()) {
            if (is_done()) {
                update_comp_state();
                info("Computation done");
            } else {
                queue_push((event){.type = EV_COMPUTE});
                // info("computation_in");
            }
        }
        break;
    case MSG_VERSION:
        fprintf(stderr, "INFO: Module version %d.%d-p%d\n",
                msg->data.version.major, msg->data.version.minor,
                msg->data.version.patch);
        break;
    default:
        fprintf(stderr, "Unhandled pipe message type %d\n", msg->type);
        break;
    }
    free(ev->data.msg);
    ev->data.msg = NULL;
}

/* end of b3b36prg-sem/main.c */