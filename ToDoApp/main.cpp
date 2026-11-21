#include "raylib.h"
#include "storage.h"
#include "ui_manager.h"

int main() {
    loadTasks();
    InitWindow(1000, 800, "ToDo App");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGREEN);

        drawUI();

        EndDrawing();
    }

    saveTasks();

    CloseWindow();
    return 0;
}