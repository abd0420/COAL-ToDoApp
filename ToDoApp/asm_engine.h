#pragma once
#ifndef ASM_ENGINE_H
#define ASM_ENGINE_H

#include "storage.h"

//copies string from src into dest 
void asm_copy_string(char* dest, const char* src);

//wipes buffer with zeros 
void asm_clear_buffer(char* dest, int length);

//returns count of active, uncompleted tasks
int asm_count_active(Task* list, int totalTasks);

//Scans a task's title for a target character
bool asm_search_char(const char* str, char target);

#endif