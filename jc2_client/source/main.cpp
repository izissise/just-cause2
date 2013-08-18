#include <windows.h>
#include "detours.h"
#include "hooksystem.h"
#include "utilities.h"
#include "cheat.h"

void main()
{
    SetMultiRope(true);
    SetStrongRope(true);
    SetGodmod(true);
    //SetNoWanted(true);
    SetInfiniteAmmo(true);
    SetInfiniteGrenade(true);
    SetInfiniteHackTime(true);
}

void initThread()
{
    /// Unprotect all of the JC2 memory at once and leave it that way
    DWORD oldProt;
    if(!VirtualProtect((LPVOID)0x00401000, 0x00FEEFFF, PAGE_EXECUTE_READWRITE, &oldProt)) //0x00401000 to 0x00FFFFFF
        MessageBoxA(NULL, NULL, "Couldn't unprotect memory", MB_OK);
    main();
}

LRESULT ClientThread(LPVOID param)
{
    initThread();
    return 0;
}
//Function called at dll attach
BOOL WINAPI DllMain(UNUSED HMODULE hModule,
                    DWORD  ul_reason_for_call,
                    UNUSED LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DWORD id;
        MessageBoxA(NULL, NULL, "Dll injected", MB_OK);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientThread, NULL, 0, &id);
    }
    break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
