# COAL-ToDoApp

A desktop To-Do application built in **C++** with **raylib** for graphical rendering and **x86 inline assembly (`__asm`)** for low-level memory, string, search, and task-counting operations—created for **Computer Organization and Assembly Language (COAL)**.

---

## Table of Contents

- [Overview](#overview)
- [Features & UI Highlights](#features--ui-highlights)
- [Repository Structure](#repository-structure)
- [Memory Layout & Assembly Engine (`asm_engine`)](#memory-layout--assembly-engine)
  - [`Task` Struct & Byte Offsets](#task-struct--byte-offsets)
  - [Routine-by-Routine Assembly Breakdown](#routine-by-routine-assembly-breakdown)
- [File Persistence & `tasks.txt` Format](#file-persistence--taskstxt-format)
- [How to Build & Run](#how-to-build--run)
  - [Prerequisites](#prerequisites)
  - [Visual Studio Setup (Windows)](#visual-studio-setup-windows)
  - [MSVC Command-Line Build](#msvc-command-line-build)
- [User Guide & Controls](#user-guide--controls)
- [Troubleshooting](#troubleshooting)
- [Known Limitations & Edge Cases](#known-limitations--edge-cases)
- [Git & Pull Request Workflow](#git--pull-request-workflow)

---

## Overview

**COAL-ToDoApp** bridges a graphical desktop interface rendered with **raylib** and low-level x86 inline assembly routines (`__asm`) compiled with **MSVC**. Application state is stored in a fixed-size in-memory array (`MAX_TASKS = 20`), loaded from `tasks.txt` at startup, and written back to `tasks.txt` when exiting.

---

## Features & UI Highlights

- **Create, Toggle, and Delete Tasks:**
  - Open the **Create a new task** modal via **`+ Add Task`**, type up to 63 characters, and submit with **`Enter`** or **`Save Task`** (or dismiss with **`Esc`** / **`Cancel`**).
  - Toggle completion status (`Pending` ↔ `Done`) directly from each task's checkbox.
  - Delete tasks via the **`X`** button, which zeroes the task's title buffer with `asm_clear_buffer` and marks `isActive = false` so the slot can be reused later.
- **Status & Alphabetical Filtering:**
  - Filter tasks by **`All`**, **`Pending`**, or **`Done`** in the left sidebar.
  - Filter tasks by character using the **A–Z** grid (`asm_search_char`, checking both uppercase and lowercase ASCII) or reset with **`Clear letter filter`**.
- **Live Metrics & Scrollable Workspace:**
  - Displays real-time counters for **TOTAL**, **PENDING** (`asm_count_active`), and **COMPLETED** (`asm_count_completed`) tasks, alongside a live `"N showing"` count and smooth mouse-wheel scrolling.
- **Uniform Rounded UI:**
  - Converts a shared pixel radius (`CORNER_RADIUS = 10.0f`) into raylib's normalized roundness based on each rectangle's smallest side (`min(width, height)`) so panels, cards, inputs, and buttons share consistent corner styling.

---

## Repository Structure

```text
COAL-ToDoApp/
├── ToDoApp/
│   ├── main.cpp           # Entry point, window sizing/centering, background & overlay, render loop, save/load
│   ├── storage.h          # Task struct, MAX_TASKS constant, and storage API declarations
│   ├── storage.cpp        # taskList / taskCount definitions and tasks.txt serialization/deserialization
│   ├── asm_engine.h       # Function declarations for x86 inline assembly routines
│   ├── asm_engine.cpp     # x86 inline assembly (__asm) implementations
│   ├── ui_manager.h       # UI lifecycle and draw declarations
│   ├── ui_manager.cpp     # Sidebar, workspace, filters, modal dialog, scrolling, and uniform box helpers
│   ├── raygui.h           # raygui v5.1-dev header
│   └── assets/
│       ├── background.png         # Background texture rendered behind the dark readability overlay
│       ├── Hello Baby.otf         # Heading/display font
│       └── Internet Friends.ttf   # Body/UI font
└── README.md