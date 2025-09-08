#include "quickui.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Proper Error Handeling and logging

// implemntaed Colors
static qui_Color fg     = { 56  ,56  ,56  ,255 };
static qui_Color bg     = { 0   ,0   ,0   ,255 };
static qui_Color blue   = { 0   ,0   ,255 ,255 };
static qui_Color hot    = { 80  ,80  ,80  ,255 };
static qui_Color active = { 100 ,100 ,100 ,255 };
static qui_Color text   = { 255 ,255 ,255 ,255 };

static qui_Id qui_gen_id(qui_Context *ctx) {
    ctx->last_id += 1;
    if (ctx->last_id == 0) ctx->last_id = 1;
    return ctx->last_id;
}

void qui_init(qui_Context *ctx, void *user_data) {
    if (!ctx) return;
    memset(ctx, 0, sizeof(*ctx));
    ctx->userdata = user_data;
    ctx->spacing_x = 8.0f;
    ctx->spacing_y = 8.0f;
    ctx->col_box = fg;
    ctx->col_box_hot = hot;
    ctx->col_box_active = active;
    ctx->col_text = text;
    ctx->last_id = 0;
    ctx->font = NULL;
    ctx->font_size = 0;
    ctx->font_spacing = 0;
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

// Font Garbage
void qui_set_font(qui_Context *ctx, void *font, float font_size, float font_spacing) {
    ctx->font = font;
    ctx->font_size = font_size;
    ctx->font_spacing = font_spacing;
}

qui_vec2_t qui_vec2(int x, int y) { qui_vec2_t v = {x,y}; return v; }
qui_Rect qui_rec(int width, int height, int pos_x, int pos_y) { qui_Rect r = {width,height,pos_x,pos_y}; return r; }
qui_RectV2 qui_recV2(qui_vec2_t size, qui_vec2_t pos) { qui_RectV2 r = {size,pos}; return r; }

void qui_draw_rect(qui_Context *ctx, qui_Rect *rec, qui_Color col) {
    if (ctx->draw_rect) {
        float ox = ctx->layout_offset_x;
        float oy = ctx->layout_offset_y;
        ctx->draw_rect(ctx,
        rec->pos_x + ox,
        rec->pos_y + oy,
        rec->width,
        rec->height,
        col
        );
    }
}

void qui_draw_text(qui_Context *ctx, const char *text, float x, float y) {
    if (ctx->draw_text) {
        float ox = ctx->layout_offset_x;
        float oy = ctx->layout_offset_y;
        ctx->draw_text(ctx, text, x + ox, y + oy);
    }
}

// Hit detection for absolute coordinates (window controls)
static int qui_hit_absolute(qui_Context *ctx, float x, float y, float w, float h) {
    int mx = ctx->mouse_pos.x;
    int my = ctx->mouse_pos.y;
    return (mx >= x && mx <= x + w &&
    my >= y && my <= y + h);
}

// hit detection with layout offset
static int qui_hit(qui_Context *ctx, float x, float y, float w, float h) {
    float ox = ctx->layout_offset_x;
    float oy = ctx->layout_offset_y;
    int mx = ctx->mouse_pos.x;
    int my = ctx->mouse_pos.y;
    return (mx >= x + ox && mx <= x + ox + w &&
    my >= y + oy && my <= y + oy + h);
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

int qui_slider(qui_Context *ctx, const char *label, float *value, float minv, float maxv, float width) {
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
        // Fix: Apply layout offset to mouse coordinate calculation
        float local_x = (float)ctx->mouse_pos.x - (slider_x + ctx->layout_offset_x);
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

bool qui_begin_window(qui_Context *ctx, const char *title, qui_vec2_t size, qui_vec2_t *pos) {
    
    qui_Id window_id = qui_gen_id(ctx);
    
    float x = (float)pos->x;
    float y = (float)pos->y;
    float w = (float)size.x;
    float h = (float)size.y;
    float title_height = qui_text_height_fallback(ctx, title ? title : "") + 8.0f;

    if (ctx->active_id == 0 || ctx->active_id == window_id) {
        bool is_title_bar_hit = qui_hit_absolute(ctx, x, y, w - 4, title_height);
        
        if (is_title_bar_hit) {
            ctx->hot_id = window_id;
            if (ctx->mouse_pressed) {
                ctx->active_id = window_id;
                ctx->drag_offset_x = ctx->mouse_pos.x - x;
                ctx->drag_offset_y = ctx->mouse_pos.y - y;
            }
        }
    }
    
    if (ctx->active_id == window_id && ctx->mouse_down) {
        pos->x = ctx->mouse_pos.x - ctx->drag_offset_x;
        pos->y = ctx->mouse_pos.y - ctx->drag_offset_y;
        x = (float)pos->x;
        y = (float)pos->y;
    }

    qui_draw_rect(ctx, &(qui_Rect){(int)w, (int)h, (int)x, (int)y}, bg);
    
    qui_draw_rect(ctx, &(qui_Rect){(int)w, (int)title_height, (int)x, (int)y}, blue);
    
    if (title) {
        qui_draw_text(ctx, title, x + 8.0f, y + 4.0f);
    }

    if (ctx->mouse_released && ctx->active_id == window_id) {
        ctx->active_id = 0;
    }
    
    ctx->saved_cursor_x = ctx->cursor_x;
    ctx->saved_cursor_y = ctx->cursor_y;
    ctx->saved_offset_x = ctx->layout_offset_x;
    ctx->saved_offset_y = ctx->layout_offset_y;

    ctx->layout_offset_x = x + 10.0f;
    ctx->layout_offset_y = y + title_height + 10.0f;

    ctx->cursor_x = 0;
    ctx->cursor_y = 0;

    return true;
}

void qui_end_window(qui_Context *ctx) {
    ctx->cursor_x = ctx->saved_cursor_x;
    ctx->cursor_y = ctx->saved_cursor_y;
    ctx->layout_offset_x = ctx->saved_offset_x;
    ctx->layout_offset_y = ctx->saved_offset_y;
}

int qui_popup(qui_Context *ctx, char *buffer, size_t cap) {
    if (!ctx->popup_open) return 0;

    qui_vec2_t center = qui_vec2(ctx->width / 2, ctx->height / 2);
    float popup_width = 300.0f;
    float popup_height = 100.0f;

    float x = center.x - popup_width / 2;
    float y = center.y - popup_height / 2;

    qui_draw_rect(ctx, &(qui_Rect){(int)popup_width, (int)popup_height, (int)x, (int)y}, ctx->col_box);

    float prev_off_x = ctx->layout_offset_x;
    float prev_off_y = ctx->layout_offset_y;
    ctx->layout_offset_x = x + 10.0f;
    ctx->layout_offset_y = y + 10.0f;

    ctx->cursor_x = 0;
    ctx->cursor_y = 0;

    qui_textbox(ctx, buffer, cap, popup_width - 20);

    ctx->layout_offset_x = prev_off_x;
    ctx->layout_offset_y = prev_off_y;

    return 1;
}

void qui_draw_image(qui_Context *ctx, qui_Image *image, float x, float y, float w, float h) {
    if (ctx->draw_image && image) {
        float ox = ctx->layout_offset_x;
        float oy = ctx->layout_offset_y;
        ctx->draw_image(ctx, image, x + ox, y + oy, w, h);
    }
}

int qui_image_button(qui_Context *ctx, qui_Image *image, float button_width, float button_height, float img_width, float img_height) {
    if (!image) return 0;
    
    qui_Id id = qui_gen_id(ctx);
    
    // Button dimensions - use parameters or defaults
    float w = button_width > 0 ? button_width : (float)image->width + 16.0f;  // Add padding if no size specified
    float h = button_height > 0 ? button_height : (float)image->height + 16.0f;
    
    // Image dimensions - use parameters or natural size
    float img_w = img_width > 0 ? img_width : (float)image->width;
    float img_h = img_height > 0 ? img_height : (float)image->height;
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;

    if (qui_hit(ctx, x, y, w, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) ctx->active_id = id;
    }

    // Calculate centered image position within the button
    float centered_x = x + (w - img_w) / 2;
    float centered_y = y + (h - img_h) / 2;

    if (ctx->active_id == id) {
        qui_Rect bg_rect = {(int)w, (int)h, (int)x, (int)y};
        qui_draw_rect(ctx, &bg_rect, ctx->col_box_active);
        // Apply press offset to centered position
        qui_draw_image(ctx, image, centered_x + 1, centered_y + 1, img_w, img_h);
    } else if (ctx->hot_id == id) {
        qui_Rect bg_rect = {(int)w, (int)h, (int)x, (int)y};
        qui_draw_rect(ctx, &bg_rect, ctx->col_box_hot);
        // Use centered position
        qui_draw_image(ctx, image, centered_x, centered_y, img_w, img_h);
    } else {
        // Normal state - use centered position
        qui_draw_image(ctx, image, centered_x, centered_y, img_w, img_h);
    }

    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit(ctx, x, y, w, h)) clicked = 1;
        ctx->active_id = 0;
    }

    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;

    return clicked;
}

int qui_image_button_with_label(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height) {
    if (!image) return qui_button(ctx, label);
    
    qui_Id id = qui_gen_id(ctx);
    float iw = img_width > 0 ? img_width : (float)image->width;
    float ih = img_height > 0 ? img_height : (float)image->height;
    float tw = qui_text_width_fallback(ctx, label);
    float th = qui_text_height_fallback(ctx, label);
    
    float padding = 8.0f;
    float spacing = 6.0f;
    float total_w = iw + spacing + tw + (padding * 2);
    float total_h = (ih > th ? ih : th) + (padding * 2);
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;

    if (qui_hit(ctx, x, y, total_w, total_h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) ctx->active_id = id;
    }

    qui_Color col = ctx->col_box;
    if (ctx->active_id == id) col = ctx->col_box_active;
    else if (ctx->hot_id == id) col = ctx->col_box_hot;

    qui_Rect bg_rect = {(int)total_w, (int)total_h, (int)x, (int)y};
    qui_draw_rect(ctx, &bg_rect, col);

    float img_x = x + padding;
    float img_y = y + (total_h - ih) / 2;
    float text_x = img_x + iw + spacing;
    float text_y = y + (total_h - th) / 2;

    if (ctx->active_id == id) {
        img_x += 1;
        img_y += 1;
        text_x += 1;
        text_y += 1;
    }

    qui_draw_image(ctx, image, img_x, img_y, iw, ih);
    qui_draw_text(ctx, label, text_x, text_y);

    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit(ctx, x, y, total_w, total_h)) clicked = 1;
        ctx->active_id = 0;
    }

    ctx->cursor_y += total_h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;

    return clicked;
}

int qui_image_button_vertical(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height) {
    if (!image) return qui_button(ctx, label);
    
    qui_Id id = qui_gen_id(ctx);
    float iw = img_width > 0 ? img_width : (float)image->width;
    float ih = img_height > 0 ? img_height : (float)image->height;
    float tw = qui_text_width_fallback(ctx, label);
    float th = qui_text_height_fallback(ctx, label);
    
    float padding = 8.0f;
    float spacing = 4.0f;
    float total_w = (iw > tw ? iw : tw) + (padding * 2);
    float total_h = ih + spacing + th + (padding * 2);
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;

    if (qui_hit(ctx, x, y, total_w, total_h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) ctx->active_id = id;
    }

    qui_Color col = ctx->col_box;
    if (ctx->active_id == id) col = ctx->col_box_active;
    else if (ctx->hot_id == id) col = ctx->col_box_hot;

    qui_Rect bg_rect = {(int)total_w, (int)total_h, (int)x, (int)y};
    qui_draw_rect(ctx, &bg_rect, col);

    // Center image and text
    float img_x = x + (total_w - iw) / 2;
    float img_y = y + padding;
    float text_x = x + (total_w - tw) / 2;
    float text_y = img_y + ih + spacing;

    if (ctx->active_id == id) {
        img_x += 1;
        img_y += 1;
        text_x += 1;
        text_y += 1;
    }

    qui_draw_image(ctx, image, img_x, img_y, iw, ih);
    qui_draw_text(ctx, label, text_x, text_y);

    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit(ctx, x, y, total_w, total_h)) clicked = 1;
        ctx->active_id = 0;
    }

    ctx->cursor_y += total_h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;

    return clicked;
}
