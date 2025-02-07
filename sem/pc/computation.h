#ifndef __COMPUTATION_H__
#define __COMPUTATION_H__

#include <stdbool.h>

#include "event_queue.h"
#include "messages.h"

#define MAX_ZOOM_IN 5e-14 // 5 * 10^-14
#define ZOOM_DELTA 2
#define C_DELTA 0.1
#define MOVE_DELTA 0.5
#define PAN_AUX_STEP 5

void computation_init(int n, int width, int height);
void computation_cleanup(void);

void get_grid_size(int *w, int *h);
bool is_computing(void);
bool is_done(void);
bool is_abort(void);

void reset_cid(void);
void reset_pc_view(void);
void clear_buffer(void);
void update_comp_state(void);
void update_pan_vals(int x, int y, bool _is_prev);

void abort_comp(void);
void enable_comp(void);

bool zoom(zoom_type zoom_t);
bool move(move_type direction);
void pan(void);
void variate_c_const(void);

bool is_set_compute(void);
bool is_pc_mode(void);
bool is_computing(void);
bool is_done(void);
bool is_abort(void);
bool is_variating(void);

bool set_compute(message *msg);
bool compute(message *msg);

void set_pc_mode_state(bool state);
void set_variation_state(bool state);
void update_image(int w, int h, unsigned char *img);
void update_data(const msg_compute_data *compute_data);
bool compute_on_pc(message *msg);

#endif

/* end of computation.h */