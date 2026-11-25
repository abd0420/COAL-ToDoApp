# COAL-ToDoApp

A native Windows desktop to-do manager written in C++ with a graphical interface rendered by [raylib](https://www.raylib.com/). Its core data operations (string copy, buffer clearing, searching, trimming, and task counting) are implemented in hand-written **x86 inline assembly** (`__asm`) for a Computer Organization and Assembly Language (COAL) course project.

---

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Technology Stack](#technology-stack)
4. [Quick Start](#quick-start)
5. [Building from Source](#building-from-source)
6. [Running the Application](#running-the-application)
7. [User Guide](#user-guide)
8. [Data Persistence](#data-persistence)
9. [Architecture](#architecture)
10. [Assembly Engine Reference](#assembly-engine-reference)
11. [Repository Structure](#repository-structure)
12. [Troubleshooting](#troubleshooting)
13. [Known Limitations](#known-limitations)
14. [Contributing](#contributing)

---

## Overview

COAL-ToDoApp keeps up to 20 tasks in a fixed-size in-memory array. Tasks are loaded from a plain-text file at startup, edited through a custom-drawn raylib interface, and written back to disk when the application closes.

The project's purpose is to show how a conventional application can delegate well-defined, performance-sensitive primitives to assembly while C++ handles windowing, input, layout, and file I/O. Every assembly routine operates directly on the memory layout of the `Task` struct, using explicit byte offsets and a fixed 66-byte stride.

---

## Features

**Task management**
- Add tasks through a modal dialog (up to 63 printable ASCII characters).
- Mark tasks as done or pending with a checkbox.
- Delete tasks. A deleted slot is zeroed with assembly and reused by the next task you add.

**Filtering and search**
- Status filter: `All`, `Pending`, `Done`.
- Letter filter: an A-Z grid that shows only tasks whose title contains the chosen letter (case-insensitive). Status and letter filters combine.

**Interface**
- Live counters for total, pending, and completed tasks, plus a "N showing" count for the current view.
- Mouse-wheel scrolling in the task list.
- Resizable window, sized to 75% of the current monitor and centered on launch.
- Custom fonts, a cover-fit background image with a readability overlay, and consistent rounded corners across all components.

---

## Technology Stack

| Component        | Details                                                          |
|------------------|------------------------------------------------------------------|
| Language         | C++20 (`/std:c++20`) with MSVC inline assembly (`__asm`)         |
| Graphics / input | raylib 6.0, linked statically via the bundled `raylib.lib`       |
| Toolchain        | Visual Studio, MSVC platform toolset `v145`                      |
| Target           | Windows, **x86 (Win32) only**                                    |
| Storage          | Plain-text file (`tasks.txt`), no external database              |

> **Why Win32 only?** MSVC supports the `__asm` keyword on x86 only. It is not available when compiling for x64 or ARM64, and the bundled raylib binaries are 32-bit. Always build the **Win32** platform.

---

## Quick Start

```bat
git clone https://github.com/<your-username>/COAL-ToDoApp.git
cd COAL-ToDoApp\ToDoApp
start ToDoApp.slnx
```

In Visual Studio, select **Debug** (or **Release**) and **x86** in the toolbar, then press **Ctrl+F5**.

---

## Building from Source

### Prerequisites

| Requirement | Notes |
|-------------|-------|
| Windows 10 or 11 | The application links against Win32 system libraries. |
| Visual Studio with the **Desktop development with C++** workload | Includes MSVC and the Windows SDK. |
| Git | To clone the repository. |

All third-party dependencies (raylib headers and libraries) are vendored in `ToDoApp/vendor/raylib/`. No package manager or separate install is needed.

The project file specifies platform toolset `v145`. If you use an older Visual Studio, see [Troubleshooting](#toolset-v145-not-found-msb8020).

### Option A: Visual Studio (recommended)

1. Clone the repository.
2. Open `ToDoApp/ToDoApp.slnx` (or `ToDoApp/ToDoApp.vcxproj`) in Visual Studio.
3. In the toolbar, set the configuration to **Debug** or **Release** and the platform to **x86**.
4. Build with **Ctrl+Shift+B**.
5. Run with **Ctrl+F5** (without debugging) or **F5** (with debugger).

Visual Studio sets the working directory to the project folder (`ToDoApp/`) by default, which is exactly what the application needs to find its assets and `tasks.txt`.

### Option B: Command line (MSBuild)

Open the **x86 Native Tools Command Prompt for VS** (or **Developer Command Prompt**) and run:

```bat
cd COAL-ToDoApp\ToDoApp
msbuild ToDoApp.vcxproj /p:Configuration=Release /p:Platform=Win32
```

Replace `Release` with `Debug` for a debug build. The executable is written to the project's output directory (typically `ToDoApp\Release\` or `ToDoApp\Debug\`); MSBuild prints the exact path at the end of the build.

### Build configuration summary

| Setting              | Value                                                          |
|----------------------|----------------------------------------------------------------|
| Include path         | `vendor\raylib\include` and the project directory              |
| Library path         | `vendor\raylib\lib`                                            |
| Linked libraries     | `raylib.lib`, `winmm.lib`, `gdi32.lib`, `user32.lib`, `shell32.lib` |
| Subsystem            | Console (a console window opens alongside the app window)      |
| Language standard    | C++20                                                          |
| Supported platform   | Win32 (x86). The x64 configurations are not usable.            |

---

## Running the Application

The program loads `assets/background.png`, both font files, and `tasks.txt` using **relative paths**. The **working directory must be the `ToDoApp/` project folder**, the one that contains the `assets/` directory.

**From Visual Studio:** press Ctrl+F5. The working directory is already correct.

**From the command line:**

```bat
cd COAL-ToDoApp\ToDoApp
Release\ToDoApp.exe
```

**From File Explorer:** double-clicking the `.exe` inside `Debug\` or `Release\` sets the wrong working directory. The window will open without its background or custom fonts. Either launch from the command line as above, or copy the `assets` folder next to the executable.

On first launch there is no `tasks.txt`, so the list starts empty. The file is created automatically when you close the application.

---

## User Guide

### Controls

| Action                    | How                                                            |
|---------------------------|----------------------------------------------------------------|
| Create a task             | Click **+ Add Task**, type a name, press **Enter** or click **Save Task** |
| Cancel task creation      | Press **Esc** or click **Cancel**                              |
| Mark done / pending       | Click the checkbox on the task card                            |
| Delete a task             | Click the **X** button on the task card                        |
| Filter by status          | Click **All**, **Pending**, or **Done** in the sidebar         |
| Filter by letter          | Click a letter in the A-Z grid                                 |
| Clear the letter filter   | Click **Clear letter filter**                                  |
| Scroll the list           | Mouse wheel while the pointer is over the task list            |
| Save and quit             | Close the window with the title-bar close button               |

### Notes on behavior

- Task names accept printable ASCII characters (codes 32-126) and are limited to 63 characters.
- Empty names are ignored.
- Long titles are shortened with an ellipsis in the list view; the full text is still stored.
- While the Add Task dialog is open, the rest of the interface and list scrolling are disabled.
- The window has a minimum size of 900 x 600.
- **Esc does not quit the application.** It only dismisses the dialog.

---

## Data Persistence

Tasks are stored in `tasks.txt` in the working directory, one task per line:

```text
<status>|<title>
```

| Field    | Meaning                          |
|----------|----------------------------------|
| `status` | `1` = done, `0` = pending        |
| `title`  | Task name (up to 63 characters)  |

Example:

```text
0|Finish COAL assignment
1|Submit lab report
0|Review x86 string instructions
```

**Lifecycle**
- `loadTasks()` runs before the window opens. Malformed lines (shorter than 3 characters or without `|` in the second position) are skipped, and loading stops after 20 tasks.
- `saveTasks()` runs once, when the main loop exits normally. Deleted tasks are not written, so the file is compacted on every save.
- Changes are **not** written continuously. If the process is killed or crashes, changes since launch are lost.
- `tasks.txt` is listed in `.gitignore`, so personal task data is never committed. To reset the app, delete the file.

---

## Architecture

```text
 main.cpp ──► window setup, background rendering, frame loop, save on exit
    │
    ├──► storage.cpp ──► Task array, tasks.txt load / save
    │
    └──► ui_manager.cpp ──► sidebar, task list, modal, filters, input
              │
              └──► asm_engine.cpp ──► x86 __asm primitives
```

| Module              | Responsibility                                                                 |
|---------------------|--------------------------------------------------------------------------------|
| `main.cpp`          | Initializes raylib, sizes and centers the window, draws the cover-fit background and overlay, runs the frame loop, and calls `saveTasks()` on exit. |
| `storage.h/.cpp`    | Defines `Task`, `MAX_TASKS`, the global `taskList` / `taskCount`, and file I/O. |
| `ui_manager.h/.cpp` | Immediate-mode rendering and interaction: sidebar, filters, stat cards, scrollable list, and the add-task modal. |
| `asm_engine.h/.cpp` | The inline assembly routines described below.                                   |

### Task memory layout

```cpp
struct Task {
    char title[64];   // offset +0
    bool isDone;      // offset +64
    bool isActive;    // offset +65
};                    // sizeof(Task) == 66
```

All fields are single bytes, so the struct has no padding and the array stride is exactly 66 bytes. The assembly code relies on these constants directly.

### Slot management

`isActive` marks whether a slot holds a live task. Deleting a task clears its title and sets `isActive = false`; `taskCount` does not shrink. When adding a task, the first inactive slot is reused before a new slot is appended, up to `MAX_TASKS` (20).

---

## Assembly Engine Reference

All routines are defined in `asm_engine.cpp` and declared in `asm_engine.h`. Each guards against null pointers, saves and restores the registers it touches, and respects the 64-byte title capacity.

| Routine | Purpose | Key instructions | Used by |
|---------|---------|------------------|---------|
| `asm_copy_string(dest, src)` | Copies a string, capped at 63 characters, and null-terminates it. | Length scan loop, `rep movsb`, `cld` | Saving a new task |
| `asm_clear_buffer(dest, length)` | Zeroes a buffer: 4 bytes at a time, then the 0-3 byte tail. | `rep stosd`, `rep stosb` | Deleting a task; resetting the input buffer |
| `asm_count_active(list, n)` | Counts tasks that are active and not done. | `cmp byte ptr [esi+65]`, `[esi+64]`, stride `add esi, 66` | PENDING counter |
| `asm_count_completed(list, n)` | Counts tasks that are active and done. | `movzx`, offsets +64 / +65, stride 66 | COMPLETED counter |
| `asm_search_char(str, c)` | Returns whether a character occurs in a title (bounded to 64 bytes). | `repne scasb` (twice: find terminator, then search) | A-Z letter filter |
| `asm_trim_string(str)` | Trims leading and trailing spaces and tabs in place and returns the new length. | Byte comparisons, `rep movsb` to shift text left | Not currently called from the UI |

**Notes**
- The letter filter calls `asm_search_char` twice per task, once for the uppercase letter and once for `letter + 32` (lowercase), which makes the match case-insensitive.
- `asm_trim_string` is implemented and declared but is not yet wired into the task-creation flow. Titles are currently stored exactly as typed, including leading or trailing spaces.

---

## Repository Structure

```text
COAL-ToDoApp/
├── README.md
├── .gitignore
├── .gitattributes
└── ToDoApp/
    ├── ToDoApp.slnx                 # Visual Studio solution
    ├── ToDoApp.vcxproj              # Project: toolset, include/lib paths, link settings
    ├── ToDoApp.vcxproj.filters
    ├── ToDoApp.vcxproj.user
    ├── main.cpp                     # Entry point, window, background, frame loop
    ├── storage.h / storage.cpp      # Task struct, globals, tasks.txt I/O
    ├── ui_manager.h / ui_manager.cpp# All UI drawing and interaction
    ├── asm_engine.h / asm_engine.cpp# x86 inline assembly routines
    ├── raygui.h                     # raygui header (bundled; not used by current code)
    ├── assets/
    │   ├── background.png           # Background image
    │   ├── Hello Baby.otf           # Heading font
    │   └── Internet Friends.ttf     # Body / UI font
    └── vendor/
        └── raylib/
            ├── include/             # raylib.h, raymath.h, rlgl.h
            └── lib/                 # raylib.lib (static), raylibdll.lib, raylib.dll (32-bit)
```

`tasks.txt` is generated at runtime and is not part of the repository.

---

## Troubleshooting

### Build problems

**`__asm` is not supported on this architecture / C4235 / x64 errors**
You have an x64 platform selected. Inline assembly is x86-only. Change the platform dropdown to **x86** (Win32). The x64 configurations in the project are not set up with raylib and cannot compile the assembly routines.

**`Cannot open include file: 'raylib.h'`**
The include path is configured only for the Win32 configurations. Confirm the platform is **x86**, that you opened `ToDoApp.vcxproj` or `ToDoApp.slnx` (not just the folder), and that `ToDoApp/vendor/raylib/include/raylib.h` exists. If the `vendor` folder is missing, re-clone the repository.

**`LNK1104` or unresolved externals such as `InitWindow`, `DrawTexturePro`**
The linker cannot find or use `raylib.lib`. Check that the platform is **x86** and that `ToDoApp/vendor/raylib/lib/raylib.lib` is present. If it is, do a clean rebuild (**Build > Rebuild Solution**).

**Unresolved externals for `timeBeginPeriod`, `CreateDIBSection`, `ShellExecuteA`, etc.**
A Windows system library is missing from the link line. The project links `winmm.lib`, `gdi32.lib`, `user32.lib`, and `shell32.lib`; verify these are still listed under **Project Properties > Linker > Input > Additional Dependencies**.

**`LNK2038: mismatch detected for 'RuntimeLibrary'`**
The runtime library of your build does not match the one `raylib.lib` was built against. Under **Project Properties > C/C++ > Code Generation > Runtime Library**, switch between `/MD`, `/MT` (and the Debug variants) until the mismatch clears, or rebuild raylib with your preferred setting.

**Toolset v145 not found (MSB8020)**
The project targets platform toolset `v145`. If your Visual Studio does not include it, right-click the project in Solution Explorer, choose **Retarget** or go to **Properties > General > Platform Toolset**, and select the newest toolset you have installed (for example `v143` for Visual Studio 2022). Apply this to all configurations.

**`std:c++20` or `IsTextureValid` not recognized**
The project requires C++20 and raylib 6.0 headers. Use the vendored headers in `vendor/raylib/include`, and avoid putting a different raylib version earlier on your include path.

### Runtime problems

**The window opens with no background image, or the text looks wrong or uses a default font**
The working directory is not the `ToDoApp/` folder, so `assets/...` cannot be found. Run the program from Visual Studio (Ctrl+F5), or from `ToDoApp\` on the command line, or copy the `assets` folder beside the executable. Check the console window for raylib `FILE: Failed to open` messages.

**The program does not start and reports a missing `raylib.dll`**
The default configuration links raylib statically and does not need the DLL. This error occurs only if you changed the project to link `raylibdll.lib`. In that case, copy `vendor/raylib/lib/raylib.dll` next to the executable. It is a 32-bit DLL, so it only works with an x86 build.

**My tasks were not saved**
Tasks are written only when the application exits normally. Close it with the window's close button. Ending the process from Task Manager, stopping the debugger, or a crash skips the save. Also verify that the working directory is writable; if `tasks.txt` cannot be opened for writing, the save fails silently.

**My tasks appear to be gone after starting from a different location**
`tasks.txt` is read from the current working directory. Launching from a different folder creates or reads a different file. Always launch from the same directory.

**I cannot add more tasks**
The list holds at most 20 tasks. Delete a task to free a slot; deleted slots are reused immediately.

**Some characters I type do not appear**
Input is restricted to printable ASCII (letters, digits, punctuation, space). Accented and non-Latin characters are ignored.

**Pressing Esc does not close the app**
This is intentional. The default exit key is disabled; Esc only closes the Add Task dialog. Use the window's close button to quit.

**A console window opens with the app**
The project uses the Console subsystem. The window is expected and shows raylib log output, which is useful for diagnosing missing files. To hide it, change **Linker > System > SubSystem** to **Windows** and add `/ENTRY:mainCRTStartup` under **Linker > Advanced > Entry Point**.

**The letter filter shows no tasks**
The filter matches letters that appear anywhere in the title and combines with the status filter. If the status filter is set to **Done** or **Pending**, a task must satisfy both. Click **Clear letter filter** or set the status filter to **All**.

**`tasks.txt` contains stray or corrupted lines**
Lines that do not follow the `<0|1>|<title>` format are skipped on load and removed on the next save. Fix or delete the file if you need to recover specific entries.

### Still stuck?

Open an issue and include your Visual Studio version, the selected configuration and platform, and the full build output or console log.

---

## Known Limitations

- Windows x86 (32-bit) only, a consequence of MSVC inline assembly.
- Maximum of 20 tasks, fixed at compile time by `MAX_TASKS`.
- Titles are limited to 63 printable ASCII characters; there is no Unicode input.
- Tasks cannot be edited after creation, only toggled or deleted.
- Persistence happens only on normal exit.
- `asm_trim_string` exists but is not yet applied to user input.
- `raygui.h` is bundled but not used by the current code.

---

## Contributing

1. Fork the repository and create a feature branch: `git checkout -b feature/short-description`.
2. Build and test the **Win32** configuration before committing.
3. Keep assembly routines register-safe (preserve `ebx`, `esi`, `edi` and any other registers you modify), null-guarded, and bounded by the 64-byte title size.
4. Do not commit `tasks.txt` or build output (`Debug/`, `Release/`, `.vs/`); these are already ignored.
5. Commit with a clear message, push your branch, and open a pull request describing what changed and how you tested it.

---

## License

No license file is currently included in this repository. Add one (for example MIT) before others reuse or redistribute the code. raylib is distributed under the zlib/libpng license; see the [raylib repository](https://github.com/raysan5/raylib) for its terms.