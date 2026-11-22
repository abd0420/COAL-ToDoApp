#include "asm_engine.h"

void asm_copy_string(char* dest, const char* src) {
    // TODO: Implement using CLD and REP MOVSB
}

void asm_clear_buffer(char* dest, int length) {
    // TODO: Implement using CLD, AL = 0, and REP STOSB
}

int asm_count_active(Task* list, int totalTasks) {
    int pendingCount = 0;

    __asm {
        push esi
        push ecx
        push ebx

        mov esi, list; ESI = base pointer to taskList[0]
        mov ecx, totalTasks; ECX = loop counter
        xor eax, eax; EAX = pending task count(0)

        cmp ecx, 0
        jle done_count; Exit if taskCount <= 0

        count_loop:
        mov bl, byte ptr[esi + 65]; Load taskList[i].isActive(offset + 65)
            cmp bl, 1
            jne next_task; Skip deleted slots(isActive == false)

            mov bl, byte ptr[esi + 64]; Load taskList[i].isDone(offset + 64)
            cmp bl, 0
            jne next_task; Skip completed tasks(isDone == true)

            inc eax; Active& pending->increment EAX

            next_task :
        add esi, 66; Advance ESI to next Task struct (sizeof(Task) = 66)
            dec ecx
            jnz count_loop

            done_count :
        mov pendingCount, eax

            pop ebx
            pop ecx
            pop esi
    }

    return pendingCount;
}

bool asm_search_char(const char* str, char target) {
    // TODO: Scan string using REPNE SCASB or byte-comparison loop
    return false;
}