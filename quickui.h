/* Copyright (c) 2025 csode
 * Licensed under the GPL3 License. See LICENSE file for details.
 */

/**
 * @file quickui.h
 * @brief QuickUI - A simple immediate mode GUI library for C
 * 
 * QuickUI is a lightweight immediate mode GUI library designed for quick prototyping
 * and simple applications. It provides basic UI elements like buttons, checkboxes,
 * sliders, and text boxes with customizable rendering backends.
 * 
 * Key Features:
 * - Immediate mode rendering
 * - Backend-agnostic (works with any renderer)
 * - Window management with drag support
 * - Customizable colors and fonts
 * - Image button support
 * - Error checking and validation
 * 
 * Usage:
 * 1. Initialize context with qui_init()
 * 2. Set up rendering callbacks
 * 3. Handle input events
 * 4. Call qui_begin() at start of frame
 * 5. Add UI elements
 * 6. Call qui_end() at end of frame
 */

#if 0
#include "quickui.h"
#include "raylib.h"

void raylib_draw_rect(qui_Context *ctx, float x, float y, float w, float h, qui_Color col) {
    DrawRectangle((int)x, (int)y, (int)w, (int)h, (Color){col.r, col.g, col.b, col.a});
}

void raylib_draw_text(qui_Context *ctx, const char *text, float x, float y) {
    DrawText(text, (int)x, (int)y, 20, WHITE);
}

float raylib_text_width(qui_Context *ctx, const char *text) {
    return (float)MeasureText(text, 20);
}

float raylib_text_height(qui_Context *ctx, const char *text) {
    return 20.0f;
}

int main() {
    InitWindow(800, 600, "QuickUI Demo");
    
    qui_Context ui;
    qui_Result result = qui_init(&ui, NULL);
    if (result != QUI_OK) {
        printf("Failed to initialize QuickUI: %s\n", qui_get_error_string(result));
        return -1;
    }
    
    ui.draw_rect = raylib_draw_rect;
    ui.draw_text = raylib_draw_text;
    ui.text_width = raylib_text_width;
    ui.text_height = raylib_text_height;
    
    int checkbox_val = 0;
    float slider_val = 50.0f;
    
   while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            qui_mouse_down(&ui, GetMouseX(), GetMouseY());
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            qui_mouse_up(&ui, GetMouseX(), GetMouseY());
        qui_mouse_move(&ui, GetMouseX(), GetMouseY());
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        qui_begin(&ui, 20, 20);
        qui_button(&ui, "Hello World!");
        qui_checkbox(&ui, "Enable Feature", &checkbox_val);
        qui_slider(&ui, "Value", &slider_val, 0.0f, 100.0f, 200.0f);
        qui_end(&ui);
        
        EndDrawing();
    }
    
    qui_cleanup(&ui);
    CloseWindow();
    return 0;
}
#endif

#ifndef QUICK_UI_H
#define QUICK_UI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================================================
 * CORE TYPES AND CONSTANTS
 * ================================================================================================ */

/** @brief Unique identifier for UI elements */
typedef uint32_t qui_Id;

/** @brief Result codes for QuickUI operations */
typedef enum {
    QUI_OK = 0,                /**< Operation successful */
    QUI_ERROR_NULL_POINTER,    /**< Null pointer passed */
    QUI_ERROR_INVALID_VALUE,   /**< Invalid parameter value */
    QUI_ERROR_NOT_INITIALIZED, /**< Context not initialized */
    QUI_ERROR_BUFFER_TOO_SMALL,/**< Buffer capacity too small */
    QUI_ERROR_INVALID_STATE    /**< Invalid operation for current state */
} qui_Result;

/** @brief 2D integer vector */
typedef struct {
    int x;
    int y;
}qui_Vec2;

/** @brief RGBA color value (0-255) */
typedef struct {
    int r;
    int g;
    int b;
    int a;
} qui_Color;

/** @brief Image data structure */
typedef struct {
    void *data;    /**< Pixel data */
    int width;     /**< Image width in pixels */
    int height;    /**< Image height in pixels */
    int channels;  /**< Number of color channels (1-4) */
} qui_Image;

/** @brief Rectangle definition */
typedef struct {
    int width;  /**< Rectangle width */
    int height; /**< Rectangle height */
    int pos_x;  /**< X position */
    int pos_y;  /**< Y position */
} qui_Rect;

/** @brief Default color scheme */
typedef enum {
    QUI_COLOR_BACKGROUND = 0, /**< Default background color */
    QUI_COLOR_FOREGROUND,     /**< Default foreground/box color */
    QUI_COLOR_HOT,            /**< Color when mouse hovers */
    QUI_COLOR_ACTIVE,         /**< Color when element is active */
    QUI_COLOR_TEXT,           /**< Text color */
    QUI_COLOR_WINDOW_BG,      /**< Window background color */
    QUI_COLOR_TITLE_BAR,      /**< Window title bar color */
    QUI_COLOR_COUNT           /**< Number of color types */
} qui_ColorType;

/* ================================================================================================
 * MAIN CONTEXT STRUCTURE
 * ================================================================================================ */

/** @brief Main QuickUI context structure */
typedef struct qui_Context {
    /* Display properties */
    int width;                /**< Context width */
    int height;               /**< Context height */
    bool is_running;          /**< Runtime state flag */

    /* UI state management */
    qui_Id active_id;         /**< Currently active element ID */
    qui_Id hot_id;           /**< Element under mouse cursor */
    qui_Id keyboard_focus_id; /**< Element with keyboard focus */
    qui_Id last_id;          /**< Last generated ID */

    /* Input state */
    qui_Vec2 mouse_pos;      /**< Current mouse position */
    int mouse_down;          /**< Mouse button down state */
    int mouse_pressed;       /**< Mouse button just pressed */
    int mouse_released;      /**< Mouse button just released */
    int key_pressed;         /**< Key pressed state */
    int key_backspace;       /**< Backspace key pressed */
    int key_enter;           /**< Enter key pressed */

    /* Color scheme */
    qui_Color colors[QUI_COLOR_COUNT]; /**< Default color palette */

    /* Layout state */
    float cursor_x;          /**< Current layout cursor X */
    float cursor_y;          /**< Current layout cursor Y */
    float spacing_x;         /**< Horizontal spacing between elements */
    float spacing_y;         /**< Vertical spacing between elements */
    float layout_offset_x;   /**< Layout X offset (for windows) */
    float layout_offset_y;   /**< Layout Y offset (for windows) */
    
    /* Saved layout state (for nested contexts) */
    float saved_cursor_x;    /**< Saved cursor X position */
    float saved_cursor_y;    /**< Saved cursor Y position */
    float saved_offset_x;    /**< Saved offset X position */
    float saved_offset_y;    /**< Saved offset Y position */

    /* Drag support */
    float drag_offset_x;     /**< Drag X offset */
    float drag_offset_y;     /**< Drag Y offset */

    /* Font properties */
    void *font;              /**< Font handle (backend-specific) */
    float font_size;         /**< Font size */
    float font_spacing;      /**< Font character spacing */

    /* Window/popup state */
    bool popup_open;         /**< Popup window open state */
    qui_Vec2 popup_pos;      /**< Popup position */
    qui_Vec2 popup_size;     /**< Popup size */

    /* User data */
    void *userdata;          /**< User-defined data pointer */

    /* Rendering callbacks (must be set by user) */
    
    void (*draw_rect)(struct qui_Context* ctx, float x, float y, float w, float h, qui_Color col);
    
    /** @brief Draw text at position
     * @param ctx QuickUI context
     * @param text Text string to draw
     * @param x X position
     * @param y Y position
     */
    void (*draw_text)(struct qui_Context* ctx, const char *text, float x, float y);
    
    /** @brief Get text width in pixels
     * @param ctx QuickUI context
     * @param text Text string to measure
     * @return Text width in pixels
     */
    float (*text_width)(struct qui_Context* ctx, const char *text);
    
    /** @brief Get text height in pixels
     * @param ctx QuickUI context
     * @param text Text string to measure
     * @return Text height in pixels
     */
    float (*text_height)(struct qui_Context* ctx, const char *text);
    
    /** @brief Draw an image
     * @param ctx QuickUI context
     * @param img Image to draw
     * @param x X position
     * @param y Y position
     * @param w Width to draw
     * @param h Height to draw
     */
    void (*draw_image)(struct qui_Context* ctx, qui_Image *img, float x, float y, float w, float h);
} qui_Context;

/* ================================================================================================
 * CORE FUNCTIONS
 * ================================================================================================ */

/**
 * @brief Initialize QuickUI context with default values
 * @param ctx Pointer to context structure (must not be NULL)
 * @param user_data Optional user data pointer
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_init(qui_Context *ctx, void *user_data);

/**
 * @brief Clean up QuickUI context resources
 * @param ctx Pointer to context structure
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_cleanup(qui_Context *ctx);

/**
 * @brief Begin frame rendering
 * @param ctx Context pointer (must not be NULL)
 * @param start_x Starting X position for layout
 * @param start_y Starting Y position for layout
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_begin(qui_Context *ctx, float start_x, float start_y);

/**
 * @brief End frame rendering
 * @param ctx Context pointer (must not be NULL)
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_end(qui_Context *ctx);

/**
 * @brief Get error string for result code
 * @param result Result code
 * @return Human-readable error string
 */
const char* qui_get_error_string(qui_Result result);

/* ================================================================================================
 * INPUT HANDLING
 * ================================================================================================ */

/**
 * @brief Handle mouse button press
 * @param ctx Context pointer (must not be NULL)
 * @param x Mouse X position
 * @param y Mouse Y position
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_mouse_down(qui_Context *ctx, int x, int y);

/**
 * @brief Handle mouse button release
 * @param ctx Context pointer (must not be NULL)
 * @param x Mouse X position
 * @param y Mouse Y position
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_mouse_up(qui_Context *ctx, int x, int y);

/**
 * @brief Handle mouse movement
 * @param ctx Context pointer (must not be NULL)
 * @param x Mouse X position
 * @param y Mouse Y position
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_mouse_move(qui_Context *ctx, int x, int y);

/**
 * @brief Handle generic mouse button state
 * @param ctx Context pointer (must not be NULL)
 * @param pressed Non-zero if button is pressed
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_feed_mouse_button(qui_Context *ctx, int pressed);

/**
 * @brief Handle backspace key press
 * @param ctx Context pointer (must not be NULL)
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_feed_key_backspace(qui_Context *ctx);

/**
 * @brief Handle enter key press
 * @param ctx Context pointer (must not be NULL)
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_feed_key_enter(qui_Context *ctx);

/* ================================================================================================
 * UTILITY FUNCTIONS
 * ================================================================================================ */

/**
 * @brief Create a 2D vector
 * @param x X coordinate
 * @param y Y coordinate
 * @return qui_Vec2 structure
 */
qui_Vec2 qui_vec2(int x, int y);

/**
 * @brief Create a rectangle
 * @param width Rectangle width
 * @param height Rectangle height
 * @param pos_x X position
 * @param pos_y Y position
 * @return qui_Rect structure
 */
qui_Rect qui_rect(int width, int height, int pos_x, int pos_y);

/**
 * @brief Create an RGBA color
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @param a Alpha component (0-255)
 * @return qui_Color structure
 */
qui_Color qui_color(int r, int g, int b, int a);

/**
 * @brief Set color in context color scheme
 * @param ctx Context pointer (must not be NULL)
 * @param type Color type
 * @param color New color value
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_set_color(qui_Context *ctx, qui_ColorType type, qui_Color color);

/**
 * @brief Get color from context color scheme
 * @param ctx Context pointer (must not be NULL)
 * @param type Color type
 * @param color Output color value (must not be NULL)
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_get_color(qui_Context *ctx, qui_ColorType type, qui_Color *color);

/**
 * @brief Set font properties
 * @param ctx Context pointer (must not be NULL)
 * @param font Font handle (backend-specific)
 * @param font_size Font size
 * @param font_spacing Character spacing
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_set_font(qui_Context *ctx, void *font, float font_size, float font_spacing);

/* ================================================================================================
 * UI ELEMENTS
 * ================================================================================================ */

/**
 * @brief Create a button
 * @param ctx Context pointer (must not be NULL)
 * @param label Button text (must not be NULL)
 * @return 1 if clicked, 0 otherwise, negative on error
 */
int qui_button(qui_Context *ctx, const char *label);

/**
 * @brief Create a checkbox
 * @param ctx Context pointer (must not be NULL)
 * @param label Checkbox label (must not be NULL)
 * @param value Pointer to boolean value (must not be NULL)
 * @return 1 if value changed, 0 otherwise, negative on error
 */
int qui_checkbox(qui_Context *ctx, const char *label, int *value);

/**
 * @brief Create a floating-point slider
 * @param ctx Context pointer (must not be NULL)
 * @param label Slider label (must not be NULL)
 * @param value Pointer to float value (must not be NULL)
 * @param min_val Minimum slider value
 * @param max_val Maximum slider value
 * @param width Slider width (0 for default)
 * @return 1 on success, negative on error
 */
int qui_slider(qui_Context *ctx, const char *label, float *value, float min_val, float max_val, float width);

/**
 * @brief Create a text input box
 * @param ctx Context pointer (must not be NULL)
 * @param buffer Text buffer (must not be NULL)
 * @param capacity Buffer capacity (must be > 0)
 * @param width Textbox width (0 for default)
 * @return 1 if focused, 0 otherwise, negative on error
 */
int qui_textbox(qui_Context *ctx, char *buffer, size_t capacity, float width);

/**
 * @brief Create an image button
 * @param ctx Context pointer (must not be NULL)
 * @param image Image to display (must not be NULL)
 * @param button_width Button width (0 for auto)
 * @param button_height Button height (0 for auto)
 * @param img_width Image display width (0 for natural size)
 * @param img_height Image display height (0 for natural size)
 * @return 1 if clicked, 0 otherwise, negative on error
 */
int qui_image_button(qui_Context *ctx, qui_Image *image, float button_width, float button_height, float img_width, float img_height);

/**
 * @brief Create an image button with text label (horizontal layout)
 * @param ctx Context pointer (must not be NULL)
 * @param image Image to display (must not be NULL)
 * @param label Button label (must not be NULL)
 * @param img_width Image display width (0 for natural size)
 * @param img_height Image display height (0 for natural size)
 * @return 1 if clicked, 0 otherwise, negative on error
 */
int qui_image_button_with_label(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height);

/**
 * @brief Create an image button with text label (vertical layout)
 * @param ctx Context pointer (must not be NULL)
 * @param image Image to display (must not be NULL)
 * @param label Button label (must not be NULL)
 * @param img_width Image display width (0 for natural size)
 * @param img_height Image display height (0 for natural size)
 * @return 1 if clicked, 0 otherwise, negative on error
 */
int qui_image_button_vertical(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height);

/* ================================================================================================
 * WINDOW MANAGEMENT
 * ================================================================================================ */

/**
 * @brief Begin a draggable window
 * @param ctx Context pointer (must not be NULL)
 * @param title Window title (can be NULL)
 * @param size Window size
 * @param pos Pointer to window position (will be modified for dragging)
 * @return true on success, false on error
 */
bool qui_begin_window(qui_Context *ctx, const char *title, qui_Vec2 size, qui_Vec2 *pos);

/**
 * @brief End window rendering
 * @param ctx Context pointer (must not be NULL)
 * @return QUI_OK on success, error code on failure
 */
qui_Result qui_end_window(qui_Context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* QUICK_UI_H */

/* ================================================================================================
 * IMPLEMENTATION
 * ================================================================================================ */

#ifdef QUI_IMPLEMENTATION

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ================================================================================================
 * INTERNAL CONSTANTS AND HELPERS
 * ================================================================================================ */

/** @brief Default color palette */
static const qui_Color QUI_DEFAULT_COLORS[QUI_COLOR_COUNT] = {
    {32,  32,  32,  255}, /* QUI_COLOR_BACKGROUND */
    {56,  56,  56,  255}, /* QUI_COLOR_FOREGROUND */
    {80,  80,  80,  255}, /* QUI_COLOR_HOT */
    {100, 100, 100, 255}, /* QUI_COLOR_ACTIVE */
    {255, 255, 255, 255}, /* QUI_COLOR_TEXT */
    {48,  48,  48,  255}, /* QUI_COLOR_WINDOW_BG */
    {64,  64,  64,  255}  /* QUI_COLOR_TITLE_BAR */
};

/** @brief Default spacing values */
#define QUI_DEFAULT_SPACING_X 8.0f
#define QUI_DEFAULT_SPACING_Y 8.0f

/** @brief Default text metrics when callbacks aren't available */
#define QUI_FALLBACK_CHAR_WIDTH 8.0f
#define QUI_FALLBACK_TEXT_HEIGHT 16.0f

/** @brief Validation macro */
#define QUI_VALIDATE_CTX(ctx) \
    do { \
        if (!(ctx)) return QUI_ERROR_NULL_POINTER; \
    } while(0)

#define QUI_VALIDATE_PTR(ptr) \
    do { \
        if (!(ptr)) return QUI_ERROR_NULL_POINTER; \
    } while(0)

/** @brief Generate unique ID for UI element */
static qui_Id qui_gen_id(qui_Context *ctx) {
    if (!ctx) return 0;
    ctx->last_id += 1;
    if (ctx->last_id == 0) ctx->last_id = 1; /* Avoid zero ID */
    return ctx->last_id;
}

/** @brief Check if point is inside rectangle (with layout offset) */
static bool qui_hit_test(qui_Context *ctx, float x, float y, float w, float h) {
    if (!ctx) return false;
    
    float ox = ctx->layout_offset_x;
    float oy = ctx->layout_offset_y;
    int mx = ctx->mouse_pos.x;
    int my = ctx->mouse_pos.y;
    
    return (mx >= x + ox && mx <= x + ox + w &&
            my >= y + oy && my <= y + oy + h);
}

/** @brief Check if point is inside rectangle (absolute coordinates) */
static bool qui_hit_test_absolute(qui_Context *ctx, float x, float y, float w, float h) {
    if (!ctx) return false;
    
    int mx = ctx->mouse_pos.x;
    int my = ctx->mouse_pos.y;
    
    return (mx >= x && mx <= x + w &&
            my >= y && my <= y + h);
}

/** @brief Get text width with fallback */
static float qui_get_text_width(qui_Context *ctx, const char *text) {
    if (!ctx || !text) return 0.0f;
    
    if (ctx->text_width) {
        return ctx->text_width(ctx, text);
    }
    
    /* Fallback calculation */
    return (float)strlen(text) * QUI_FALLBACK_CHAR_WIDTH;
}

/** @brief Get text height with fallback */
static float qui_get_text_height(qui_Context *ctx, const char *text) {
    if (!ctx || !text) return 0.0f;
    
    if (ctx->text_height) {
        return ctx->text_height(ctx, text);
    }
    
    /* Fallback calculation */
    return QUI_FALLBACK_TEXT_HEIGHT;
}

/** @brief Draw rectangle with error checking */
static void qui_draw_rect_safe(qui_Context *ctx, qui_Rect *rect, qui_Color color) {
    if (!ctx || !rect || !ctx->draw_rect) return;
    
    float ox = ctx->layout_offset_x;
    float oy = ctx->layout_offset_y;
    
    ctx->draw_rect(ctx,
                   rect->pos_x + ox,
                   rect->pos_y + oy,
                   rect->width,
                   rect->height,
                   color);
}

/** @brief Draw text with error checking */
static void qui_draw_text_safe(qui_Context *ctx, const char *text, float x, float y) {
    if (!ctx || !text || !ctx->draw_text) return;
    
    float ox = ctx->layout_offset_x;
    float oy = ctx->layout_offset_y;
    
    ctx->draw_text(ctx, text, x + ox, y + oy);
}

/** @brief Draw image with error checking */
static void qui_draw_image_safe(qui_Context *ctx, qui_Image *image, float x, float y, float w, float h) {
    if (!ctx || !image || !ctx->draw_image) return;
    
    float ox = ctx->layout_offset_x;
    float oy = ctx->layout_offset_y;
    
    ctx->draw_image(ctx, image, x + ox, y + oy, w, h);
}

/* ================================================================================================
 * CORE FUNCTIONS IMPLEMENTATION
 * ================================================================================================ */

qui_Result qui_init(qui_Context *ctx, void *user_data) {
    QUI_VALIDATE_CTX(ctx);
    
    /* Clear entire structure */
    memset(ctx, 0, sizeof(*ctx));
    
    /* Set user data */
    ctx->userdata = user_data;
    
    /* Initialize default spacing */
    ctx->spacing_x = QUI_DEFAULT_SPACING_X;
    ctx->spacing_y = QUI_DEFAULT_SPACING_Y;
    
    /* Copy default color scheme */
    memcpy(ctx->colors, QUI_DEFAULT_COLORS, sizeof(QUI_DEFAULT_COLORS));
    
    /* Initialize ID counter */
    ctx->last_id = 0;
    
    /* Initialize font properties */
    ctx->font = NULL;
    ctx->font_size = 0.0f;
    ctx->font_spacing = 0.0f;
    
    return QUI_OK;
}

qui_Result qui_cleanup(qui_Context *ctx) {
    QUI_VALIDATE_CTX(ctx);
    
    /* Currently no dynamic resources to clean up */
    /* This function is provided for future extensibility */
    
    return QUI_OK;
}

qui_Result qui_begin(qui_Context *ctx, float start_x, float start_y) {
    QUI_VALIDATE_CTX(ctx);
    
    /* Validate coordinates */
    if (!isfinite(start_x) || !isfinite(start_y)) {
        return QUI_ERROR_INVALID_VALUE;
    }
    
    /* Reset layout state */
    ctx->cursor_x = start_x;
    ctx->cursor_y = start_y;
    ctx->last_id = 0;
    ctx->hot_id = 0;
    
    return QUI_OK;
}

qui_Result qui_end(qui_Context *ctx) {
    QUI_VALIDATE_CTX(ctx);
    
    /* Clear frame-specific input states */
    ctx->mouse_pressed = 0;
    ctx->mouse_released = 0;
    ctx->key_backspace = 0;
    ctx->key_enter = 0;
    
    return QUI_OK;
}

const char* qui_get_error_string(qui_Result result) {
    switch (result) {
        case QUI_OK: return "Success";
        case QUI_ERROR_NULL_POINTER: return "Null pointer argument";
        case QUI_ERROR_INVALID_VALUE: return "Invalid parameter value";
        case QUI_ERROR_NOT_INITIALIZED: return "Context not initialized";
        case QUI_ERROR_BUFFER_TOO_SMALL: return "Buffer capacity too small";
        case QUI_ERROR_INVALID_STATE: return "Invalid operation for current state";
        default: return "Unknown error";
    }
}

/* ================================================================================================
 * INPUT HANDLING IMPLEMENTATION
 * ================================================================================================ */

qui_Result qui_mouse_down(qui_Context *ctx, int x, int y) {
    QUI_VALIDATE_CTX(ctx);
    
    ctx->mouse_pos.x = x;
    ctx->mouse_pos.y = y;
    ctx->mouse_down = 1;
    ctx->mouse_pressed = 1;
    
    return QUI_OK;
}

qui_Result qui_mouse_up(qui_Context *ctx, int x, int y) {
    QUI_VALIDATE_CTX(ctx);
    
    ctx->mouse_pos.x = x;
    ctx->mouse_pos.y = y;
    ctx->mouse_down = 0;
    ctx->mouse_released = 1;
    
    return QUI_OK;
}

qui_Result qui_mouse_move(qui_Context *ctx, int x, int y) {
    QUI_VALIDATE_CTX(ctx);
    
    ctx->mouse_pos.x = x;
    ctx->mouse_pos.y = y;
    
    return QUI_OK;
}

qui_Result qui_feed_mouse_button(qui_Context *ctx, int pressed) {
    QUI_VALIDATE_CTX(ctx);
    
    ctx->mouse_pressed = pressed ? 1 : 0;
    
    return QUI_OK;
}

qui_Result qui_feed_key_backspace(qui_Context *ctx) {
    QUI_VALIDATE_CTX(ctx);
    
    ctx->key_backspace = 1;
    
    return QUI_OK;
}

qui_Result qui_feed_key_enter(qui_Context *ctx) {
    QUI_VALIDATE_CTX(ctx);
    
    ctx->key_enter = 1;
    
    return QUI_OK;
}

/* ================================================================================================
 * UTILITY FUNCTIONS IMPLEMENTATION
 * ================================================================================================ */

qui_Vec2 qui_vec2(int x, int y) {
    qui_Vec2 v = {x, y};
    return v;
}

qui_Rect qui_rect(int width, int height, int pos_x, int pos_y) {
    qui_Rect r = {width, height, pos_x, pos_y};
    return r;
}

qui_Color qui_color(int r, int g, int b, int a) {
    /* Clamp values to valid range */
    if (r < 0) r = 0; else if (r > 255) r = 255;
    if (g < 0) g = 0; else if (g > 255) g = 255;
    if (b < 0) b = 0; else if (b > 255) b = 255;
    if (a < 0) a = 0; else if (a > 255) a = 255;
    
    qui_Color color = {r, g, b, a};
    return color;
}

qui_Result qui_set_color(qui_Context *ctx, qui_ColorType type, qui_Color color) {
    QUI_VALIDATE_CTX(ctx);
    
    if (type >= QUI_COLOR_COUNT) {
        return QUI_ERROR_INVALID_VALUE;
    }
    
    ctx->colors[type] = color;
    return QUI_OK;
}

qui_Result qui_get_color(qui_Context *ctx, qui_ColorType type, qui_Color *color) {
    QUI_VALIDATE_CTX(ctx);
    QUI_VALIDATE_PTR(color);
    
    if (type >= QUI_COLOR_COUNT) {
        return QUI_ERROR_INVALID_VALUE;
    }
    
    *color = ctx->colors[type];
    return QUI_OK;
}

qui_Result qui_set_font(qui_Context *ctx, void *font, float font_size, float font_spacing) {
    QUI_VALIDATE_CTX(ctx);
    
    if (font_size < 0.0f || !isfinite(font_size) || 
        font_spacing < 0.0f || !isfinite(font_spacing)) {
        return QUI_ERROR_INVALID_VALUE;
    }
    
    ctx->font = font;
    ctx->font_size = font_size;
    ctx->font_spacing = font_spacing;
    
    return QUI_OK;
}

/* ================================================================================================
 * UI ELEMENTS IMPLEMENTATION
 * ================================================================================================ */

int qui_button(qui_Context *ctx, const char *label) {
    if (!ctx || !label) return -1;
    
    qui_Id id = qui_gen_id(ctx);
    float text_width = qui_get_text_width(ctx, label);
    float text_height = qui_get_text_height(ctx, label);
    float padding = 10.0f;
    
    float w = text_width + (padding * 2);
    float h = text_height + 8.0f;
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    
    /* Hit testing */
    if (qui_hit_test(ctx, x, y, w, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->active_id = id;
        }
    }
    
    /* Determine color based on state */
    qui_Color color = ctx->colors[QUI_COLOR_FOREGROUND];
    if (ctx->active_id == id) {
        color = ctx->colors[QUI_COLOR_ACTIVE];
    } else if (ctx->hot_id == id) {
        color = ctx->colors[QUI_COLOR_HOT];
    }
    
    /* Draw button background */
    qui_Rect rect = qui_rect((int)w, (int)h, (int)x, (int)y);
    qui_draw_rect_safe(ctx, &rect, color);
    
    /* Draw button text */
    qui_draw_text_safe(ctx, label, x + padding, y + 4.0f);
    
    /* Check for click */
    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit_test(ctx, x, y, w, h)) {
            clicked = 1;
        }
        ctx->active_id = 0;
    }
    
    /* Update layout cursor */
    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    
    return clicked;
}

int qui_checkbox(qui_Context *ctx, const char *label, int *value) {
    if (!ctx || !label || !value) return -1;
    
    qui_Id id = qui_gen_id(ctx);
    float box_size = 18.0f;
    float text_width = qui_get_text_width(ctx, label);
    float text_height = qui_get_text_height(ctx, label);
    float spacing = 6.0f;
    
    float total_width = box_size + spacing + text_width;
    float h = fmaxf(box_size, text_height);
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    
    /* Hit testing */
    if (qui_hit_test(ctx, x, y, total_width, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->active_id = id;
        }
    }
    
    /* Determine color based on state */
    qui_Color color = ctx->colors[QUI_COLOR_FOREGROUND];
    if (ctx->active_id == id) {
        color = ctx->colors[QUI_COLOR_ACTIVE];
    } else if (ctx->hot_id == id) {
        color = ctx->colors[QUI_COLOR_HOT];
    }
    
    /* Draw checkbox box */
    qui_Rect box_rect = qui_rect((int)box_size, (int)box_size, (int)x, (int)y);
    qui_draw_rect_safe(ctx, &box_rect, color);
    
    /* Draw check mark if checked */
    if (*value) {
        qui_draw_text_safe(ctx, "X", x + 4.0f, y + 1.0f);
    }
    
    /* Draw label */
    qui_draw_text_safe(ctx, label, x + box_size + spacing, y + 1.0f);
    
    /* Check for click */
    int changed = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit_test(ctx, x, y, total_width, h)) {
            *value = !(*value);
            changed = 1;
        }
        ctx->active_id = 0;
    }
    
    /* Update layout cursor */
    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    
    return changed;
}

int qui_slider(qui_Context *ctx, const char *label, float *value, float min_val, float max_val, float width) {
    if (!ctx || !label || !value) return -1;
    
    if (min_val >= max_val || !isfinite(min_val) || !isfinite(max_val) || 
        !isfinite(*value) || width < 0.0f) {
        return -1;
    }
    
    qui_Id id = qui_gen_id(ctx);
    float label_width = qui_get_text_width(ctx, label);
    float text_height = qui_get_text_height(ctx, label);
    float slider_width = (width > 0.0f) ? width : 160.0f;
    float slider_height = 12.0f;
    float knob_width = 10.0f;
    float spacing = 12.0f;
    float value_display_width = 60.0f;
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    float slider_x = x + label_width + spacing;
    float h = text_height + 8.0f;
    
    /* Clamp value to valid range */
    if (*value < min_val) *value = min_val;
    if (*value > max_val) *value = max_val;
    
    /* Hit testing on slider area */
    if (qui_hit_test(ctx, slider_x, y, slider_width, slider_height)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->active_id = id;
        }
    }
    
    /* Determine color based on state */
    qui_Color color = ctx->colors[QUI_COLOR_FOREGROUND];
    if (ctx->active_id == id) {
        color = ctx->colors[QUI_COLOR_ACTIVE];
    } else if (ctx->hot_id == id) {
        color = ctx->colors[QUI_COLOR_HOT];
    }
    
    /* Draw label */
    qui_draw_text_safe(ctx, label, x, y);
    
    /* Draw slider background */
    qui_Rect slider_bg = qui_rect((int)slider_width, (int)slider_height, (int)slider_x, (int)(y + 2));
    qui_draw_rect_safe(ctx, &slider_bg, color);
    
    /* Calculate knob position */
    float t = (*value - min_val) / (max_val - min_val);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float knob_x = slider_x + t * (slider_width - knob_width);
    
    /* Handle dragging */
    if (ctx->active_id == id && ctx->mouse_down) {
        float local_x = (float)ctx->mouse_pos.x - (slider_x + ctx->layout_offset_x);
        float new_t = local_x / (slider_width - knob_width);
        if (new_t < 0.0f) new_t = 0.0f;
        if (new_t > 1.0f) new_t = 1.0f;
        *value = min_val + new_t * (max_val - min_val);
    }
    
    if (ctx->mouse_released && ctx->active_id == id) {
        ctx->active_id = 0;
    }
    
    /* Draw knob */
    qui_Rect knob_rect = qui_rect((int)knob_width, (int)(slider_height + 4), (int)knob_x, (int)y);
    qui_draw_rect_safe(ctx, &knob_rect, ctx->colors[QUI_COLOR_ACTIVE]);
    
    /* Draw value display */
    char value_buffer[32];
    snprintf(value_buffer, sizeof(value_buffer), "%.2f", *value);
    qui_draw_text_safe(ctx, value_buffer, slider_x + slider_width + 8.0f, y);
    
    /* Update layout cursor */
    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    
    return 1;
}

int qui_textbox(qui_Context *ctx, char *buffer, size_t capacity, float width) {
    if (!ctx || !buffer || capacity == 0) return -1;
    
    qui_Id id = qui_gen_id(ctx);
    float text_height = qui_get_text_height(ctx, "A");
    float box_width = (width > 0.0f) ? width : 240.0f;
    float h = text_height + 8.0f;
    float padding = 6.0f;
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    
    /* Hit testing */
    if (qui_hit_test(ctx, x, y, box_width, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->keyboard_focus_id = id;
        }
    } else if (ctx->mouse_pressed && ctx->keyboard_focus_id == id) {
        /* Clicked outside while focused - lose focus */
        ctx->keyboard_focus_id = 0;
    }
    
    /* Determine color based on state */
    qui_Color color = ctx->colors[QUI_COLOR_FOREGROUND];
    if (ctx->keyboard_focus_id == id) {
        color = ctx->colors[QUI_COLOR_ACTIVE];
    } else if (ctx->hot_id == id) {
        color = ctx->colors[QUI_COLOR_HOT];
    }
    
    /* Draw textbox background */
    qui_Rect rect = qui_rect((int)box_width, (int)h, (int)x, (int)y);
    qui_draw_rect_safe(ctx, &rect, color);
    
    /* Draw text content */
    qui_draw_text_safe(ctx, buffer, x + padding, y + 4.0f);
    
    /* Draw cursor if focused */
    if (ctx->keyboard_focus_id == id) {
        float text_width = qui_get_text_width(ctx, buffer);
        qui_draw_text_safe(ctx, "|", x + padding + text_width, y + 2.0f);
    }
    
    /* Handle keyboard input */
    if (ctx->keyboard_focus_id == id) {
        if (ctx->key_backspace) {
            size_t len = strlen(buffer);
            if (len > 0) {
                buffer[len - 1] = '\0';
            }
        }
    }
    
    /* Update layout cursor */
    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    
    return (ctx->keyboard_focus_id == id) ? 1 : 0;
}

int qui_image_button(qui_Context *ctx, qui_Image *image, float button_width, float button_height, float img_width, float img_height) {
    if (!ctx || !image) return -1;
    
    if (image->width <= 0 || image->height <= 0 || !image->data) {
        return -1;
    }
    
    qui_Id id = qui_gen_id(ctx);
    
    /* Calculate dimensions */
    float padding = 8.0f;
    float w = (button_width > 0.0f) ? button_width : (float)image->width + padding * 2;
    float h = (button_height > 0.0f) ? button_height : (float)image->height + padding * 2;
    float img_w = (img_width > 0.0f) ? img_width : (float)image->width;
    float img_h = (img_height > 0.0f) ? img_height : (float)image->height;
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    
    /* Hit testing */
    if (qui_hit_test(ctx, x, y, w, h)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->active_id = id;
        }
    }
    
    /* Calculate centered image position */
    float centered_x = x + (w - img_w) * 0.5f;
    float centered_y = y + (h - img_h) * 0.5f;
    
    /* Draw button background and image based on state */
    if (ctx->active_id == id) {
        qui_Rect bg_rect = qui_rect((int)w, (int)h, (int)x, (int)y);
        qui_draw_rect_safe(ctx, &bg_rect, ctx->colors[QUI_COLOR_ACTIVE]);
        qui_draw_image_safe(ctx, image, centered_x + 1, centered_y + 1, img_w, img_h);
    } else if (ctx->hot_id == id) {
        qui_Rect bg_rect = qui_rect((int)w, (int)h, (int)x, (int)y);
        qui_draw_rect_safe(ctx, &bg_rect, ctx->colors[QUI_COLOR_HOT]);
        qui_draw_image_safe(ctx, image, centered_x, centered_y, img_w, img_h);
    } else {
        qui_draw_image_safe(ctx, image, centered_x, centered_y, img_w, img_h);
    }
    
    /* Check for click */
    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit_test(ctx, x, y, w, h)) {
            clicked = 1;
        }
        ctx->active_id = 0;
    }
    
    /* Update layout cursor */
    ctx->cursor_y += h + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    
    return clicked;
}

int qui_image_button_with_label(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height) {
    if (!ctx || !image || !label) return -1;
    
    if (image->width <= 0 || image->height <= 0 || !image->data) {
        /* Fallback to text button if image is invalid */
        return qui_button(ctx, label);
    }
    
    qui_Id id = qui_gen_id(ctx);
    float img_w = (img_width > 0.0f) ? img_width : (float)image->width;
    float img_h = (img_height > 0.0f) ? img_height : (float)image->height;
    float text_width = qui_get_text_width(ctx, label);
    float text_height = qui_get_text_height(ctx, label);
    
    float padding = 8.0f;
    float spacing = 6.0f;
    float total_width = img_w + spacing + text_width + (padding * 2);
    float total_height = fmaxf(img_h, text_height) + (padding * 2);
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    
    /* Hit testing */
    if (qui_hit_test(ctx, x, y, total_width, total_height)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->active_id = id;
        }
    }
    
    /* Determine color based on state */
    qui_Color color = ctx->colors[QUI_COLOR_FOREGROUND];
    if (ctx->active_id == id) {
        color = ctx->colors[QUI_COLOR_ACTIVE];
    } else if (ctx->hot_id == id) {
        color = ctx->colors[QUI_COLOR_HOT];
    }
    
    /* Draw button background */
    qui_Rect bg_rect = qui_rect((int)total_width, (int)total_height, (int)x, (int)y);
    qui_draw_rect_safe(ctx, &bg_rect, color);
    
    /* Calculate positions */
    float img_x = x + padding;
    float img_y = y + (total_height - img_h) * 0.5f;
    float text_x = img_x + img_w + spacing;
    float text_y = y + (total_height - text_height) * 0.5f;
    
    /* Apply press offset for active state */
    if (ctx->active_id == id) {
        img_x += 1;
        img_y += 1;
        text_x += 1;
        text_y += 1;
    }
    
    /* Draw image and text */
    qui_draw_image_safe(ctx, image, img_x, img_y, img_w, img_h);
    qui_draw_text_safe(ctx, label, text_x, text_y);
    
    /* Check for click */
    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit_test(ctx, x, y, total_width, total_height)) {
            clicked = 1;
        }
        ctx->active_id = 0;
    }
    
    /* Update layout cursor */
    ctx->cursor_y += total_height + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    
    return clicked;
}

int qui_image_button_vertical(qui_Context *ctx, qui_Image *image, const char *label, float img_width, float img_height) {
    if (!ctx || !image || !label) return -1;
    
    if (image->width <= 0 || image->height <= 0 || !image->data) {
        /* Fallback to text button if image is invalid */
        return qui_button(ctx, label);
    }
    
    qui_Id id = qui_gen_id(ctx);
    float img_w = (img_width > 0.0f) ? img_width : (float)image->width;
    float img_h = (img_height > 0.0f) ? img_height : (float)image->height;
    float text_width = qui_get_text_width(ctx, label);
    float text_height = qui_get_text_height(ctx, label);
    
    float padding = 8.0f;
    float spacing = 4.0f;
    float total_width = fmaxf(img_w, text_width) + (padding * 2);
    float total_height = img_h + spacing + text_height + (padding * 2);
    
    float x = ctx->cursor_x;
    float y = ctx->cursor_y;
    
    /* Hit testing */
    if (qui_hit_test(ctx, x, y, total_width, total_height)) {
        ctx->hot_id = id;
        if (ctx->mouse_pressed) {
            ctx->active_id = id;
        }
    }
    
    /* Determine color based on state */
    qui_Color color = ctx->colors[QUI_COLOR_FOREGROUND];
    if (ctx->active_id == id) {
        color = ctx->colors[QUI_COLOR_ACTIVE];
    } else if (ctx->hot_id == id) {
        color = ctx->colors[QUI_COLOR_HOT];
    }
    
    /* Draw button background */
    qui_Rect bg_rect = qui_rect((int)total_width, (int)total_height, (int)x, (int)y);
    qui_draw_rect_safe(ctx, &bg_rect, color);
    
    /* Calculate centered positions */
    float img_x = x + (total_width - img_w) * 0.5f;
    float img_y = y + padding;
    float text_x = x + (total_width - text_width) * 0.5f;
    float text_y = img_y + img_h + spacing;
    
    /* Apply press offset for active state */
    if (ctx->active_id == id) {
        img_x += 1;
        img_y += 1;
        text_x += 1;
        text_y += 1;
    }
    
    /* Draw image and text */
    qui_draw_image_safe(ctx, image, img_x, img_y, img_w, img_h);
    qui_draw_text_safe(ctx, label, text_x, text_y);
    
    /* Check for click */
    int clicked = 0;
    if (ctx->mouse_released && ctx->active_id == id) {
        if (qui_hit_test(ctx, x, y, total_width, total_height)) {
            clicked = 1;
        }
        ctx->active_id = 0;
    }
    
    /* Update layout cursor */
    ctx->cursor_y += total_height + ctx->spacing_y;
    ctx->cursor_x = ctx->spacing_x;
    
    return clicked;
}

/* ================================================================================================
 * WINDOW MANAGEMENT IMPLEMENTATION
 * ================================================================================================ */

bool qui_begin_window(qui_Context *ctx, const char *title, qui_Vec2 size, qui_Vec2 *pos) {
    if (!ctx || !pos) return false;
    
    if (size.x <= 0 || size.y <= 0) return false;
    
    qui_Id window_id = qui_gen_id(ctx);
    
    float x = (float)pos->x;
    float y = (float)pos->y;
    float w = (float)size.x;
    float h = (float)size.y;
    float title_height = qui_get_text_height(ctx, title ? title : "Window") + 8.0f;
    
    /* Handle window dragging */
    if (ctx->active_id == 0 || ctx->active_id == window_id) {
        bool title_bar_hit = qui_hit_test_absolute(ctx, x, y, w - 4, title_height);
        
        if (title_bar_hit) {
            ctx->hot_id = window_id;
            if (ctx->mouse_pressed) {
                ctx->active_id = window_id;
                ctx->drag_offset_x = (float)ctx->mouse_pos.x - x;
                ctx->drag_offset_y = (float)ctx->mouse_pos.y - y;
            }
        }
    }
    
    /* Update window position if being dragged */
    if (ctx->active_id == window_id && ctx->mouse_down) {
        pos->x = (int)((float)ctx->mouse_pos.x - ctx->drag_offset_x);
        pos->y = (int)((float)ctx->mouse_pos.y - ctx->drag_offset_y);
        x = (float)pos->x;
        y = (float)pos->y;
    }
    
    /* Draw window background */
    qui_Rect window_bg = qui_rect((int)w, (int)h, (int)x, (int)y);
    qui_draw_rect_safe(ctx, &window_bg, ctx->colors[QUI_COLOR_WINDOW_BG]);
    
    /* Draw title bar */
    qui_Rect title_bar = qui_rect((int)w, (int)title_height, (int)x, (int)y);
    qui_draw_rect_safe(ctx, &title_bar, ctx->colors[QUI_COLOR_TITLE_BAR]);
    
    /* Draw title text */
    if (title) {
        qui_draw_text_safe(ctx, title, x + 8.0f, y + 4.0f);
    }
    
    /* Release drag on mouse up */
    if (ctx->mouse_released && ctx->active_id == window_id) {
        ctx->active_id = 0;
    }
    
    /* Save current layout state */
    ctx->saved_cursor_x = ctx->cursor_x;
    ctx->saved_cursor_y = ctx->cursor_y;
    ctx->saved_offset_x = ctx->layout_offset_x;
    ctx->saved_offset_y = ctx->layout_offset_y;
    
    /* Set up window-relative coordinate system */
    ctx->layout_offset_x = x + 10.0f;
    ctx->layout_offset_y = y + title_height + 10.0f;
    ctx->cursor_x = 0.0f;
    ctx->cursor_y = 0.0f;
    
    return true;
}

qui_Result qui_end_window(qui_Context *ctx) {
    QUI_VALIDATE_CTX(ctx);
    
    /* Restore previous layout state */
    ctx->cursor_x = ctx->saved_cursor_x;
    ctx->cursor_y = ctx->saved_cursor_y;
    ctx->layout_offset_x = ctx->saved_offset_x;
    ctx->layout_offset_y = ctx->saved_offset_y;
    
    return QUI_OK;
}

#endif /* QUI_IMPLEMENTATION */
