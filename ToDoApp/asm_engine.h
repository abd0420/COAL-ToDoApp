#pragma once
#ifndef ASM_ENGINE_H
#define ASM_ENGINE_H

#include "storage.h"

// Copies null-terminated string from src into dest (max 64 bytes) using REP MOVSB
void asm_copy_string(char* dest, const char* src);

// Wipes buffer of given length with zeros using REP STOSB
void asm_clear_buffer(char* dest, int length);

// Traverses Task struct array and returns count of active, uncompleted tasks
int asm_count_active(Task* list, int totalTasks);

// Scans a task's title for a target character using REPNE SCASB
bool asm_search_char(const char* str, char target);

#endif