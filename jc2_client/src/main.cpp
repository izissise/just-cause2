#include <windows.h>

extern "C"
{
//Function called at dll attach
  BOOL WINAPI DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
  {
    switch (ul_reason_for_call)
      {
      case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL, NULL, NULL, MB_OK);
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
}
