#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "storage.h"
#include "ui_manager.h"
#include "asm_engine.h"

void drawUI() {
    static char inputBuffer[64] = "\0";
    static bool editMode = false;
    static int activeFilter = 0; // (0: All, 1: Pending, 2: Completed)
    static bool isAddingTask = false;
    static char selectedChar = '\0';  //for the character-wise filtering

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);  // acts as a global scaling factor for all the Text in our window.. 20px rightnow

    // main title text
    const char* mainTitle = "TO-DO List";
    int titleW = MeasureText(mainTitle, 50);
    DrawText(mainTitle, (screenW - titleW) / 2, 25, 50, RAYWHITE);

    // centered Add Task button
    float addBtnW = 165.0f;
    if (GuiButton(Rectangle{ (screenW - addBtnW) / 2.0f, 90, addBtnW, 38 }, GuiIconText(ICON_FILE_ADD, " Add Task"))) {
        isAddingTask = true;
        editMode = true;
    }

    if (isAddingTask) {
        GuiLock(); // Freeze background controls while modal is open
    }

    //search filtering panel
    const char* subHeading = "Search Filtering";
    int subW = MeasureText(subHeading, 24);
    DrawText(subHeading, (screenW - subW) / 2, 145, 24, RAYWHITE);

    float btnW = 135.0f;
    float btnH = 34.0f;
    float gap = 18.0f;
    float totalBtnsW = (btnW * 3.0f) + (gap * 2.0f);
    float startBtnX = (screenW - totalBtnsW) / 2.0f;
    float btnY = 178.0f;

    bool fAll = (activeFilter == 0);
    bool fPending = (activeFilter == 1);
    bool fCompleted = (activeFilter == 2);

    if (GuiToggle(Rectangle{ startBtnX, btnY, btnW, btnH }, "All", &fAll)) activeFilter = 0;
    if (GuiToggle(Rectangle{ startBtnX + btnW + gap, btnY, btnW, btnH }, "Pending", &fPending)) activeFilter = 1;
    if (GuiToggle(Rectangle{ startBtnX + (btnW + gap) * 2.0f, btnY, btnW, btnH }, "Completed", &fCompleted)) activeFilter = 2;


    // alphabet wise filtering panel 

    float alphaBtnW = 44.0f;
    float alphaBtnH = 26.0f;
    float alphaGap = 4.0f;
    int cols = 14;
    float gridW = (cols * alphaBtnW) + ((cols - 1) * alphaGap);
    float gridStartX = (screenW - gridW) / 2.0f;
    float gridStartY = 220.0f;

    for (int idx = 0; idx < 26; idx++) {
        char letter = 'A' + idx;
        int row = idx / cols;
        int col = idx % cols;

        float bx = gridStartX + col * (alphaBtnW + alphaGap);
        float by = gridStartY + row * (alphaBtnH + alphaGap);

        bool isSelected = (selectedChar == letter);
        const char* label = TextFormat("%c", letter);

        if (GuiToggle(Rectangle{ bx, by, alphaBtnW, alphaBtnH }, label, &isSelected)) {
            selectedChar = isSelected ? letter : '\0';
        }
    }

    // "All" reset button clears the letter filtering
    float clearX = gridStartX + 13 * (alphaBtnW + alphaGap);
    float clearY = gridStartY + (alphaBtnH + alphaGap);
    if (GuiButton(Rectangle{ clearX, clearY, alphaBtnW, alphaBtnH }, "All")) {
        selectedChar = '\0';
    }

    // divider line
    DrawLine(80, 284, screenW - 80, 284, RAYWHITE);

    // status bar for the tasks (has count for active, pending and completed)
    int pendingCount = asm_count_active(taskList, taskCount);
    int totalActive = 0;
    for (int i = 0; i < taskCount; i++) {
        if (taskList[i].isActive) totalActive++;
    }
    int completedCount = totalActive - pendingCount;

    const char* statusText = TextFormat("Total: %d   |   Pending: %d   |   Completed: %d", totalActive, pendingCount, completedCount);
    int statusW = MeasureText(statusText, 22);
    DrawText(statusText, (screenW - statusW) / 2, 294, 22, RAYWHITE);

    // transluncent rectangle window for the task cards
    int panelW = 680;
    int panelX = (screenW - panelW) / 2;
    int panelY = 326;
    int panelH = screenH - panelY - 20; // fills down to y = 775 with a 25px bottom margin

    DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.28f));

    // clips anything that goes past the panel's bottom border so cards never spill over
    BeginScissorMode(panelX, panelY, panelW, panelH);


    int cardW = panelW - 24;
    int cardH = 44;
    int cardX = panelX + 12;
    int drawY = panelY + 12;

    //adding each task's own task card
    for (int i = 0; i < taskCount; i++) {
        if (!taskList[i].isActive) continue;

        if (activeFilter == 1 && taskList[i].isDone) continue;
        if (activeFilter == 2 && !taskList[i].isDone) continue;

        if (selectedChar != '\0') {
            if (!asm_search_char(taskList[i].title, selectedChar) &&
                !asm_search_char(taskList[i].title, selectedChar + 32)) {
                continue;
            }
        }

        Color cardColor = taskList[i].isDone ? Fade(BLACK, 0.25f) : Fade(RAYWHITE, 0.20f);
        Color textColor = taskList[i].isDone ? GRAY : RAYWHITE;

        DrawRectangle(cardX, drawY, cardW, cardH, cardColor);

        GuiCheckBox(Rectangle{ (float)cardX + 12, (float)drawY + 10, 24, 24 }, "", &taskList[i].isDone);

        int taskFontSize = 26;
        DrawText(taskList[i].title, cardX + 48, drawY + 9, taskFontSize, textColor);
        if (taskList[i].isDone) {
            int textW = MeasureText(taskList[i].title, taskFontSize);
            DrawLine(cardX + 48, drawY + 22, cardX + 48 + textW, drawY + 22, GRAY);
        }

        if (GuiButton(Rectangle{ (float)(cardX + cardW - 42), (float)drawY + 7, 32, 30 }, GuiIconText(ICON_CROSS, ""))) {
            asm_clear_buffer(taskList[i].title, 64);
            taskList[i].isDone = false;
            taskList[i].isActive = false;
        }
        drawY += 52;
    }

    EndScissorMode();

    //overlay for adding a task (popup window where user types in the task content)
    if (isAddingTask) {
        GuiUnlock();

        DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.55f));

        float modalW = 460;
        float modalH = 190;
        float modalX = (screenW - modalW) / 2.0f;
        float modalY = (screenH - modalH) / 2.0f;

        DrawRectangleRec(Rectangle{ modalX, modalY, modalW, modalH }, RAYWHITE);
        DrawRectangleLinesEx(Rectangle{ modalX, modalY, modalW, modalH }, 2.0f, DARKGRAY);

        DrawText("Enter New Task", (int)modalX + 20, (int)modalY + 20, 24, DARKGRAY);

        if (GuiTextBox(Rectangle{ modalX + 20, modalY + 60, modalW - 40, 40 }, inputBuffer, 64, editMode)) {
            editMode = !editMode;
        }

        if (GuiButton(Rectangle{ modalX + 20, modalY + 120, 120, 40 }, "Save") || IsKeyPressed(KEY_ENTER)) {
            if (inputBuffer[0] != '\0' && taskCount < MAX_TASKS) {
                asm_copy_string(taskList[taskCount].title, inputBuffer);
                taskList[taskCount].isDone = false;
                taskList[taskCount].isActive = true;
                taskCount++;
            }
            asm_clear_buffer(inputBuffer, 64);
            isAddingTask = false;
            editMode = false;
        }

        if (GuiButton(Rectangle{ modalX + 155, modalY + 120, 120, 40 }, "Cancel") || IsKeyPressed(KEY_ESCAPE)) {
            asm_clear_buffer(inputBuffer, 64);
            isAddingTask = false;
            editMode = false;
        }
    }
}