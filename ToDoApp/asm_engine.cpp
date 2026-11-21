#include "asm_engine.h"

void asm_copy_string(char* dest, const char* src) {
    __asm {
        push esi
        push edi
        push ecx

        mov esi, src; ESI = source buffer address
        mov edi, dest; EDI = destination buffer address
        xor ecx, ecx; ECX = character counter(0)

        find_len:
        cmp byte ptr[esi + ecx], 0; Check for null terminator
            je copy_bytes
            cmp ecx, 63; Cap at 63 chars to prevent title[64] overflow
            je copy_bytes
            inc ecx
            jmp find_len

            copy_bytes :
        cld; Clear Direction Flag(increment ESI & EDI)
            rep movsb; Copy ECX bytes from[ESI] to[EDI]
            mov byte ptr[edi], 0; Append null terminator at end of destination

            pop ecx
            pop edi
            pop esi
    }
}

void asm_clear_buffer(char* dest, int length) {
    __asm {
        push edi
        push ecx
        push eax

        mov edi, dest; EDI = target buffer address
        mov ecx, length; ECX = number of bytes to wipe
        cmp ecx, 0
        jle done_clear; Skip if length <= 0

        xor al, al; AL = 0 (zero byte value)
        cld; Clear Direction Flag(increment EDI)
        rep stosb; Fill ECX bytes at[EDI] with 0

        done_clear:
        pop eax
            pop ecx
            pop edi
    }
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
    bool found = false;

    __asm {
        push edi
        push ecx
        push eax

        mov al, target
        cmp al, 0
        je char_found; Empty target('\0') matches by default

        ; Step 1: Find string length(up to 64 bytes) using REPNE SCASB
        mov edi, str
        mov ecx, 64
        xor al, al; Search for null terminator '\0'
        cld
        repne scasb
        jne char_not_found; Not a valid null - terminated string within 64 bytes

        mov eax, 63
        sub eax, ecx; EAX = exact character count before '\0'
        cmp eax, 0
        jle char_not_found; Empty string cannot contain non - null target

        ; Step 2: Scan those EAX characters for target using REPNE SCASB
        mov ecx, eax; ECX = number of characters to scan
        mov edi, str; Reset EDI to start of string
        mov al, target; AL = target character
        cld
        repne scasb; Scan until[EDI] == AL or ECX == 0
        je char_found; Zero Flag(ZF = 1) means match found

        char_not_found :
        mov found, 0
            jmp done_search

            char_found :
        mov found, 1

            done_search :
            pop eax
            pop ecx
            pop edi
    }

    return found;
}