#ifndef QUICK_UI_H
#define QUICK_UI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    MOUSE_BUTTON_CLICKED,
    MOUSE_BUTTON_HOVERED,
} qui_Mouse_Option;

typedef uint32_t qui_Id;
typedef struct { int x , y; } qui_Vec2;
typedef struct { int r , g, b, a; } qui_Color;

typedef struct { int width, height, pos_x, pos_y; } qui_Rect;
typedef struct { qui_Vec2 size, pos; } qui_RectV2;

typedef struct qui_Context {
    int width;
    int height;
    const char *text;
    bool is_running;

    qui_Id active_id;
    qui_Id hot_id;
    qui_Id keyboard_focus_id;

    qui_Vec2 mouse_pos;

    qui_Color col_box;
    qui_Color col_box_hot;
    qui_Color col_box_active;
    qui_Color col_text;

    int mouse_down;
    int mouse_pressed;
    int mouse_released;

    int key_pressed;
    int key_backspace;
    int key_enter;


    qui_Id last_id;

    float cursor_x, cursor_y;
    float spacing_x, spacing_y;

    void *userdata;

    void (*draw_rect)(struct qui_Context*, float x, float y, float w, float h, qui_Color col);
    void (*draw_text)(struct qui_Context*, const char *text, float x, float y);
    float (*text_width)(struct qui_Context*, const char *text);
    float (*text_height)(struct qui_Context*, const char *text);
} qui_Context;

void qui_init(qui_Context *qui_ctx, void *user_data);

void qui_begin(qui_Context *qui_ctx, float start_x, float start_y);
void qui_end(qui_Context *qui_ctx);

void qui_mouse_down(qui_Context *ctx, int x, int y);
void qui_mouse_up(qui_Context *ctx, int x, int y);
void qui_mouse_move(qui_Context *ctx, int x, int y);
void qui_feed_mouse_button(qui_Context *ctx, int pressed);
void qui_feed_key_backspace(qui_Context *ctx);
void qui_feed_key_enter(qui_Context *ctx);

qui_Vec2 qui_vec2(int x, int y);
qui_Rect qui_rec(int width, int height, int pos_x, int pos_y);
qui_RectV2 qui_recV2(qui_Vec2 size, qui_Vec2 pos);

void qui_draw_rect(qui_Context *ctx, qui_Rect *rec, qui_Color col);
void qui_draw_text(qui_Context *ctx, const char *text, float x, float y);

int qui_button(qui_Context *ctx, const char *label);
int qui_checkbox(qui_Context *ctx, const char *label, int *value);
int qui_slider_float(qui_Context *ctx, const char *label, float *value, float minv, float maxv, float width);
int qui_textbox(qui_Context *ctx, char *buffer, size_t cap, float width); 

#endif // QUICK_UI_H
