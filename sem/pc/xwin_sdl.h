#include <SDL2/SDL_ttf.h>
#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

int xwin_init(int w, int h);
void xwin_close();
void xwin_redraw(int w, int h, unsigned char *img);
void xwin_poll_events(void);
void save_to_jpg();
void save_to_png();
void render_text(const char *text, int x, int y, TTF_Font *font,
                 SDL_Color color);

#endif

/* end of xwin_sdl.h */
