#include "raylib.h"
#include "storage.h"
#include "ui_manager.h"

int main() {
    loadTasks();

    // for manual window resizing alongside automatic monitor scaling
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // init the graphics context first so monitor queries return valid sizes
    InitWindow(800, 600, "ToDo App");

    // Get the active monitor's resolution and position
    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);
    Vector2 monitorPos = GetMonitorPosition(monitor);

    //  resize to 75% of the monitor and center the window
    int windowWidth = (int)(monitorWidth * 0.75f);
    int windowHeight = (int)(monitorHeight * 0.75f);
    SetWindowSize(windowWidth, windowHeight);
    SetWindowPosition(
        (int)monitorPos.x + (monitorWidth - windowWidth) / 2,
        (int)monitorPos.y + (monitorHeight - windowHeight) / 2
    );

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