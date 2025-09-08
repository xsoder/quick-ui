// RAYLIB_EXAMPLE.c
#include <raylib.h>
#include "quickui.h"
#include <stdio.h>
#include <string.h>

#define is_font_enable true

// BACKEND IMPLEMENTATION
void raylib_draw_rect(qui_Context* ctx, float x, float y, float w, float h, qui_Color col) {
    Color raylib_color = { (unsigned char)col.r, (unsigned char)col.g, (unsigned char)col.b, (unsigned char)col.a };
    DrawRectangle((int)x, (int)y, (int)w, (int)h, raylib_color);
}

void raylib_draw_text(qui_Context* ctx, const char *text, float x, float y) {
    Font *fontPtr = (Font *)ctx->font;
    Color font_color = { (unsigned char)ctx->col_text.r, (unsigned char)ctx->col_text.g, (unsigned char)ctx->col_text.b, (unsigned char)ctx->col_text.a };
    
    if (!fontPtr) {
        DrawText(text, (int)x, (int)y, 20, font_color);
    } else {
        DrawTextEx(*fontPtr, text, (Vector2){x, y}, ctx->font_size, ctx->font_spacing, font_color);
    }
}

void raylib_draw_image(qui_Context* ctx, qui_Image *img, float x, float y, float w, float h) {
    if (!img || !img->data) return;
    Texture2D *texture = (Texture2D*)img->data;
    if (!texture->id) return;
    Rectangle source = { 0, 0, (float)texture->width, (float)texture->height };
    Rectangle dest = { x, y, w, h };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(*texture, source, dest, origin, 0.0f, WHITE);
}

float raylib_text_width(qui_Context* ctx, const char *text) {
    Font *fontPtr = (Font *)ctx->font;
    if (!fontPtr) {
        return (float)MeasureText(text, 20);
    } else {
        Vector2 size = MeasureTextEx(*fontPtr, text, ctx->font_size, ctx->font_spacing);
        return size.x;
    }
}

float raylib_text_height(qui_Context* ctx, const char *text) {
    Font *fontPtr = (Font *)ctx->font;
    if (!fontPtr) {
        return 20.0f;
    } else {
        Vector2 size = MeasureTextEx(*fontPtr, text, ctx->font_size, ctx->font_spacing);
        return size.y;
    }
}

Texture2D CreateColoredTexture(int width, int height, Color color) {
    Image img = GenImageColor(width, height, color);
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);
    return texture;
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "QuickUI + Raylib Image Button Example");
    SetTargetFPS(60);

    qui_Context ctx;
    qui_init(&ctx, NULL);
    
    Font font = {0};
    if (FileExists("Iosevka-Regular.ttf")) {
        font = LoadFontEx("Iosevka-Regular.ttf", 128, 0, 0);
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
        qui_set_font(&ctx, &font, 20, 2);
    } else {
        qui_set_font(&ctx, NULL, 20, 2);
    }
    
    ctx.draw_rect = raylib_draw_rect;
    ctx.draw_text = raylib_draw_text;
    ctx.draw_image = raylib_draw_image;
    ctx.text_width = raylib_text_width;
    ctx.text_height = raylib_text_height;
    
    Texture2D save_texture = CreateColoredTexture(32, 32, GREEN);
    Texture2D load_texture = CreateColoredTexture(32, 32, BLUE);
    Texture2D delete_texture = CreateColoredTexture(32, 32, RED);
    Texture2D settings_texture = CreateColoredTexture(24, 24, PURPLE);
    
    qui_Image save_icon = { &save_texture, save_texture.width, save_texture.height, 4 };
    qui_Image load_icon = { &load_texture, load_texture.width, load_texture.height, 4 };
    qui_Image delete_icon = { &delete_texture, delete_texture.width, delete_texture.height, 4 };
    qui_Image settings_icon = { &settings_texture, settings_texture.width, settings_texture.height, 4 };
    
    static int checkbox_value = 0;
    static float slider_value = 50.0f;
    static char textbox_buffer[256] = "Type here...";
    static int button_clicks = 0;
    static int image_button_clicks = 0;
    static int save_clicks = 0;
    static int load_clicks = 0;
    static int delete_clicks = 0;

    static qui_vec2_t window_size = { 450, 500 };
    static qui_vec2_t window_pos = { 50, 50 };
    static int show_popup = 0;

    static float hue = 200.0f;
    static float sat = 0.7f;
    static float value = 0.9f;

    while (!WindowShouldClose()) {

        ctx.width = screenWidth;
        ctx.height = screenHeight;

        Color col = ColorFromHSV(hue, sat, value);
        Vector2 vec = { 404, 404 };
        if(show_popup) DrawTextEx(font, "Progress", vec, 50.0f, 2.0f, WHITE);
        else DrawTextEx(font, "Progress", vec, 50.0f, 2.0f, col);
        
        Vector2 mouse_pos = GetMousePosition();
        qui_mouse_move(&ctx, (int)mouse_pos.x, (int)mouse_pos.y);
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            qui_mouse_down(&ctx, (int)mouse_pos.x, (int)mouse_pos.y);
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            qui_mouse_up(&ctx, (int)mouse_pos.x, (int)mouse_pos.y);
        }
        
        if (ctx.keyboard_focus_id > 0) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125) {
                    size_t len = strlen(textbox_buffer);
                    if (len < sizeof(textbox_buffer) - 1) {
                        textbox_buffer[len] = (char)key;
                        textbox_buffer[len + 1] = '\0';
                    }
                }
                key = GetCharPressed();
            }
            
            if (IsKeyPressed(KEY_BACKSPACE)) {
                qui_feed_key_backspace(&ctx);
            }
            
            if (IsKeyPressed(KEY_ENTER)) {
                qui_feed_key_enter(&ctx);
            }
        }
        
        BeginDrawing();
        ClearBackground(col);
        
        qui_begin(&ctx, 50.0f, 50.0f);
        
        Vector2 position = {50, 20};
        float size = 30;
        float spacing = 2;
        if (is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font,"QuickUI + Raylib Image Button Demo", position, size, spacing, RAYWHITE);
        else DrawText("QuickUI + Raylib Image Button Demo", 50, 20, 30, RAYWHITE);
        
        if (qui_button(&ctx, "Click Me!")) {
            button_clicks++;
        }

        char button_info[64];
        snprintf(button_info, sizeof(button_info), "Button clicked: %d times", button_clicks);
        if(is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font, button_info, (Vector2) {50, (int)ctx.cursor_y}, size, spacing, LIGHTGRAY);
        else DrawText(button_info, 50, (int)ctx.cursor_y, 16, LIGHTGRAY);
        ctx.cursor_y += 25;
        
        if(is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font, "Image Buttons:", (Vector2) {50, (int)ctx.cursor_y}, size, spacing, RAYWHITE);
        else DrawText("Image Buttons:", 50, (int)ctx.cursor_y, 16, RAYWHITE);
        ctx.cursor_y += 30;
        
        if (qui_image_button(&ctx, &save_icon, 48, 48, 32, 32)) {
            image_button_clicks++;
        }
        
        if (qui_image_button_with_label(&ctx, &load_icon, "Load File", 32, 32)) {
            load_clicks++;
        }
        
        if (qui_image_button_vertical(&ctx, &delete_icon, "Delete", 32, 32)) {
            delete_clicks++;
        }
        
        char image_info[256];
        snprintf(image_info, sizeof(image_info), 
            "Image clicks: %d | Load: %d | Delete: %d", 
            image_button_clicks, load_clicks, delete_clicks);
        if(is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font, image_info, (Vector2) {50, (int)ctx.cursor_y}, size, spacing, LIGHTGRAY);
        else DrawText(image_info, 50, (int)ctx.cursor_y, 16, LIGHTGRAY);
        ctx.cursor_y += 35;
        
        qui_checkbox(&ctx, "Enable Feature", &checkbox_value);
        
        char checkbox_info[64];
        snprintf(checkbox_info, sizeof(checkbox_info), "Checkbox is: %s", checkbox_value ? "ON" : "OFF");
        if (is_font_enable && ctx.font)DrawTextEx(*(Font*)ctx.font, checkbox_info, (Vector2) {50, (int)ctx.cursor_y}, size, spacing, LIGHTGRAY);
        else DrawText(checkbox_info, 50, (int)ctx.cursor_y, 16, LIGHTGRAY);
        ctx.cursor_y += 25;
        
        qui_slider(&ctx, "Volume", &slider_value, 0.0f, 100.0f, 200.0f);
        
        int focused = qui_textbox(&ctx, textbox_buffer, sizeof(textbox_buffer), 300.0f);
        
        char textbox_info[64];
        snprintf(textbox_info, sizeof(textbox_info), "Text input %s", focused ? "(focused)" : "");
        if(is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font, textbox_info, (Vector2) {50, (int)ctx.cursor_y}, size, spacing, LIGHTGRAY);
        else DrawText(textbox_info, 50, (int)ctx.cursor_y, 16, LIGHTGRAY);
        ctx.cursor_y += 25;
        
        char content_info[512];
        snprintf(content_info, sizeof(content_info), "Content: \"%s\"", textbox_buffer);
        if (is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font, content_info, (Vector2) {50, (int)ctx.cursor_y}, size, spacing, YELLOW);
        else DrawText(content_info, 50, (int)ctx.cursor_y, 16, LIGHTGRAY);
        ctx.cursor_y += 30;
        
        char debug_info[256];
        snprintf(debug_info, sizeof(debug_info), 
            "Mouse: (%d, %d) | Active ID: %u | Hot ID: %u | Focus ID: %u",
            (int)mouse_pos.x, (int)mouse_pos.y, 
            ctx.active_id, ctx.hot_id, ctx.keyboard_focus_id);
        if(is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font, debug_info, (Vector2) {10, screenHeight - 25}, size - 2, 2, BLUE);
        else DrawText(debug_info, 10, screenHeight - 25, 12, BLUE);

        if (qui_begin_window(&ctx, "Settings Window", window_size, &window_pos)) {
            qui_checkbox(&ctx, "Show Progress Text", &show_popup);
            
            if (qui_image_button_with_label(&ctx, &settings_icon, "Settings", 20, 20)) {
                show_popup = !show_popup;
            }
            
            qui_slider(&ctx, "Hue", &hue, 0.0f, 360.0f, 150.0f);
            qui_slider(&ctx, "Saturation", &sat, 0.0f, 1.0f, 150.0f);
            qui_slider(&ctx, "Value", &value, 0.0f, 1.0f, 150.0f);

            if(is_font_enable && ctx.font) DrawTextEx(*(Font*)ctx.font, "Toolbar:", (Vector2) {ctx.layout_offset_x, ctx.layout_offset_y + ctx.cursor_y}, 18, 2, WHITE);
            else DrawText("Toolbar:", ctx.layout_offset_x, ctx.layout_offset_y + ctx.cursor_y, 16, WHITE);
            ctx.cursor_y += 25;
            
            float start_x = ctx.cursor_x;
            float start_y = ctx.cursor_y;
            
            if (qui_image_button(&ctx, &save_icon, 24, 24, 20, 20)) {
                save_clicks++;
            }
            
            ctx.cursor_x = start_x + 35;
            ctx.cursor_y = start_y;
            
            if (qui_image_button(&ctx, &load_icon, 24, 24, 20, 20)) {
                load_clicks++;
            }
            
            ctx.cursor_x = start_x + 70;
            ctx.cursor_y = start_y;
            
            if (qui_image_button(&ctx, &delete_icon, 24, 24, 20, 20)) {
                delete_clicks++;
            }
            
            ctx.cursor_x = start_x;
            ctx.cursor_y = start_y + 35;

            qui_end_window(&ctx);
        }

        qui_end(&ctx);
        
        EndDrawing();
    }
    
    UnloadTexture(save_texture);
    UnloadTexture(load_texture);
    UnloadTexture(delete_texture);
    UnloadTexture(settings_texture);
    if (ctx.font) UnloadFont(*(Font*)ctx.font);
    
    CloseWindow();
    
    return 0;
}
