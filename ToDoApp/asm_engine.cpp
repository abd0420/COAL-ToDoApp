#include "asm_engine.h"


void asm_copy_string(char* dest, const char* src) {
    __asm {
        __asm {
            push ebx
            push esi
            push edi
            push ecx

            mov esi, src
            mov edi, dest
            test esi, esi       // null ptr check for src
            jz CopyDone
            test edi, edi       // null ptr check for src
            jz CopyDone

            xor ecx, ecx       // ecx = 0

            count_loop:
                cmp byte ptr[esi + ecx], 0
                je copy_start  //stops when null-term is reached
                cmp ecx, 63    //capps at 63 chars so title[64] never overflows
                je copy_start
                inc ecx
                jmp count_loop

            copy_start :
                cld     //clears DF
                rep movsb
                mov byte ptr[edi], 0    // to append \0 at the end

            CopyDone:
                pop ecx
                pop edi
                pop esi
                pop ebx
        }
    }
}

void asm_clear_buffer(char* dest, int bufferLength) {
    __asm {
        push ebx
        push edi
        push ecx

        mov edi, dest
        mov ecx, bufferLength
        
        //NULL dest gurad
        test edi, edi
        jz done_clear

        // 0 or -ve buffer lengths omitted
        cmp ecx, 0
        jle done_clear

        cld
        xor eax, eax

        ; Stage 1: Fast 4 - byte(DWORD) bulk wipe using REP STOSD
        mov ebx, ecx
        shr ecx, 2         // ECX = bufferLength / 4 
        rep stosd; 

        ; Stage 2: Wipe remaining 0–3 trailing bytes using REP STOSB
        mov ecx, ebx
        and ecx, 3      //ecx = bufferLength % 4 (remainder bytes)
        rep stosb

        done_clear :
            pop ecx
            pop edi
            pop ebx
    }
}

//  Clear-out queries on this
int asm_count_active(Task* list, int totalTasks) {
    int pendingCount = 0;

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi

        mov esi, list
        mov ecx, totalTasks
        xor eax, eax

        // NULL array pointer
        test esi, esi
        jz exit_count

        // empty or negative task count
        cmp ecx, 0
        jle exit_count

        count_loop :
    
            mov edi, esi
            add edi, 65         // EDI = &taskList[i].isActive(offset + 65)
            movzx edx, byte ptr[edi] 
            cmp edx, 1
            jne next_task;      //sk ips deleted / inactive slots(isActive == false)

            mov edi, esi
            add edi, 64         //EDI = &taskList[i].isDone(offset + 64)
            movzx edx, byte ptr[edi]
            cmp edx, 0
            jne next_task

            inc eax

            next_task :
      
            add esi, 66         // ESI now points to taskList[i + 1]
            dec ecx
            jnz count_loop

            mov pendingCount, eax;      //  stores final tally into C++ var

            exit_count :
            pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
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
        NoTerminator :
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