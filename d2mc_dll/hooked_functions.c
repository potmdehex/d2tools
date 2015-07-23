// Created by John Åkerblom 2013

#include "hooked_functions.h"

#include <windows.h>

typedef	HANDLE (WINAPI *fnFindWindowA)(LPCSTR, LPCSTR);
HANDLE WINAPI hooked_FindWindowA(LPCSTR a1, LPCSTR a2);
static fnFindWindowA orig_FindWindowA;

// Orig: http://www.codeproject.com/Articles/27339/API-hooking-for-hotpatchable-operating-systems
VOID hook_hotpatch_x86(void *old_proc, const void *new_proc, void **orig_proc)
{
    DWORD old_protect = 0;
    WORD *back_jmp = (WORD *)old_proc;
    BYTE *long_jmp = ((BYTE *)old_proc - 5);
    DWORD *long_jmpaddr = ((DWORD *)old_proc - 1);

    VirtualProtect(long_jmp, 7, PAGE_EXECUTE_WRITECOPY, &old_protect);

    *long_jmp = 0xE9; // long jmp
    *long_jmpaddr = ((DWORD)new_proc) - ((DWORD)old_proc);
    *back_jmp = 0xF9EB; // short jmp back -7 (back 5, plus two for this jmp)

    if (NULL != orig_proc)
    {
        *orig_proc = ((BYTE *)old_proc) + 2;
    }
    
    VirtualProtect(long_jmp, 7, old_protect, &old_protect);
}

void hooked_functions_initialize(void)
{
    HANDLE hKernel = GetModuleHandleA("kernel32");

    hook_hotpatch_x86(GetProcAddress(hKernel, "FindWindowA"), 
        hooked_FindWindowA, 
        (void **)&orig_FindWindowA);
}

HANDLE WINAPI hooked_FindWindowA(LPCSTR a1, LPCSTR a2)
{
    if (0 == lstrcmpA(a1, "Diablo II"))
    {
        return NULL;
    }

	return orig_FindWindowA(a1, a2);
}

