#include <windows.h>

#include "hooksystem.h"
#include "Cthread.h"

CThread* dllThread = NULL;

void dllthreadmain(void* arg)
{
}

void* gameCreateThread()
{
  void* cWorld;

  cWorld = (void*)0x0118EB9C;
  cWorld = *((void**)cWorld);
  dllThread->SetData(cWorld);
  dllThread->SetState(STOP);
  dllThread->startThread();
  return (cWorld);
}

void initThread()
{
  dllThread = new CThread(&dllthreadmain, NULL);
  /// Unprotect all of the JC2 memory at once and leave it that way
  DWORD oldProt;
  if(!VirtualProtect((LPVOID)0x00401000, 0x00FEEFFF, PAGE_EXECUTE_READWRITE, &oldProt)) //0x00401000 to 0x00FFFFFF
    MessageBoxA(NULL, NULL, "Couldn't unprotect memory", MB_OK);
  HookInstallCall((void*)0x006002E3, (void*)&gameCreateThread);
}

extern "C"
{
//Function called at dll attach
  BOOL WINAPI DllMain(UNSEDP HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      UNSEDP LPVOID lpReserved)
  {
    switch (ul_reason_for_call)
      {
      case DLL_PROCESS_ATTACH:
        initThread();
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
