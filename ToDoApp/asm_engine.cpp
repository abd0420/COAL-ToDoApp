#include "asm_engine.h"

// Task layout (sizeof = 66): title[64] @ +0 | isDone @ +64 | isActive @ +65

void asm_copy_string(char* dest, const char* src) {
    __asm {
        push ebx
        push esi
        push edi
        push ecx

        mov esi, src
        mov edi, dest

        test esi, esi               // null guards
        jz copy_done
        test edi, edi
        jz copy_done

        xor ecx, ecx                // ECX = length to copy

        count_loop :
        cmp byte ptr[esi + ecx], 0
            je copy_start
            cmp ecx, 63             // cap at 63 so title[64] never overflows
            je copy_start
            inc ecx
            jmp count_loop

            copy_start :
        cld
            rep movsb
            mov byte ptr[edi], 0    // null-terminate destination

            copy_done :
            pop ecx
            pop edi
            pop esi
            pop ebx
    }
}


void asm_clear_buffer(char* dest, int bufferLength) {
    __asm {
        push ebx
        push edi
        push ecx

        mov edi, dest
        mov ecx, bufferLength

        test edi, edi               // null guard
        jz done_clear

        cmp ecx, 0                  // ignore zero / negative lengths
        jle done_clear

        cld
        xor eax, eax

        mov ebx, ecx                // stage 1: wipe 4 bytes at a time
        shr ecx, 2
        rep stosd

        mov ecx, ebx                // stage 2: wipe the last 0-3 bytes
        and ecx, 3
        rep stosb

        done_clear :
        pop ecx
            pop edi
            pop ebx
    }
}


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

        test esi, esi               // null guard
        jz exit_count

        cmp ecx, 0                  // empty / negative count
        jle exit_count

        count_loop :
        cmp byte ptr[esi + 65], 1   // skip deleted slots
            jne next_task

            cmp byte ptr[esi + 64], 0   // skip completed tasks
            jne next_task

            inc eax

            next_task :
        add esi, 66
            dec ecx
            jnz count_loop

            mov pendingCount, eax

            exit_count :
        pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
    }

    return pendingCount;
}


int asm_count_completed(Task* list, int totalTasks) {
    int completedCount = 0;

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi

        mov esi, list
        mov ecx, totalTasks
        xor eax, eax

        test esi, esi               // null guard
        jz exit_completed

        cmp ecx, 0                  // empty / negative count
        jle exit_completed

        completed_loop :
        cmp byte ptr[esi + 65], 1   // skip deleted slots
            jne next_completed

            cmp byte ptr[esi + 64], 1   // skip pending tasks
            jne next_completed

            inc eax

            next_completed :
        add esi, 66
            dec ecx
            jnz completed_loop

            mov completedCount, eax

            exit_completed :
        pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
    }

    return completedCount;
}


bool asm_search_char(const char* str, char target) {
    bool found = false;

    __asm {
        push ebx
        push esi
        push edi

        mov edi, str
        test edi, edi               // null guard
        jz NotFound

        mov bl, target
        test bl, bl
        jz NotFound

        cld
        xor al, al
        mov ecx, 64
        repne scasb                 // find the null terminator
        jne NoTerminator

        mov eax, 63                 // length = 63 - remaining count
        sub eax, ecx
        mov ecx, eax
        jmp Rest

        NoTerminator :
        mov ecx, 64             // no terminator: scan all 64 bytes

            Rest :
            test ecx, ecx
            jz NotFound

            mov edi, str
            mov al, bl
            cld

            repne scasb
            jne NotFound

            mov found, 1

            NotFound:
        pop edi
            pop esi
            pop ebx
    }

    return found;
}


// Removes leading/trailing spaces and tabs in place.
// Returns the new length (0 if the string was empty or only whitespace).
int asm_trim_string(char* inputStr) {
    int trimmedLen = 0;

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi

        mov esi, inputStr
        xor eax, eax

        test esi, esi               // null guard
        jz TrimExit

        xor ebx, ebx                // EBX = startIdx

        FindStartLoop :
        cmp ebx, 63
            jge AllWhitespace

            mov dl, byte ptr[esi + ebx]
            cmp dl, 0
            je AllWhitespace
            cmp dl, 0x20            // space
            je AdvanceStart
            cmp dl, 0x09            // tab
            je AdvanceStart
            jmp FoundStart

            AdvanceStart :
        inc ebx
            jmp FindStartLoop

            AllWhitespace :
        mov byte ptr[esi], 0
            xor eax, eax
            jmp TrimExit

            FoundStart :
        mov ecx, ebx            // find the null terminator

            FindNullLoop :
        cmp ecx, 63
            jge FoundNull
            mov dl, byte ptr[esi + ecx]
            cmp dl, 0
            je FoundNull
            inc ecx
            jmp FindNullLoop

            FoundNull :
        dec ecx                 // ECX = endIdx (last character)

            TrimEndLoop :
        cmp ecx, ebx
            jle ComputeLength

            mov dl, byte ptr[esi + ecx]
            cmp dl, 0x20
            je StepBackEnd
            cmp dl, 0x09
            je StepBackEnd
            jmp ComputeLength

            StepBackEnd :
        dec ecx
            jmp TrimEndLoop

            ComputeLength :
        mov eax, ecx            // length = endIdx - startIdx + 1
            sub eax, ebx
            inc eax

            test ebx, ebx           // no shift needed if startIdx == 0
            jz WriteNull

            mov ecx, eax            // shift the trimmed text to index 0
            mov edi, esi
            add esi, ebx
            cld
            rep movsb

            mov esi, inputStr

            WriteNull :
        mov byte ptr[esi + eax], 0
            mov trimmedLen, eax

            TrimExit :
        pop edi
            pop esi
            pop edx
            pop ecx
            pop ebx
    }

    return trimmedLen;
}