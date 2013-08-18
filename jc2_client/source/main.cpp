#include <windows.h>
#include "detours.h"
#include "hooksystem.h"
#include "Cthread.h"

#include "cheat.h"

CThread* dllThread = NULL;

void dllthreadmain(void* arg)
{
  if (arg == NULL)
    {
      Sleep(1);
      return;
    }

  SetMultiRope(true);
  SetStrongRope(true);
  SetGodmod(true);
  SetNoWanted(true);
  //DetourFunction((PBYTE)0x768540, hookInst->Detour);

  while(1)
    Sleep(5);
}

void* gameCreateThread()
{
  void* cWorld;

  cWorld = (void*)0x0118EB9C;
  cWorld = *((void**)cWorld);
  dllThread->SetData(cWorld);
  return (cWorld);
}

void initThread()
{
	MessageBoxA(NULL, NULL, "working", MB_OK);
  dllThread = new CThread(&dllthreadmain, NULL);
  /// Unprotect all of the JC2 memory at once and leave it that way
  DWORD oldProt;
  if(!VirtualProtect((LPVOID)0x00401000, 0x00FEEFFF, PAGE_EXECUTE_READWRITE, &oldProt)) //0x00401000 to 0x00FFFFFF
    MessageBoxA(NULL, NULL, "Couldn't unprotect memory", MB_OK);
  dllThread->SetData(NULL);
  dllThread->SetState(STOP);
  dllThread->startThread();
  HookInstallCall((void*)0x006002E3, (void*)&gameCreateThread);
}

extern "C"
{
	DLLEXPORT void main()
	{
		initThread();
	}

//Function called at dll attach
  BOOL WINAPI DllMain(UNUSED HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      UNUSED LPVOID lpReserved)
  {
    switch (ul_reason_for_call)
      {
      case DLL_PROCESS_ATTACH:
		  MessageBoxA(NULL, NULL, "Dll injected", MB_OK);
        break;
      case DLL_THREAD_ATTACH:
        delete dllThread; //Delete thread
        break;
      case DLL_THREAD_DETACH:
        delete dllThread;
        break;
      case DLL_PROCESS_DETACH:
        break;
      }
    return TRUE;
  }
}
