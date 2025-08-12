// SDL_EXAMPLE.c 

#include "quickui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    SDL_Window *win;
    SDL_Renderer *ren;
    TTF_Font *font;
} Backend;

static Backend g = {0};

static void backend_draw_rect(qui_Context *ctx, float x, float y, float w, float h, qui_Color col) {
    Backend *b = (Backend*)ctx->userdata;
    if (!b->ren) return;
    SDL_Rect r = {(int)x, (int)y, (int)w, (int)h};
    SDL_SetRenderDrawColor(b->ren, col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(b->ren, &r);
}

static void backend_draw_text(qui_Context *ctx, const char *text, float x, float y) {
    Backend *b = (Backend*)ctx->userdata;
    if (!b->font) return;
    SDL_Color color = {255,255,255,255};
    SDL_Surface *s = TTF_RenderUTF8_Blended(b->font, text, color);
    if (!s) return;
    SDL_Texture *t = SDL_CreateTextureFromSurface(b->ren, s);
    SDL_Rect dst = {(int)x, (int)y, s->w, s->h};
    SDL_RenderCopy(b->ren, t, NULL, &dst);
    SDL_DestroyTexture(t);
    SDL_FreeSurface(s);
}

static float backend_text_width(qui_Context *ctx, const char *text) {
    Backend *b = (Backend*)ctx->userdata;
    if (!b->font) return (float)strlen(text) * 8.0f;
    int w,h; TTF_SizeUTF8(b->font, text, &w, &h); return (float)w;
}

static float backend_text_height(qui_Context *ctx, const char *text) {
    Backend *b = (Backend*)ctx->userdata;
    if (!b->font) return 16.0f;
    int w,h; TTF_SizeUTF8(b->font, text, &w, &h); return (float)h;
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { 
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); 
        return 1; 
    }
    if (TTF_Init() != 0) { 
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError()); 
        SDL_Quit(); 
        return 1; 
    }

    g.win = SDL_CreateWindow("SDL QUICK-UI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_SHOWN);
    g.ren = SDL_CreateRenderer(g.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    g.font = TTF_OpenFont("Iosevka-Regular.ttf", 20);
    if (!g.font) { 
        fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
		printf("Install Iosevka\n");
		exit(1);
    }

    qui_Context ui;
    qui_init(&ui, &g);

    ui.draw_rect = backend_draw_rect;
    ui.draw_text = backend_draw_text;
    ui.text_width = backend_text_width;
    ui.text_height = backend_text_height;

    int checkbox = 0;
    float slider = 0.5f;
    char buf1[256] = "Type here...";
    
    qui_Id textbox1_id = 0;

    SDL_StartTextInput();
    int running = 1;
    while (running) {
        SDL_Event e;

        ui.key_backspace = 0;
        ui.key_enter = 0;
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int mx = e.button.x, my = e.button.y;
                    qui_mouse_down(&ui, mx, my);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int mx = e.button.x, my = e.button.y;
                    qui_mouse_up(&ui, mx, my);
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                qui_mouse_move(&ui, e.motion.x, e.motion.y);
            }
            else if (e.type == SDL_TEXTINPUT) {
                if (ui.keyboard_focus_id == textbox1_id) {
                    size_t len = strlen(buf1);
                    size_t input_len = strlen(e.text.text);
                    if (len + input_len < sizeof(buf1) - 1) {
                        strcat(buf1, e.text.text);
                    }
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    ui.key_backspace = 1;
                }
                else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
                    ui.key_enter = 1;
                }
                else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(g.ren, 20, 20, 24, 255);
        SDL_RenderClear(g.ren);

        qui_begin(&ui, 12, 50);

        qui_draw_text(&ui, "QuickUI SDL Demo", 12, 12);

        if (qui_button(&ui, "Press me")) {
            printf("Button clicked\n");
        }
        
        if (qui_checkbox(&ui, "Check me", &checkbox)) {
            printf("Checkbox changed -> %d\n", checkbox);
        }

        qui_slider_float(&ui, "Volume", &slider, 0.0f, 1.0f, 240.0f);

        qui_Id current_id = ui.last_id + 1;
        qui_textbox(&ui, buf1, sizeof(buf1), 360.0f);
        if (textbox1_id == 0) textbox1_id = current_id;

        char show[512];
        snprintf(show, sizeof(show), "Text content: \"%s\"", buf1);
        qui_draw_text(&ui, show, 12, ui.cursor_y + 8);
        
        char debug[256];
        snprintf(debug, sizeof(debug), "Focus ID: %u (textbox: %u)", 
                ui.keyboard_focus_id, textbox1_id);
        qui_draw_text(&ui, debug, 12, ui.cursor_y + 30);

        qui_end(&ui);

        SDL_RenderPresent(g.ren);
        SDL_Delay(16);
    }

    SDL_StopTextInput();

    if (g.font) TTF_CloseFont(g.font);
    if (g.ren) SDL_DestroyRenderer(g.ren);
    if (g.win) SDL_DestroyWindow(g.win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
