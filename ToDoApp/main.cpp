#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "raylib.h"

// Define implementation macro in EXACTLY one file before including raygui
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Test function: Uses 32-bit x86 inline assembly to add two numbers
int test_asm_add(int a, int b) {
    int result = 0;
    __asm {
        mov eax, a
        add eax, b
        mov result, eax
    }
    return result;
}

int main() {
    // 1. Initialize Raylib window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1800, 1800, "COAL ToDo App - Base Setup");
    SetTargetFPS(60);

    // 2. Execute Assembly Test
    int asm_result = test_asm_add(20, 22);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // 3. Render Assembly Output
        DrawText(TextFormat("Assembly Result (20 + 22): %d", asm_result), 50, 50, 20, DARKGRAY);

        // 4. Render Raygui Widget Test
        if (GuiButton(Rectangle{ 50, 100, 500, 400}, GuiIconText(ICON_FILE_ADD, " Add Task Test"))) {
            std::cout << "Raygui button clicked successfully!" << std::endl;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}