/* Copyright (c) 2025 csode
 * Licensed under the GPL3 License. See LICENSE file for details.
 */

#ifndef QUICK_UI_H
#define QUICK_UI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint32_t qui_Id;
typedef struct { int x , y; } qui_vec2_t;
typedef struct { int r , g, b, a; } qui_Color;

typedef struct { void *data;  int width, height, channels; } qui_Image;
typedef struct { int width, height, pos_x, pos_y; } qui_Rect;
typedef struct { qui_vec2_t size, pos; } qui_RectV2;

typedef struct qui_Context {
    int width;
    int height;
    const char *text;
    bool is_running;

    qui_Id active_id;
    qui_Id hot_id;
    qui_Id keyboard_focus_id;

    qui_vec2_t mouse_pos;

    qui_Color col_box;
    qui_Color col_box_hot;
    qui_Color col_box_active;
    qui_Color col_text;

    bool popup_open;
    qui_vec2_t popup_pos;
    qui_vec2_t popup_size;

    int mouse_down;
    int mouse_pressed;
    int mouse_released;

    int key_pressed;
    int key_backspace;
    int key_enter;

    // Drag support
    float drag_offset_x, drag_offset_y;

    qui_Id last_id;
    float cursor_x, cursor_y;
    float spacing_x, spacing_y;
    
    void *font;
    float font_size;
    float font_spacing;

    float layout_offset_x, layout_offset_y;
    float saved_cursor_x, saved_cursor_y;
    float saved_offset_x, saved_offset_y;

    void *userdata;

    void (*draw_rect)(struct qui_Context*, float x, float y, float w, float h, qui_Color col);
    void (*draw_text)(struct qui_Context*, const char *text, float x, float y);
    void (*draw_image)(struct qui_Context*, qui_Image *img, float x, float y, float w, float h);
    float (*text_width)(struct qui_Context*, const char *text);
    float (*text_height)(struct qui_Context*, const char *text);
} qui_Context;


void qui_init(qui_Context *qui_ctx, void *user_data);
void qui_begin(qui_Context *qui_ctx, float start_x, float start_y);
void qui_end(qui_Context *qui_ctx);
void qui_set_font(qui_Context *qui_ctx, void *font, float font_size, float font_spacing);

bool qui_begin_window(qui_Context *ctx, const char *title, qui_vec2_t size, qui_vec2_t *pos);
void qui_end_window(qui_Context *ctx);

void qui_mouse_down(qui_Context *ctx, int x, int y);
void qui_mouse_up(qui_Context *ctx, int x, int y);
void qui_mouse_move(qui_Context *ctx, int x, int y);
void qui_feed_mouse_button(qui_Context *ctx, int pressed);
void qui_feed_key_backspace(qui_Context *ctx);
void qui_feed_key_enter(qui_Context *ctx);

qui_vec2_t qui_vec2(int x, int y);
qui_Rect qui_rec(int width, int height, int pos_x, int pos_y);
qui_RectV2 qui_recV2(qui_vec2_t size, qui_vec2_t pos);

void qui_draw_rect(qui_Context *ctx, qui_Rect *rec, qui_Color col);
void qui_draw_text(qui_Context *ctx, const char *text, float x, float y);

int qui_button(qui_Context *ctx, const char *label);
int qui_checkbox(qui_Context *ctx, const char *label, int *value);
int qui_slider(qui_Context *ctx, const char *label, float *value, float minv, float maxv, float width);
int qui_textbox(qui_Context *ctx, char *buffer, size_t cap, float width);

void qui_draw_image(qui_Context *ctx, qui_Image *image, float x, float y, float w, float h);
int qui_image_button(qui_Context *ctx, qui_Image *image, float button_width, float button_height, float img_width, float img_height);
int qui_image_button_with_label(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height);
int qui_image_button_vertical(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height);

#endif // QUICK_UI_H
