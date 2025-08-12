#include "quickui.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static qui_Id qui_gen_id(qui_Context *ctx) {
    ctx->last_id += 1;
    if (ctx->last_id == 0) ctx->last_id = 1;
    return ctx->last_id;
}

// TODO: Color specification
void qui_init(qui_Context *ctx, void *user_data) {
    if (!ctx) return;
    memset(ctx, 0, sizeof(*ctx));
    ctx->userdata = user_data;
    ctx->spacing_x = 8.0f;
    ctx->spacing_y = 8.0f;
    ctx->col_box = (qui_Color){56,56,56,255};
    ctx->col_box_hot = (qui_Color){80,80,80,255};
    ctx->col_box_active = (qui_Color){100,100,100,255};
    ctx->col_text = (qui_Color){255,255,255,255};
    ctx->last_id = 0;
}

void qui_begin(qui_Context *ctx, float start_x, float start_y) {
    ctx->cursor_x = start_x;
    ctx->cursor_y = start_y;
    ctx->last_id = 0;
    ctx->hot_id = 0;
}

void qui_end(qui_Context *ctx) {
    ctx->mouse_pressed = 0;
    ctx->mouse_released = 0;
    ctx->key_backspace = 0;
    ctx->key_enter = 0;
}

void qui_mouse_down(qui_Context *ctx, int x, int y) {
    ctx->mouse_pos.x = x;
    ctx->mouse_pos.y = y;
    ctx->mouse_down = 1;
    ctx->mouse_pressed = 1;
}

void qui_mouse_up(qui_Context *ctx, int x, int y) {
    ctx->mouse_pos.x = x;
    ctx->mouse_pos.y = y;
    ctx->mouse_down = 0;
    ctx->mouse_released = 1;
}

void qui_mouse_move(qui_Context *ctx, int x, int y) {
    ctx->mouse_pos.x = x;
    ctx->mouse_pos.y = y;
}

void qui_feed_mouse_button(qui_Context *ctx, int pressed) {
    ctx->mouse_pressed = pressed ? 1 : 0;
}

void qui_feed_key_backspace(qui_Context *ctx) {
    ctx->key_backspace = 1;
}

void qui_feed_key_enter(qui_Context *ctx) {
    ctx->key_enter = 1;
}

qui_Vec2 qui_vec2(int x, int y) { qui_Vec2 v = {x,y}; return v; }
qui_Rect qui_rec(int width, int height, int pos_x, int pos_y) { qui_Rect r = {width,height,pos_x,pos_y}; return r; }
qui_RectV2 qui_recV2(qui_Vec2 size, qui_Vec2 pos) { qui_RectV2 r = {size,pos}; return r; }

void qui_draw_rect(qui_Context *ctx, qui_Rect *rec, qui_Color col) {
    if (ctx->draw_rect) ctx->draw_rect(ctx, (float)rec->pos_x, (float)rec->pos_y, (float)rec->width, (float)rec->height, col);
}

void qui_draw_text(qui_Context *ctx, const char *text, float x, float y) {
    if (ctx->draw_text) ctx->draw_text(ctx, text, x, y);
}

static int qui_hit(qui_Context *ctx, float x, float y, float w, float h) {
    int mx = ctx->mouse_pos.x;
    int my = ctx->mouse_pos.y;
    return (mx >= (int)x && mx <= (int)(x+w) && my >= (int)y && my <= (int)(y+h));
}

//text metrics 
static float qui_text_width_fallback(qui_Context *ctx, const char *text) {
    if (ctx->text_width) return ctx->text_width(ctx, text);
    return (float)strlen(text) * 8.0f;
}

static float qui_text_height_fallback(qui_Context *ctx, const char *text) {
    if (ctx->text_height) return ctx->text_height(ctx, text);
    return 16.0f;
}

// button
int qui_button(qui_Context *ctx, const char *label) {
    qui_Id id = qui_gen_id(ctx);
    float tw = qui_text_width_fallback(ctx, label);
    float w = tw + 20.0f;
    float h = qui_text_height_fallback(ctx, label) + 8.0f;
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;

    if (qui_hit(ctx, x, y, w, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) ctx->active_id = id;
    }

    qui_Color col = ctx->col_box;
    if (ctx->active_id == id) col = ctx->col_box_active;
    else if (ctx->hot_id == id) col = ctx->col_box_hot;

    qui_Rect r = {(int)w, (int)h, (int)x, (int)y};
    qui_draw_rect(ctx, &r, col);
    qui_draw_text(ctx, label, x + 10.0f, y + 4.0f);

    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit(ctx, x, y, w, h)) clicked = 1;
        ctx->active_id = 0;
    }

    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;

    return clicked;
}

int qui_checkbox(qui_Context *ctx, const char *label, int *value) {
    qui_Id id = qui_gen_id(ctx);
    float box_size = 18.0f;
    float tw = qui_text_width_fallback(ctx, label);
    float h = box_size;
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;

    if (qui_hit(ctx, x, y, box_size + 6 + tw, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) ctx->active_id = id;
    }

    qui_Color col = ctx->col_box;
    if (ctx->active_id == id) col = ctx->col_box_active;
    else if (ctx->hot_id == id) col = ctx->col_box_hot;

    qui_Rect rbox = {(int)box_size, (int)box_size, (int)x, (int)y};
    qui_draw_rect(ctx, &rbox, col);
    if (*value) qui_draw_text(ctx, "x", x + 4.0f, y - 1.0f);

    qui_draw_text(ctx, label, x + box_size + 6.0f, y + 1.0f);

    int changed = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit(ctx, x, y, box_size + 6 + tw, h)) {
            *value = !*value;
            changed = 1;
        }
        ctx->active_id = 0;
    }

    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    return changed;
}

int qui_slider_float(qui_Context *ctx, const char *label, float *value, float minv, float maxv, float width) {
    qui_Id id = qui_gen_id(ctx);
    float label_w = qui_text_width_fallback(ctx, label);
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    float h = qui_text_height_fallback(ctx, label) + 8.0f;
    float slider_x = x + label_w + 12.0f;
    float slider_w = (width > 0.0f ? width : 160.0f);
    float slider_h = 12.0f;
    float knob_w = 10.0f;

    if (qui_hit(ctx, slider_x, y, slider_w, slider_h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) ctx->active_id = id;
    }

    qui_Color col = ctx->col_box;
    if (ctx->active_id == id) col = ctx->col_box_active;
    else if (ctx->hot_id == id) col = ctx->col_box_hot;

    qui_draw_text(ctx, label, x, y);
    qui_Rect rback = {(int)slider_w, (int)slider_h, (int)slider_x, (int)(y + 2)};
    qui_draw_rect(ctx, &rback, col);

    float t = (*value - minv) / (maxv - minv);
    if (t < 0) t = 0; if (t > 1) t = 1;
    float kx = slider_x + t * (slider_w - knob_w);

    if (ctx->active_id == id && ctx->mouse_down) {
        float local_x = (float)ctx->mouse_pos.x - slider_x;
        float nt = local_x / (slider_w - knob_w);
        if (nt < 0) nt = 0; if (nt > 1) nt = 1;
        *value = minv + nt * (maxv - minv);
    }
    if (ctx->mouse_released && ctx->active_id == id) ctx->active_id = 0;

    qui_Rect rknob = {(int)knob_w, (int)(slider_h + 4), (int)kx, (int)(y - 2)};
    qui_draw_rect(ctx, &rknob, ctx->col_box_active);

    char buf[64];
    snprintf(buf, sizeof(buf), "%.3f", *value);
    qui_draw_text(ctx, buf, slider_x + slider_w + 8.0f, y);

    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    return 1;
}

int qui_textbox(qui_Context *ctx, char *buffer, size_t cap, float width) {
    qui_Id id = qui_gen_id(ctx);
    float h = qui_text_height_fallback(ctx, "A") + 8.0f;
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    float w = (width > 0.0f ? width : 240.0f);

    if (qui_hit(ctx, x, y, w, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->keyboard_focus_id = id;
        }
    } else {
        if (ctx->mouse_pressed && ctx->keyboard_focus_id == id) {
            ctx->keyboard_focus_id = 0;
        }
    }

    qui_Color col = ctx->col_box;
    if (ctx->keyboard_focus_id == id) col = ctx->col_box_active;
    else if (ctx->hot_id == id) col = ctx->col_box_hot;

    qui_Rect r = {(int)w, (int)h, (int)x, (int)y};
    qui_draw_rect(ctx, &r, col);

    qui_draw_text(ctx, buffer, x + 6.0f, y + 4.0f);

    if (ctx->keyboard_focus_id == id) {
        float tw = qui_text_width_fallback(ctx, buffer);
        qui_draw_text(ctx, "|", x + 6.0f + tw, y + 2.0f);
    }

    if (ctx->keyboard_focus_id == id) {
        if (ctx->key_backspace) {
            size_t cur = strlen(buffer);
            if (cur > 0) buffer[cur-1] = '\0';
        }
    }

    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    return (ctx->keyboard_focus_id == id);
}
