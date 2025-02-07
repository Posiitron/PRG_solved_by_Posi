/*
 * Author Posiitron
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#include "xwin_sdl.h"
#include <SDL2/SDL.h>

#include "computation.h"
#include "utils.h"

#include "event_queue.h"
#include "gui.h"

#ifndef SDL_EVENT_POLL_WAIT_MS
#define SDL_EVENT_POLL_WAIT_MS 1
#endif

static struct {
    int w;
    int h;
    unsigned char *img;
} gui = {.img = NULL};

void gui_init(void)
{
    get_grid_size(&gui.w, &gui.h);
    gui.img = my_alloc(gui.w * gui.h * 3);
    my_assert(xwin_init(gui.w, gui.h) == 0, __func__, __LINE__, __FILE__);
}

void gui_cleanup(void)
{
    if (gui.img) {
        free(gui.img);
        gui.img = NULL;
    }
    xwin_close();
}

void gui_refresh(void)
{
    if (gui.img) {
        update_image(gui.w, gui.h, gui.img);
        xwin_redraw(gui.w, gui.h, gui.img);
    }
}

void *gui_win_thread(void *d)
{
    debug("gui_win_thread - start");
    bool quit = false;
    SDL_Event event_sdl;
    event ev;

    while (!quit) {
        ev.type = EV_TYPE_NUM;
        if (SDL_PollEvent(&event_sdl)) {
            if (event_sdl.type == SDL_KEYDOWN) {
                switch (event_sdl.key.keysym.sym) {
                case SDLK_q:
                    ev.type = EV_QUIT;
                    // custom queue push
                    // queue_push(ev);
                    break;
                case SDLK_s:
                    ev.type = EV_SET_COMPUTE;
                    break;
                case SDLK_a:
                    if (!is_pc_mode()) ev.type = EV_ABORT;
                    break;
                case SDLK_1:
                    if (is_computing())
                        info("Computation is already in progress!");
                    else if (is_pc_mode())
                        info("You are currently in 💻 PC mode, 👉 to enter "
                             "COMPUTE "
                             "mode please press 'l'");
                    else
                        ev.type = EV_COMPUTE;
                    break;
                case SDLK_c:
                    if (!is_computing())
                        ev.type = EV_COMPUTE_PC;
                    else
                        info("Computation is already in process!");
                    break;
                case SDLK_UP:
                    if (is_pc_mode()) {
                        ev.type = EV_MOVE;
                        ev.data.param = UP;
                    }
                    break;
                case SDLK_DOWN:
                    if (is_pc_mode()) {
                        ev.type = EV_MOVE;
                        ev.data.param = DOWN;
                    }
                    break;
                case SDLK_LEFT:
                    if (is_pc_mode()) {
                        ev.type = EV_MOVE;
                        ev.data.param = LEFT;
                    }
                    break;
                case SDLK_RIGHT:
                    if (is_pc_mode()) {
                        ev.type = EV_MOVE;
                        ev.data.param = RIGHT;
                    }
                    break;
                case SDLK_p:
                    ev.type = EV_PNG;
                    break;
                case SDLK_j:
                    ev.type = EV_JPG;
                    break;
                case SDLK_g:
                    ev.type = EV_GET_VERSION;
                    break;
                case SDLK_r:
                    if (!is_variating()) ev.type = EV_REFRESH;
                    break;
                case SDLK_l:
                    if (!is_variating()) ev.type = EV_CLEAR_BUFFER;
                    break;
                case SDLK_x:
                    if (is_pc_mode()) {
                        if (is_variating()) {
                            set_variation_state(false);
                            info("Stopping C variation...");
                        } else {
                            ev.type = EV_VARIATE_C;
                            set_variation_state(true);
                            info("C variation started");
                        }
                    }
                    break;
                case SDLK_h:
                    ev.type = EV_HELP;
                    break;
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    if (is_pc_mode()) {
                        ev.type = EV_ZOOM;
                        ev.data.param = ZOOM_IN;
                    }
                    break;
                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    if (is_pc_mode()) {
                        ev.type = EV_ZOOM;
                        ev.data.param = ZOOM_OUT;
                    }
                    break;
                }
            } else if (event_sdl.type == SDL_KEYUP) {
                // debug("gui_win_thread - keyup");

            } else if (event_sdl.type == SDL_MOUSEBUTTONDOWN && is_pc_mode()) {
                if (event_sdl.button.button == SDL_BUTTON_LEFT) {
                    update_pan_vals(event_sdl.button.x, event_sdl.button.y,
                                    true);
                }
            } else if (event_sdl.type == SDL_MOUSEBUTTONUP && is_pc_mode()) {
                if (event_sdl.button.button == SDL_BUTTON_LEFT) {
                    update_pan_vals(event_sdl.button.x, event_sdl.button.y,
                                    false);
                    ev.type = EV_PAN;
                }
            } else if (event_sdl.type == SDL_MOUSEWHEEL && is_pc_mode()) {
                if (event_sdl.wheel.y > 0) {
                    if (!is_computing() || is_abort()) {
                        ev.type = EV_ZOOM;
                        ev.data.param = ZOOM_IN;
                    }
                } else if (event_sdl.wheel.y < 0) {
                    if (!is_computing() || is_abort()) {
                        ev.type = EV_ZOOM;
                        ev.data.param = ZOOM_OUT;
                    }
                }
            } else if (event_sdl.type == SDL_WINDOWEVENT) {
                if (event_sdl.window.event == SDL_WINDOWEVENT_CLOSE)
                    ev.type = EV_QUIT;
            }
        }
        if (ev.type != EV_TYPE_NUM) {
            queue_push(ev);
        }
        SDL_Delay(SDL_EVENT_POLL_WAIT_MS);
        quit = is_quit();
    }
    set_quit();
    debug("gui_win_thread - end");
    return NULL;
}

/* end of gui.c */