#include "asm_engine.h"

void asm_copy_string(char* dest, const char* src) {
    // TODO: Implement using CLD and REP MOVSB
}

void asm_clear_buffer(char* dest, int length) {
    // TODO: Implement using CLD, AL = 0, and REP STOSB
}

int asm_count_active(Task* list, int totalTasks) {
    // TODO: Traverse Task structs using [ESI + offset] and count active & !isDone
    return 0;
}

bool asm_search_char(const char* str, char target) {
    // TODO: Scan string using REPNE SCASB or byte-comparison loop
    return false;
}