// RAYLIB_EXAMPLE.c

#include <raylib.h>
#include "quickui.h"
#include <stdio.h>
#include <string.h>

// BACKEND IMPLEMNTATION
void raylib_draw_rect(qui_Context* ctx, float x, float y, float w, float h, qui_Color col) {
    Color raylib_color = { col.r, col.g, col.b, col.a };
    DrawRectangle((int)x, (int)y, (int)w, (int)h, raylib_color);
}

void raylib_draw_text(qui_Context* ctx, const char *text, float x, float y) {
    Font *font = (Font *)ctx->font;
    Color font_color = GRAY;
    if (!font){
        DrawText(text, (int)x, (int)y, 20, font_color);
    } 
    else{
        DrawTextEx(*font, text, (Vector2){x, y}, ctx->font_size,ctx->font_spacing, font_color);
    }
}

float raylib_text_width(qui_Context* ctx, const char *text) {
    return (float)MeasureText(text, 20);
}

float raylib_text_height(qui_Context* ctx, const char *text) {
    return 20.0f;
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "QuickUI + Raylib Example");
    SetTargetFPS(60);

    Font font = LoadFont("Iosevka-Regular.ttf");
    // QuickUI context
    qui_Context ui_ctx;
    qui_init(&ui_ctx, NULL);
    qui_set_font(&ui_ctx, &font, 20, 2);
    
    ui_ctx.draw_rect = raylib_draw_rect;
    ui_ctx.draw_text = raylib_draw_text;
    ui_ctx.text_width = raylib_text_width;
    ui_ctx.text_height = raylib_text_height;
    
    // Application state
    int checkbox_value = 0;
    float slider_value = 50.0f;
    char textbox_buffer[256] = "Type here...";
    int button_clicks = 0;
    
    while (!WindowShouldClose()) {

        Vector2 mouse_pos = GetMousePosition();
        qui_mouse_move(&ui_ctx, (int)mouse_pos.x, (int)mouse_pos.y);
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            qui_mouse_down(&ui_ctx, (int)mouse_pos.x, (int)mouse_pos.y);
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            qui_mouse_up(&ui_ctx, (int)mouse_pos.x, (int)mouse_pos.y);
        }
        
        if (ui_ctx.keyboard_focus_id > 0) {

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
                qui_feed_key_backspace(&ui_ctx);
            }
            
            if (IsKeyPressed(KEY_ENTER)) {
                qui_feed_key_enter(&ui_ctx);
            }
        }
        
        BeginDrawing();
        ClearBackground(CLITERAL(Color) {0x18, 0x18, 0x18, 0xFF});
        
        qui_begin(&ui_ctx, 50.0f, 50.0f);
        
        Vector2 position = {50 , 20 };
        float size = 30;
        float spacing = 2;
        DrawTextEx(font,"QuickUI + Raylib Demo" , position, size, spacing, RAYWHITE);
        
        if (qui_button(&ui_ctx, "Click Me!")) {
            button_clicks++;
        }

        char button_info[64];
        snprintf(button_info, sizeof(button_info), "Button clicked: %d times", button_clicks);
        DrawTextEx(font, button_info, (Vector2) {50, (int)ui_ctx.cursor_y}, size, spacing, LIGHTGRAY);
        ui_ctx.cursor_y += 25;
        
        qui_checkbox(&ui_ctx, "Enable Feature", &checkbox_value);
        
        char checkbox_info[64];
        snprintf(checkbox_info, sizeof(checkbox_info), "Checkbox is: %s", checkbox_value ? "ON" : "OFF");
        DrawTextEx(font, checkbox_info, (Vector2) {50, (int)ui_ctx.cursor_y}, size, spacing, LIGHTGRAY);
        ui_ctx.cursor_y += 25;
        
        qui_slider_float(&ui_ctx, "Volume", &slider_value, 0.0f, 100.0f, 200.0f);
        
        int focused = qui_textbox(&ui_ctx, textbox_buffer, sizeof(textbox_buffer), 300.0f);
        
        char textbox_info[64];
        snprintf(textbox_info, sizeof(textbox_info), "Text input %s", focused ? "(focused)" : "");
        DrawTextEx(font, textbox_info, (Vector2) {50, (int)ui_ctx.cursor_y}, size, spacing, LIGHTGRAY);
        ui_ctx.cursor_y += 25;
        
        char content_info[512];
        snprintf(content_info, sizeof(content_info), "Content: \"%s\"", textbox_buffer);
        DrawTextEx(font, content_info, (Vector2) {50, (int)ui_ctx.cursor_y}, size, spacing, YELLOW);
        ui_ctx.cursor_y += 30;
        char debug_info[256];

        snprintf(debug_info, sizeof(debug_info), 
            "Mouse: (%d, %d) | Active ID: %u | Hot ID: %u | Focus ID: %u",
            (int)mouse_pos.x, (int)mouse_pos.y, 
            ui_ctx.active_id, ui_ctx.hot_id, ui_ctx.keyboard_focus_id);
            DrawTextEx(font, debug_info, (Vector2) {10, screenHeight - 25}, size - 2, 2, BLUE);
        
        qui_end(&ui_ctx);
        
        EndDrawing();
    }
    CloseWindow();
    
    return 0;
}
