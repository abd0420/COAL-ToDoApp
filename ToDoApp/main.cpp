#include "raylib.h"
#include "storage.h"
#include "ui_manager.h"

int main() {
    loadTasks();

    //window resizeability and centering as per user's screen size

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "ToDo App");
    SetExitKey(KEY_NULL);

    SetWindowMinSize(900, 600);

    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);
    Vector2 monitorPos = GetMonitorPosition(monitor);

    int windowWidth = (int)(monitorWidth * 0.75f);
    int windowHeight = (int)(monitorHeight * 0.75f);

    if (windowWidth < 750) windowWidth = 750;
    if (windowHeight < 600) windowHeight = 600;

    SetWindowSize(windowWidth, windowHeight);
    SetWindowPosition(
        (int)monitorPos.x + (monitorWidth - windowWidth) / 2,
        (int)monitorPos.y + (monitorHeight - windowHeight) / 2
    );

    SetTargetFPS(60);

    // loads background and fonts
    Texture2D background = LoadTexture("assets/background.png");
    initUIAssets();

    //frame loop

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // helps in drawing the  background without stretching
        if (IsTextureValid(background)) {
            float scaleX = (float)screenW / background.width;
            float scaleY = (float)screenH / background.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;

            float sourceW = screenW / scale;
            float sourceH = screenH / scale;

            Rectangle source = {
                (background.width - sourceW) / 2.0f,
                (background.height - sourceH) / 2.0f,
                sourceW,
                sourceH
            };

            Rectangle destination = {
                0, 0, (float)screenW, (float)screenH
            };

            DrawTexturePro(
                background,
                source,
                destination,
                Vector2{ 0, 0 },
                0,
                WHITE
            );
        }

        // Dark overlay for readability
        DrawRectangle(
            0, 0, screenW, screenH,
            Fade(BLACK, 0.48f)
        );

        drawUI(); //for all the UI heavy Lifting

        EndDrawing();
    }

    saveTasks(); //saves the updated tasks list to the file "tasks.txt"

    unloadUIAssets();
    UnloadTexture(background);

    CloseWindow();
    return 0;
}