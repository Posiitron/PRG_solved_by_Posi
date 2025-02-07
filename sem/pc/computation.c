/*
 * Author Posiitron
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#include "computation.h"

#include "event_queue.h"
#include "utils.h"
#include <stdio.h>

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

    double zoom;
    double move_x;
    double move_y;

    int prev_mouse_x;
    int prev_mouse_y;
    int curr_mouse_x;
    int curr_mouse_y;

    int8_t n_c_var_iter;

    uint8_t chunk_n_re;
    uint8_t chunk_n_im;

    uint8_t *grid;
    bool is_set_comp;
    bool is_pc_mode;
    bool computing;
    bool abort;
    bool done;
    bool variating;
    bool c_var_right_bound;

} comp = {
    .c_re = -0.4 * 1,
    .c_im = 0.6 * 1,
    // TODO change the color of the fractol
    .n = 60 * 1,

    .range_re_min = -1.6 * 1,
    .range_re_max = 1.6 * 1,
    .range_im_min = -1.1 * 1,
    .range_im_max = 1.1 * 1,

    .grid_w = 640,
    .grid_h = 480,

    .curr_x = 0,
    .curr_y = 0,

    .d_re = 0.0 * 1,
    .d_im = 0.0 * 1,

    .nbr_chunks = 0,
    .cid = 0,
    .chunk_re = 0.0,
    .chunk_im = 0.0,

    .chunk_n_re = 64,
    .chunk_n_im = 48,

    .zoom = 1,
    .move_x = 0,
    .move_y = 0,

    .prev_mouse_x = -1,
    .prev_mouse_y = -1,
    .prev_mouse_x = -1,
    .prev_mouse_y = -1,

    .n_c_var_iter = 0,

    .grid = NULL,
    .is_set_comp = false,
    .is_pc_mode = false,
    .computing = false,
    .abort = false,
    .done = false,
    .variating = false,
    .c_var_right_bound = true,
};

void computation_init(int n, int width, int height)
{
    comp.n = n;
    comp.grid_w = width;
    comp.grid_h = height;
    comp.chunk_n_re = width / 10;
    comp.chunk_n_im = height / 10;
    comp.grid = my_alloc(comp.grid_w * comp.grid_h * sizeof(uint8_t));
    comp.d_re = (comp.range_re_max - comp.range_re_min) / (1. * comp.grid_w);
    comp.d_im = -(comp.range_im_max - comp.range_im_min) / (1. * comp.grid_h);
    comp.nbr_chunks =
        (comp.grid_h / comp.chunk_n_im) * (comp.grid_w / comp.chunk_n_re);
}
void computation_cleanup(void)
{
    if (comp.grid) {
        free(comp.grid);
    }
    comp.grid = NULL;
}

bool is_set_compute(void) { return comp.is_set_comp; }

bool is_pc_mode(void) { return comp.is_pc_mode; }

bool is_computing(void) { return comp.computing; }

bool is_done(void) { return comp.done; }

bool is_abort(void) { return comp.abort; }

bool is_variating(void) { return comp.variating; }

void set_pc_mode_state(bool state) { comp.is_pc_mode = state; }
void set_variation_state(bool state) { comp.variating = state; }

void update_comp_state(void)
{
    comp.is_set_comp = false;
    comp.computing = false;
}

void update_pan_vals(int x, int y, bool _is_prev)
{
    if (_is_prev) {
        comp.prev_mouse_x = x;
        comp.prev_mouse_y = y;
    } else {
        comp.curr_mouse_x = x;
        comp.curr_mouse_y = y;
    }
}

void reset_cid(void)
{
    comp.cid = 0;
    comp.curr_x = comp.curr_y = 0;
    comp.chunk_re = comp.range_re_min;
    comp.chunk_im = comp.range_im_max;
    comp.computing = false;
    comp.done = false;
}

void reset_pc_view(void)
{
    comp.zoom = 1;
    comp.move_x = comp.move_y = 0;
    comp.c_re = -0.4 * 1;
    comp.c_im = 0.6 * 1;
}

void variate_c_const(void)
{
    if (comp.n_c_var_iter == 15) {
        comp.n_c_var_iter = -15;
        comp.c_var_right_bound = !comp.c_var_right_bound;
    } else
        comp.n_c_var_iter++;

    if (comp.c_var_right_bound) {
        comp.c_re += C_DELTA;
        comp.c_im += C_DELTA;

    } else {
        comp.c_re -= C_DELTA;
        comp.c_im -= C_DELTA;
    }
}

bool zoom(zoom_type zoom_t)
{
    bool proceed = false;
    switch (zoom_t) {
    case ZOOM_IN:
        if (comp.zoom < MAX_ZOOM_IN)
            info("Max zoom-in reached");
        else {
            comp.zoom /= ZOOM_DELTA;
            info("➕ Zooming in");
            proceed = true;
        }
        break;
    case ZOOM_OUT:
        if (comp.zoom > 2)
            info("Max zoom-out reached");
        else {
            comp.zoom *= ZOOM_DELTA;
            info("➖ Zooming out");
            proceed = true;
        }
        break;
    default:
        error("unhandled zoom_type");
        break;
    }
    return proceed;
}

bool move(move_type direction)
{
    bool proceed = true;
    double zoom = comp.zoom;
    switch (direction) {
    case UP:
        comp.move_y += MOVE_DELTA * zoom;
        break;
    case DOWN:
        comp.move_y -= MOVE_DELTA * zoom;
        break;
    case RIGHT:
        comp.move_x += MOVE_DELTA * zoom;
        break;
    case LEFT:
        comp.move_x -= MOVE_DELTA * zoom;
        break;
    default:
        error("unhandled move_type");
        break;
    }
    return proceed;
}

void pan(void)
{
    double zoom = comp.zoom;
    int delta_x = comp.curr_mouse_x - comp.prev_mouse_x;
    int delta_y = comp.curr_mouse_y - comp.prev_mouse_y;
    comp.prev_mouse_x = comp.curr_mouse_x;
    comp.prev_mouse_y = comp.curr_mouse_y;

    if (delta_x > 0)
        comp.move_x -=
            MOVE_DELTA * zoom * abs(delta_x) * PAN_AUX_STEP / comp.grid_w;
    if (delta_x < 0)
        comp.move_x +=
            MOVE_DELTA * zoom * abs(delta_x) * PAN_AUX_STEP / comp.grid_w;

    if (delta_y > 0)
        comp.move_y +=
            MOVE_DELTA * zoom * abs(delta_y) * PAN_AUX_STEP / comp.grid_h;
    if (delta_y < 0)
        comp.move_y -=
            MOVE_DELTA * zoom * abs(delta_y) * PAN_AUX_STEP / comp.grid_h;
}

void clear_buffer(void)
{
    for (int i = 0; i < comp.grid_h * comp.grid_w; i++) {
        comp.grid[i] = 0;
    }
}

void get_grid_size(int *w, int *h)
{
    *w = comp.grid_w;
    *h = comp.grid_h;
}

void abort_comp(void)
{
    comp.abort = true;
    comp.computing = false;
}

void enable_comp(void) { comp.abort = false; }

bool set_compute(message *msg)
{
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    bool ret = !is_computing();
    if (ret) {
        msg->type = MSG_SET_COMPUTE;
        msg->data.set_compute.c_re = comp.c_re;
        msg->data.set_compute.c_im = comp.c_im;
        msg->data.set_compute.d_re = comp.d_re;
        msg->data.set_compute.d_im = comp.d_im;
        msg->data.set_compute.n = comp.n;
        comp.done = false;
        comp.is_set_comp = true;
    }
    return ret;
}
bool compute(message *msg)
{
    if (!is_computing()) { // first chunk
        comp.cid = 0;
        comp.computing = true;
        comp.curr_x = comp.curr_y = 0;
        comp.chunk_re = comp.range_re_min; // very left
        comp.chunk_im = comp.range_im_max; // upper left corner
        msg->type = MSG_COMPUTE;
    } else {
        comp.cid += 1; // nex chun
        if (comp.cid < comp.nbr_chunks) {
            comp.curr_x += comp.chunk_n_re;
            comp.chunk_re += comp.chunk_n_re * comp.d_re;
            if (comp.curr_x >= comp.grid_w) {
                comp.chunk_re = comp.range_re_min;
                comp.chunk_im += comp.chunk_n_im * comp.d_im;
                comp.curr_x = 0;
                comp.curr_y += comp.chunk_n_im;
            }
            msg->type = MSG_COMPUTE;
        }
    }

    if (comp.computing && msg->type == MSG_COMPUTE) {
        msg->data.compute.cid = comp.cid;
        msg->data.compute.re = comp.chunk_re;
        msg->data.compute.im = comp.chunk_im;
        msg->data.compute.n_re = comp.chunk_n_re;
        msg->data.compute.n_im = comp.chunk_n_im;
    }
    return is_computing();
}

void update_image(int w, int h, unsigned char *img)
{
    my_assert(img && comp.grid && w == comp.grid_w && h == comp.grid_h,
              __func__, __LINE__, __FILE__);
    for (int i = 0; i < w * h; i++) {
        const double t = 1. * comp.grid[i] / (comp.n + 1.0);
        *(img++) = 9 * (1 - t) * t * t * t * 255;
        *(img++) = 15 * (1 - t) * (1 - t) * t * t * 255;
        *(img++) = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;
    }
}

void update_data(const msg_compute_data *compute_data)
{
    my_assert(compute_data != NULL, __func__, __LINE__, __FILE__);
    if (compute_data->cid == comp.cid) {
        const int idx = comp.curr_x + compute_data->i_re +
                        (comp.curr_y + compute_data->i_im) * comp.grid_w;
        if (idx >= 0 && idx < comp.grid_w * comp.grid_h) {
            comp.grid[idx] = compute_data->iter;
        }
        if ((comp.cid + 1) >= comp.nbr_chunks &&
            (compute_data->i_re + 1) == comp.chunk_n_re &&
            (compute_data->i_im + 1) == comp.chunk_n_im) {
            comp.done = true;
            comp.computing = false;
        }
    } else {
        warn("received chunk with unexpected chunk id (cid)\n");
    }
}

bool compute_on_pc(message *msg)
{
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    double c_re = comp.c_re;
    double c_im = comp.c_im;
    int n = comp.n;
    int cid = 0;
    // Starting coords for the chunk
    double start_re = comp.range_re_min +
                      (cid % comp.chunk_n_re) * comp.chunk_n_re * comp.d_re;
    double start_im = comp.range_im_max -
                      (cid / comp.chunk_n_re) * comp.chunk_n_im * comp.d_im;

    for (int y = 0; y < comp.grid_h; y++) {
        for (int x = 0; x < comp.grid_w; x++) {
            // Coordinates of the current point in the complex plane
            double z_re = (start_re + x * comp.d_re) * comp.zoom + comp.move_x;
            double z_im = (start_im + y * comp.d_im) * comp.zoom + comp.move_y;
            int iter = 0;

            while (iter < n) {
                double z_re_new = z_re * z_re - z_im * z_im + c_re;
                double z_im_new = 2 * z_re * z_im + c_im;
                z_re = z_re_new;
                z_im = z_im_new;
                if (z_re * z_re + z_im * z_im > 4) {
                    break;
                }
                iter++;
            }

            // Determine the index in the grid array
            comp.grid[x + comp.grid_w * y] = iter;
        }
    }
    return true;
}

/* end of computation.c */