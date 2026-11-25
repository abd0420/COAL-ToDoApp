#include "raylib.h"
#include "storage.h"
#include "ui_manager.h"
#include "asm_engine.h"
#include <string>
#include <cstring>
#include <algorithm>
using namespace std;

// fonts & Colors
static Font fontTitle, fontBody;

static const float CORNER_RADIUS = 10.0f;

static const Color PANEL = { 19, 31, 28, 245 };
static const Color CARD = { 40, 59, 51, 255 };
static const Color HOVER = { 54, 82, 68, 255 };
static const Color ACCENT = { 118, 221, 179, 255 };
static const Color WHITE2 = { 239, 245, 236, 255 };
static const Color MUTED = { 160, 185, 171, 255 };
static const Color BORDER = { 67, 91, 77, 255 };

static float radiusToRoundness(Rectangle r, float radiusPx) {
    float minSide = std::min(r.width, r.height);
    if (minSide <= 0.0f) return 0.0f;
    float roundness = (radiusPx * 2.0f) / minSide;
    return std::clamp(roundness, 0.0f, 1.0f);
}

static void drawBox(Rectangle r, Color bg, bool outlined = false,
    Color outlineColor = BORDER, float radiusPx = CORNER_RADIUS, int segments = 8) {
    float rnd = radiusToRoundness(r, radiusPx);
    DrawRectangleRounded(r, rnd, segments, bg);
    if (outlined) {
        DrawRectangleRoundedLinesEx(r, rnd, segments, 1, outlineColor);
    }
}

static void text(Font f, const char* s, float x, float y,
    float size, Color c) {
    DrawTextEx(f, s, { x, y }, size, 0.5f, c);
}

static void centered(Font f, const char* s, Rectangle r,
    float size, Color c) {
    Vector2 m = MeasureTextEx(f, s, size, 0.5f);
    text(f, s, r.x + (r.width - m.x) / 2,
        r.y + (r.height - m.y) / 2, size, c);
}

static bool button(Rectangle r, const char* label,
    bool selected = false, bool primary = false,
    bool enabled = true) {
    bool hover = enabled &&
        CheckCollisionPointRec(GetMousePosition(), r);

    Color bg = selected || primary ? ACCENT : (hover ? HOVER : CARD);
    Color fg = selected || primary ? PANEL : WHITE2;

    drawBox(r, bg, !selected && !primary, BORDER, CORNER_RADIUS, 8);

    centered(fontBody, label, r, 23, fg);

    return enabled && hover &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static bool matchesFilter(int i, int filter, char letter) {
    if (!taskList[i].isActive) return false;
    if (filter == 1 && taskList[i].isDone) return false;
    if (filter == 2 && !taskList[i].isDone) return false;

    // asm_functions called here below
    if (letter != '\0') {
        if (!asm_search_char(taskList[i].title, letter) &&
            !asm_search_char(taskList[i].title, letter + 32))
            return false;
    }
    return true;
}

void initUIAssets() {
    // headings font 
    fontTitle = LoadFontEx(
        "assets/Hello Baby.otf", 80, nullptr, 0);

    // overall interface font
    fontBody = LoadFontEx(
        "assets/Internet Friends.ttf", 48, nullptr, 0);

    SetTextureFilter(fontTitle.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(fontBody.texture, TEXTURE_FILTER_BILINEAR);
}

void unloadUIAssets() {
    UnloadFont(fontTitle);
    UnloadFont(fontBody);
}

void drawUI() {
    static char inputBuffer[64] = ""; //array where the user's input gets stored
    static int activeFilter = 0;
    static char selectedChar = '\0';
    static bool isAddingTask = false;
    static float scroll = 0;

    int W = GetScreenWidth();
    int H = GetScreenHeight();

    // layout of our interface
    float margin = 24;
    float gap = 18;
    float sideW = 342;

    Rectangle sidebar = {
        margin, margin, sideW, (float)H - margin * 2
    };

    Rectangle workspace = {
        margin + sideW + gap,
        margin,
        W - sideW - gap - margin * 2,
        H - margin * 2.0f
    };

    // Background panels
    drawBox(sidebar, PANEL, false, BORDER, CORNER_RADIUS, 8);
    drawBox(workspace, PANEL, false, BORDER, CORNER_RADIUS, 8);

    // ==================================================
    // LEFT SIDEBAR
    // ==================================================

    float sx = sidebar.x + 22;
    float sy = sidebar.y + 25;

    text(fontTitle, "TO DO", sx, sy + 19, 47, WHITE2);
    text(fontBody, "Organize your day.", sx, sy + 79, 18, MUTED);

    DrawLine(
        (int)sx, (int)(sy + 120),
        (int)(sidebar.x + sideW - 22),
        (int)(sy + 120), BORDER
    );

    // status wise filtering 
    text(fontTitle, "FILTER TASKS", sx, sy + 140, 25, WHITE2);

    float fw = 94;
    float fy = sy + 185;

    if (button({ sx, fy, fw, 40 }, "All",
        activeFilter == 0, false, !isAddingTask))
        activeFilter = 0;

    if (button({ sx + 103, fy, fw, 40 }, "Pending",
        activeFilter == 1, false, !isAddingTask))
        activeFilter = 1;

    if (button({ sx + 206, fy, fw, 40 }, "Done",
        activeFilter == 2, false, !isAddingTask))
        activeFilter = 2;

    // alphabetical filtering
    text(fontTitle, "FIND BY LETTER",
        sx, sy + 250, 25, WHITE2);

    float aw = 39;
    float ah = 33;
    float agap = 5;
    float ax = sx;
    float ay = sy + 295;

    for (int i = 0; i < 26; i++) {
        int col = i % 7;
        int row = i / 7;

        char letter = 'A' + i;
        char label[2] = { letter, '\0' };

        Rectangle r = {
            ax + col * (aw + agap),
            ay + row * (ah + agap),
            aw, ah
        };

        if (button(r, label, selectedChar == letter,
            false, !isAddingTask)) {
            selectedChar =
                (selectedChar == letter) ? '\0' : letter;
            scroll = 0;
        }
    }

    if (button({ sx, ay + 160, 303, 36 },
        "Clear letter filter", false, false,
        !isAddingTask)) {
        selectedChar = '\0';
        scroll = 0;
    }

    // ==================================================
    // RIGHT WORKSPACE
    // ==================================================

    float rx = workspace.x + 25;
    float rw = workspace.width - 50;
    float ry = workspace.y + 25;

    text(fontTitle, "My Tasks", rx, ry + 21, 40, WHITE2);

    // Add task button
    float addW = 155;
    Rectangle addRect = {
        workspace.x + workspace.width - addW - 25,
        ry + 20, addW, 43
    };

    if (button(addRect, "+ Add Task", false, true,
        !isAddingTask)) {
        isAddingTask = true;
        asm_clear_buffer(inputBuffer, 64);  // ASM FUNCTION CALLED
    }

    // tasks count
    int pendingCount = asm_count_active(taskList, taskCount); //ASM FUNCTION CALLED
    int completedCount = asm_count_completed(taskList, taskCount); //ASM FUNCTION CALLED
    int totalActive = 0;

    for (int i = 0; i < taskCount; i++) {
        if (taskList[i].isActive) totalActive++;
    }

    // stats for the count of tasks
    float statGap = 12;
    float statW = (rw - statGap * 2) / 3;
    float statY = ry + 96;

    int values[3] = {
        totalActive, pendingCount, completedCount
    };

    const char* statNames[3] = {
        "TOTAL", "PENDING", "COMPLETED"
    };

    for (int i = 0; i < 3; i++) {
        Rectangle r = {
            rx + i * (statW + statGap),
            statY, statW, 78
        };

        drawBox(r, CARD, false, BORDER, CORNER_RADIUS, 8);

        const char* value = TextFormat("%d", values[i]);

        text(fontTitle, value,
            r.x + 15, r.y + 8, 32,
            i == 1 ? ACCENT : WHITE2);

        text(fontBody, statNames[i],
            r.x + 15, r.y + 51, 15, MUTED);
    }

    // Task list heading
    float headingY = statY + 100;

    text(fontTitle, "YOUR TASKS",
        rx, headingY - 4, 27, WHITE2);

    const char* shown = TextFormat("%d showing", totalActive);

    text(fontBody, shown,
        rx + rw - MeasureTextEx(fontBody, shown, 13, 0.5f).x,
        headingY + 4, 16, MUTED);

    DrawLine(
        (int)rx, (int)(headingY + 35),
        (int)(rx + rw), (int)(headingY + 35),
        BORDER
    );

    // Scrollable list region
    Rectangle listArea = {
        rx,
        headingY + 48,
        rw,
        workspace.y + workspace.height - (headingY + 48) - 20
    };

    int visibleCount = 0;

    for (int i = 0; i < taskCount; i++) {
        if (matchesFilter(i, activeFilter, selectedChar))
            visibleCount++;
    }

    float itemHeight = 65;
    float contentHeight = visibleCount * itemHeight;
    float maxScroll = std::max(
        0.0f, contentHeight - listArea.height
    );

    bool mouseInList =
        CheckCollisionPointRec(GetMousePosition(), listArea);

    if (mouseInList && !isAddingTask) {
        scroll -= GetMouseWheelMove() * 35;
    }

    if (scroll < 0) scroll = 0;
    if (scroll > maxScroll) scroll = maxScroll;

    // Empty state
    if (visibleCount == 0) {
        const char* emptyTitle = totalActive == 0
            ? "No tasks yet"
            : "No matching tasks";

        const char* emptyDesc = totalActive == 0
            ? "Add your first task to get started."
            : "Try changing your filters.";

        Vector2 m1 = MeasureTextEx(
            fontTitle, emptyTitle, 28, 0.5f);

        Vector2 m2 = MeasureTextEx(
            fontBody, emptyDesc, 18, 0.5f);

        float cy = listArea.y + listArea.height / 2;

        text(fontTitle, emptyTitle,
            listArea.x + (rw - m1.x) / 2,
            cy - 29, 28, WHITE2);

        text(fontBody, emptyDesc,
            listArea.x + (rw - m2.x) / 2,
            cy + 16, 18, MUTED);
    }

    BeginScissorMode(
        (int)listArea.x, (int)listArea.y,
        (int)listArea.width, (int)listArea.height
    );

    int rowNumber = 0;

    for (int i = 0; i < taskCount; i++) {
        if (!matchesFilter(i, activeFilter, selectedChar))
            continue;

        float cardY =
            listArea.y + rowNumber * itemHeight - scroll;

        rowNumber++;

        if (cardY + 56 < listArea.y ||
            cardY > listArea.y + listArea.height)
            continue;

        Rectangle card = {
            rx, cardY, rw, 56
        };

        bool hovered = mouseInList && !isAddingTask &&
            CheckCollisionPointRec(GetMousePosition(), card);

        drawBox(card, hovered ? HOVER : CARD, false, BORDER, CORNER_RADIUS, 8);

        // checkbox drawings
        Rectangle checkRect = {
            rx + 14, cardY + 16, 23, 23
        };

        drawBox(checkRect,
            taskList[i].isDone ? ACCENT : PANEL,
            true, BORDER, std::min(CORNER_RADIUS, 6.0f), 8);

        if (taskList[i].isDone) {
            centered(fontBody, "X",
                checkRect, 15, PANEL);
        }

        if (hovered &&
            CheckCollisionPointRec(
                GetMousePosition(), checkRect) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            taskList[i].isDone = !taskList[i].isDone;
        }

        // Task text, truncated to fit
        string display = taskList[i].title;
        float availableW = rw - 140;

        if (MeasureTextEx(fontBody, display.c_str(),
            30, 0.5f).x > availableW) {
            while (!display.empty() &&
                MeasureTextEx(fontBody,
                    (display + "...").c_str(),
                    20, 0.5f).x > availableW) {
                display.pop_back();
            }
            display += "...";
        }

        text(fontBody, display.c_str(),
            rx + 52, cardY + 16, 20,
            taskList[i].isDone ? MUTED : WHITE2);

        // Delete button "X"
        Rectangle deleteRect = {
            rx + rw - 42, cardY + 12, 31, 31
        };

        bool deleteHover = hovered &&
            CheckCollisionPointRec(
                GetMousePosition(), deleteRect);

        drawBox(deleteRect,
            deleteHover ? Color{ 145, 65, 65, 255 } : PANEL,
            false, BORDER, std::min(CORNER_RADIUS, 8.0f), 8);

        centered(fontBody, "X", deleteRect,
            15, deleteHover ? WHITE2 : MUTED);

        if (deleteHover &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            asm_clear_buffer(taskList[i].title, 64); //ASM FUNCTION CALLED
            taskList[i].isActive = false;
            taskList[i].isDone = false;
        }
    }

    EndScissorMode();

    // Scrollbar
    if (maxScroll > 0 && listArea.height > 0) {
        float barH = listArea.height *
            (listArea.height / contentHeight);

        if (barH < 25) barH = 25;

        float barY = listArea.y +
            (scroll / maxScroll) *
            (listArea.height - barH);

        Rectangle scrollBar = {
            listArea.x + listArea.width - 4, barY, 4, barH
        };
        drawBox(scrollBar, ACCENT, false, BORDER, 2.0f, 6);
    }

    // ==================================================
    // ADD TASK Panel
    // ==================================================

    if (isAddingTask) {
        DrawRectangle(
            0, 0, W, H, Fade(BLACK, 0.75f)
        );

        float mw = 480;
        float mh = 230;
        float mx = (W - mw) / 2;
        float my = (H - mh) / 2;

        Rectangle modal = { mx, my, mw, mh };

        drawBox(modal, PANEL, true, BORDER, CORNER_RADIUS, 8);
        DrawRectangleRoundedLinesEx(
            modal, radiusToRoundness(modal, CORNER_RADIUS), 8, 2, BORDER
        );

        text(fontTitle, "Create a new task",
            mx + 25, my + 18, 30, WHITE2);

        text(fontBody, "TASK NAME",
            mx + 25, my + 65, 16, MUTED);

        Rectangle input = {
            mx + 25, my + 90, mw - 50, 46
        };

        drawBox(input, CARD, true, ACCENT, CORNER_RADIUS, 8);

        // Keyboard text input
        int key = GetCharPressed();

        while (key > 0) {
            int len = (int)strlen(inputBuffer);

            if (key >= 32 && key <= 126 && len < 63) {
                inputBuffer[len] = (char)key;
                inputBuffer[len + 1] = '\0';
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = (int)strlen(inputBuffer);

            if (len > 0)
                inputBuffer[len - 1] = '\0';
        }

        if (inputBuffer[0] == '\0') {
            text(fontBody, "What needs to be done?",
                input.x + 14, input.y + 12,
                17, MUTED);
        }
        else {
            // Keep the ending visible for long input
            string entry = inputBuffer;

            while (!entry.empty() &&
                MeasureTextEx(fontBody, entry.c_str(),
                    17, 0.5f).x > input.width - 36) {
                entry.erase(0, 1);
            }

            text(fontBody, entry.c_str(),
                input.x + 14, input.y + 12,
                17, WHITE2);
        }

        // task saving & cancel functionality
        bool saveClicked = button(
            { mx + mw - 165, my + 164, 140, 42 },
            "Save Task", false, true
        );

        bool cancelClicked = button(
            { mx + 25, my + 164, 115, 42 },
            "Cancel"
        );

        if (saveClicked || IsKeyPressed(KEY_ENTER)) {
            if (inputBuffer[0] != '\0') {

                // reuses a deleted task slot
                int slot = -1;

                for (int i = 0; i < taskCount; i++) {
                    if (!taskList[i].isActive) {
                        slot = i;
                        break;
                    }
                }

                if (slot == -1 && taskCount < MAX_TASKS)
                    slot = taskCount++;

                if (slot != -1) {
                    asm_copy_string(
                        taskList[slot].title, inputBuffer
                    );

                    taskList[slot].isDone = false;
                    taskList[slot].isActive = true;
                }
            }

            asm_clear_buffer(inputBuffer, 64);
            isAddingTask = false;
        }

        if (cancelClicked || IsKeyPressed(KEY_ESCAPE)) {
            asm_clear_buffer(inputBuffer, 64);
            isAddingTask = false;
        }
    }
}