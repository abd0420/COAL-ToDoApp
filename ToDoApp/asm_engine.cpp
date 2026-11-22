#include "asm_engine.h"


void asm_copy_string(char* dest, const char* src) {
    __asm {
        push esi
        push edi
        push ecx

        mov esi, src
        mov edi, dest

        mov ecx, 0

        count_loop :
        cmp byte ptr[esi + ecx], 0
            je copy_start
            inc ecx
            jmp count_loop

            copy_start :
        inc ecx
            cld
            rep movsb

            pop ecx
            pop edi
            pop esi
    }
}



void asm_clear_buffer(char* dest, int bufferLength) {
    __asm {
        push edi
        push ecx

        mov edi, dest
        mov ecx, bufferLength
        xor eax, eax

        cmp ecx, 0
        jle done_clear

        cld; Forward direction
        rep stosb

        done_clear :
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

        mov esi, list;                      ESI = base pointer to taskList[0]

        mov ecx, totalTasks;                ECX = loop counter
        
        xor eax, eax;                       EAX = pending task count(0)

        cmp ecx, 0
        jle done_count;                     exit if taskCount <= 0

        count_loop:

            mov bl, byte ptr[esi + 65];     loads taskList[i].isActive(offset + 65)
            cmp bl, 1
            jne next_task;                  Skip deleted slots(isActive == false)

            mov bl, byte ptr[esi + 64];     load taskList[i].isDone(offset + 64)
            cmp bl, 0
            jne next_task;                  Skip completed tasks(isDone == true)

            inc eax;                        Active & pending-> increment EAX

        next_task :
            add esi, 66;                    Advance ESI to next Task struct (sizeof(Task) = 66)
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
        push ebx
        push esi
        push edi

        mov edi, str
        test edi, edi           //safeguard for a null-ptr access
        jz NotFound

        mov bl, target
        test bl, bl
        jz NotFound

        cld
        xor al, al
        mov ecx, 64
        REPNE SCASB             //hunts for the null-terminator
        jne NoTerminator

        //if null-terminator found then;
        mov eax, 63
        sub eax, ecx            //string length = 63 - remaining characters
        mov ecx, eax
        jmp Rest
        
        //if not found then;
        NoTerminator:
        mov ecx, 64         //re capps the scan length at 64

            Rest :
            test ecx, ecx
            jz NotFound

            mov edi, str
            mov al, bl      //target copied from bl into al
            cld

            REPNE SCASB
            jne NotFound

            mov found, 1
           

        NotFound:
            pop edi;
            pop esi;
            pop ebx;

    }
        return found;
}