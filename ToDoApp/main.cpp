#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include "raylib.h"

// Define implementation macro in EXACTLY one file before including raygui
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Test function: Uses 32-bit x86 inline assembly to add two numbers
int test_asm_add(int a, int b) {
#if defined(_M_IX86) && defined(_MSC_VER)
    int result = 0;
    __asm {
        mov eax, a
        add eax, b
        mov result, eax
    }
    return result;
#else
    return a + b;
#endif
}

int main() {
    std::cout << "[boot] Starting COAL ToDo App..." << std::endl;

    int monitorW = GetMonitorWidth(0);
    int monitorH = GetMonitorHeight(0);

    if (monitorW <= 0) monitorW = 1280;
    if (monitorH <= 0) monitorH = 720;

    std::cout << "[boot] Monitor resolution: " << monitorW << " x " << monitorH << std::endl;

    int winW = (int)(monitorW * 0.90f);
    int winH = (int)(monitorH * 0.90f);

    winW = std::max(800, std::min(winW, monitorW - 40));
    winH = std::max(600, std::min(winH, monitorH - 80));

    std::cout << "[boot] Window size: " << winW << " x " << winH << std::endl;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);

    // 1. Initialize Raylib window
    InitWindow(winW, winH, "COAL ToDo App - Base Setup");

    if (!IsWindowReady()) {
        std::cerr << "[fatal] Window creation failed. Update GPU drivers." << std::endl;
        return 1;
    }

    SetWindowMinSize(640, 480);
    SetTargetFPS(60);

    // 2. Execute Assembly Test
    int asm_result = test_asm_add(20, 22);
    std::cout << "[boot] Assembly result: " << asm_result << std::endl;

    while (!WindowShouldClose()) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        float pad  = sw * 0.05f;
        float btnW = sw * 0.60f;
        float btnH = sh * 0.25f;
        float btnX = pad;
        float btnY = pad + 60;

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // 3. Render Assembly Output
        DrawText(TextFormat("Assembly Result (20 + 22): %d", asm_result),
                 (int)pad, (int)pad, 20, DARKGRAY);

        DrawText(TextFormat("Window: %d x %d", sw, sh),
                 (int)pad, (int)pad + 28, 16, GRAY);

        // 4. Render Raygui Widget Test
        if (GuiButton(Rectangle{ btnX, btnY, btnW, btnH },
                      GuiIconText(ICON_FILE_ADD, " Add Task Test"))) {
            std::cout << "Raygui button clicked successfully!" << std::endl;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
