/*
 * Author Amir Akrami
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#include "computation.h"

#include "utils.h"
#include <stdio.h>
#include <unistd.h>

static struct {
    double c_re;
    double c_im;
    int n;

    double range_re_min;
    double range_re_max;
    double range_im_min;
    double range_im_max;

    int grid_w;
    int grid_h;

    int curr_x;
    int curr_y;

    double d_re;
    double d_im;

    int nbr_chunks;
    int cid;
    double chunk_re;
    double chunk_im;

    uint8_t chunk_n_re;
    uint8_t chunk_n_im;

    uint8_t *grid;
    bool computing;
    bool abort;
    bool done;

} comp = {

    .c_re = -0.4 * 1,
    .c_im = 0.6 * 1,
    .n = 60 * 1,

    .range_re_min = -1.6 * 1,
    .range_re_max = 1.6 * 1,
    .range_im_min = -1.1 * 1,
    .range_im_max = 1.1 * 1,

    .grid_w = 640,
    .grid_h = 480,

    .curr_x = 0,
    .curr_y = 0,

    .d_re = 0.0,
    .d_im = 0.0,

    .nbr_chunks = 0,
    .cid = 0,
    .chunk_re = 0.0,
    .chunk_im = 0.0,

    .chunk_n_re = 64,
    .chunk_n_im = 48,

    .grid = NULL,
    .computing = false,
    .abort = false,
    .done = false};

bool is_computing(void) { return comp.computing; }

bool is_done(void) { return comp.done; }

bool is_abort(void) { return comp.abort; }

void abort_comp(void) { comp.abort = true; }

void enable_comp(void) { comp.abort = false; }

bool set_compute(const msg_set_compute *msg)
{
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    bool ret = !is_computing();
    if (ret) {
        comp.c_re = msg->c_re;
        comp.c_im = msg->c_im;
        comp.d_re = msg->d_re;
        comp.d_im = msg->d_im;
        comp.n = msg->n;
        comp.done = false;
    }
    return ret;
}

void last_chunk(message *msg) { msg->data.compute_data.cid = comp.cid; }

void update_data(const msg_compute *compute_data)
{
    my_assert(compute_data != NULL, __func__, __LINE__, __FILE__);
    comp.cid = compute_data->cid;
    comp.chunk_n_re = compute_data->n_re;
    comp.chunk_n_im = compute_data->n_im;
    comp.chunk_re = compute_data->re;
    comp.chunk_im = compute_data->im;
}

int c_pixel(int x, int y, int cid)
{
    double new_re, new_im, old_re, old_im;

    old_re = comp.chunk_re + comp.d_re * x;
    old_im = comp.chunk_im + comp.d_im * y;
    int iteration = 0;
    while (iteration < comp.n) {
        new_re = old_re * old_re - old_im * old_im + comp.c_re;
        new_im = 2 * old_re * old_im + comp.c_im;
        old_re = new_re;
        old_im = new_im;
        // magnitude of the complex number greater than sqrt4 = 2
        if ((old_re * old_re + old_im * old_im) > 4) {
            break;
        }
        iteration++;
    }
    return iteration;
}

bool compute(message *msg)
{
    int iteration;
    if (comp.abort) return false;
    if (!comp.computing) {
        comp.curr_x = comp.chunk_re;
        comp.curr_y = comp.chunk_im;
        comp.computing = true;
    }
    // Calculate a single pixel
    iteration = c_pixel(comp.curr_x, comp.curr_y, comp.cid);
    // Message data for the current pixel
    msg->data.compute_data.iter = iteration;
    msg->data.compute_data.cid = comp.cid;
    msg->data.compute_data.i_re = comp.curr_x;
    msg->data.compute_data.i_im = comp.curr_y;
    // Next pixel
    comp.curr_y++;
    if (comp.curr_y > comp.chunk_n_im) {
        comp.curr_y = comp.chunk_im;
        comp.curr_x++;
        if (comp.curr_x > comp.chunk_n_re) {
            comp.done = true;
            comp.computing = false;
            return false; // No more pixels left to compute in this chunk
        }
    }

    return comp.computing;
}

/* end of computation.c */