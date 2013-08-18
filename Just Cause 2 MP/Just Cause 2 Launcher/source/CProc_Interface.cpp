#include <iostream>

#include "CProc_Interface.h"

using namespace std;

Proc_Interface::Proc_Interface(std::string proc_name, bool debug_pri, std::string game_path)
{
  m_PID = 0;
  m_Hproc = NULL;

  if (debug_pri) //elevated right
    if (!(this->SetDebugumod()))
      throw error(std::string("Unable to set privilege, error: ")  + to_string(GetLastError()), 0);

  if (game_path.size() > 1) //If we have a game path
    {
      STARTUPINFO siLoadee;
      memset(&m_piLoadee, 0, sizeof(PROCESS_INFORMATION)); //<- init process info struct
      memset(&siLoadee, 0, sizeof(STARTUPINFO));

      string strDir = game_path.substr(0, game_path.find_last_of("/\\")); //Get the process dir name
      //STart the process in its dir
      if (!CreateProcess(game_path.c_str(), NULL, NULL, NULL, FALSE,
                         CREATE_SUSPENDED, NULL, strDir.c_str(), &siLoadee, &m_piLoadee))
        throw error(std::string("[-] Process can't be created, error: ")  + to_string(GetLastError()), 0);
      m_Hproc = m_piLoadee.hProcess;
    }
  else if (proc_name.length() > 2) //Else we interfere a already running process
    {
      m_PID = GetProcessPIDByName(proc_name.c_str());
      int i = 0;
      while(!m_PID && i < 1000)
        {
          cerr << "\n[-] Game not Found :(\n-->Will try againin 10 sec" << endl;
          Sleep(10000);
          m_PID = GetProcessPIDByName(proc_name.c_str());//try again
          i++;
        }
      cout << "[+] Process Id found : " << m_PID << "\n-->Open process..." << endl;

      //we open the process in all access mode and get the handle.
      m_Hproc = OpenProcess(PROCESS_ALL_ACCESS, NULL, m_PID);
      if(!m_Hproc)  //Si on réussi à ouvrire le process.
        throw error(std::string("[-] Process can't be opened, error: ")  + to_string(GetLastError()), 0);
    }
}

Proc_Interface::~Proc_Interface()
{
  CloseHandle(m_Hproc); //Close the handle on the remote process
}

void Proc_Interface::resume_process()
{
  ResumeThread(m_piLoadee.hThread); //Resume remote thread
}

bool Proc_Interface::write_memory(void* Addr, const void* PATCH, unsigned int size)
{
  SIZE_T nbWrite = 0;
  bool ret;

  while ((nbWrite != size)
         && (ret = WriteProcessMemory (m_Hproc, (LPVOID)Addr, (BYTE*)PATCH, size, &nbWrite)))
    {
      if (!ret)
        {
          throw error(std::string("[-] Memory can't be written error: ") + to_string(GetLastError()), 0);
          return (1);
        }
      Addr = (void*)(&(((char*)Addr)[nbWrite]));
      size -= nbWrite;
    }
  return (0);
}

bool Proc_Interface::read_memory(void* Addr, void* buffer, unsigned int size)
{
  SIZE_T nbRead = 0;
  bool ret;

  while ((nbRead != size)
         && (ret = ReadProcessMemory(m_Hproc, (LPVOID)Addr, buffer, size, &nbRead)))
    {
      if (!ret)
        {
          throw error(std::string("[-] Memory can't be read-den: ") + to_string(GetLastError()), 0);
          return (1);
        }
      Addr = (void*)(&(((char*)Addr)[nbRead]));
      size -= nbRead;
    }
  return (0);
}

void* Proc_Interface::alloc_memory(void* Adrr, int taille) //NULL addr if you don't care where it's allocated
{
  //more tips -> http://msdn.microsoft.com/en-us/library/aa366890%28v=vs.85%29.aspx
  Adrr = VirtualAllocEx(m_Hproc, (LPVOID)Adrr, (SIZE_T)taille, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
  //MEM_commit to init mem to zero
  if(Adrr == NULL) //Alloc failed
    throw error(std::string("[-] Memory can't be allocated, error: ")  + to_string(GetLastError()), 0);
  return Adrr;
}

bool Proc_Interface::dealloc_memory(void* Adrr, int taille)
{
  //plus d'infos -> http://msdn.microsoft.com/en-us/library/aa366890%28v=vs.85%29.aspx
  return (VirtualFreeEx(m_Hproc, (LPVOID)Adrr, (SIZE_T)taille, MEM_DECOMMIT));
}

bool Proc_Interface::insertDll(std::string dll, int method)
{
  //Find the address of the LoadLibrary api, luckily for us, it is loaded in the same address for every process
  HMODULE hLocKernel32 = GetModuleHandle(L"KERNEL32.DLL");
  FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "LoadLibraryA");

  std::cout << "[+] Injecting dll " << dll << std::endl;
  if (method == 1)
    {
      bool res = false;
      //Allocate memory to hold the path to the Dll File in the process's memory
      dll += '\0';
      void* pLibPathRemote = this->alloc_memory(NULL, dll.size());

      if (!pLibPathRemote)
        throw error(std::string("[-] ERROR: Dll injection failed"), 0);
      //we unprotect the memory allocated
      VirtualProtectEx(m_Hproc, (LPVOID)pLibPathRemote, dll.size() + 10, PAGE_EXECUTE_READWRITE, NULL);
      //Write the path to the Dll File in the location just created
      this->write_memory(pLibPathRemote, dll.c_str(), dll.size());
      //Create a remote thread that call the LoadLibrary function with pLibPathRemote in arg
      HANDLE hRemoteThread = CreateRemoteThread(m_Hproc, NULL, 0,
                             reinterpret_cast<LPTHREAD_START_ROUTINE>(hLocLoadLibrary),
                             (void*)pLibPathRemote, CREATE_SUSPENDED, NULL);
      ResumeThread(hRemoteThread);

      if (!hRemoteThread)
        throw error(std::string("Thread can't be created !"), 0);
      //Wait for the thread to finish
      res = (bool)WaitForSingleObject(hRemoteThread, MAXWAIT) != WAIT_TIMEOUT;
      //Get the return of the remotely loaded DLL module
      DWORD ret = 0;
      GetExitCodeThread(hRemoteThread, &(ret));
      if(!ret)
        throw error(std::string("[-]ERROR Remote Thread fail with exit code: ") + to_string(ret), 0);
      //Free the memory created for dll name
      this->dealloc_memory(pLibPathRemote, dll.size());
      return (res);
    }
  else if (method == 2)
    {
      //lib detours code :
      BOOL fSucceeded = FALSE;
      DWORD nProtect = 0;
      DWORD nWritten = 0;
      CONTEXT cxt;
      UINT32 nCodeBase;
      PBYTE pbCode;

      struct Code
      {
        BYTE rbCode[128];
        union
        {
          WCHAR   wzLibFile[512];
          CHAR    szLibFile[512];
        };
      } code;


      SuspendThread(m_piLoadee.hThread);
      ZeroMemory(&cxt, sizeof(cxt)); //set context to zero
      cxt.ContextFlags = CONTEXT_FULL;
      GetThreadContext(m_piLoadee.hThread, &cxt);

      nCodeBase = (cxt.Esp - sizeof(code)) & ~0x1fu; // Cache-line align.
      pbCode = code.rbCode;

      CopyMemory(code.szLibFile, (dll.c_str()), dll.size());
      pbCode = DetourGenPush(pbCode, nCodeBase + offsetof(Code, szLibFile));
      pbCode = DetourGenCall(pbCode, reinterpret_cast<PBYTE>(hLocLoadLibrary),
                             (PBYTE)nCodeBase + (pbCode - code.rbCode));

      pbCode = DetourGenMovEax(pbCode, cxt.Eax);
      pbCode = DetourGenMovEbx(pbCode, cxt.Ebx);
      pbCode = DetourGenMovEcx(pbCode, cxt.Ecx);
      pbCode = DetourGenMovEdx(pbCode, cxt.Edx);
      pbCode = DetourGenMovEsi(pbCode, cxt.Esi);
      pbCode = DetourGenMovEdi(pbCode, cxt.Edi);
      pbCode = DetourGenMovEbp(pbCode, cxt.Ebp);
      pbCode = DetourGenMovEsp(pbCode, cxt.Esp);
      pbCode = DetourGenJmp(pbCode, (PBYTE)cxt.Eip,
                            (PBYTE)nCodeBase + (pbCode - code.rbCode));

      cxt.Esp = nCodeBase - 4;
      cxt.Eip = nCodeBase;

      VirtualProtectEx(m_Hproc, (PBYTE)nCodeBase, sizeof(Code),
                       PAGE_EXECUTE_READWRITE, &nProtect);

      WriteProcessMemory(m_Hproc, (PBYTE)nCodeBase, &code, sizeof(Code),
                         &nWritten);

      FlushInstructionCache(m_Hproc, (PBYTE)nCodeBase, sizeof(Code));
      SetThreadContext(m_piLoadee.hThread, &cxt);
      ResumeThread(m_piLoadee.hThread);

      return (true);
    }
  return (false);
}

bool Proc_Interface::SetDebugumod()
{
  HANDLE TokenHandle;
  LUID lpLuid;
  TOKEN_PRIVILEGES NewState;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &TokenHandle))
    return 0;
  if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &lpLuid))
    {
      CloseHandle(TokenHandle);
      return 0;
    }
  NewState.PrivilegeCount = 1;
  NewState.Privileges[0].Luid = lpLuid;
  NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  if (!AdjustTokenPrivileges(TokenHandle, FALSE, &NewState, sizeof(NewState), NULL,
                             NULL))
    {
      CloseHandle(TokenHandle);
      return 0;
    }
  CloseHandle(TokenHandle);
  return true;
}

HANDLE Proc_Interface::create_remote_thread(void* addr)
{
  LPTHREAD_START_ROUTINE fonc = remote_thread; //cast addrr into func ptr
  HANDLE thread_handle = CreateRemoteThread(m_Hproc, NULL, 0, fonc, &addr, 0, NULL);
  //No safe file descriptor or stack size, we start the thread right now
  return (thread_handle);
}

DWORD WINAPI remote_thread(void* arg)
{
  void* adrr = arg;
//call addr
  __asm ("call %%eax":"=a"(adrr));
  return (DWORD)NULL;
}

DWORD GetProcessPIDByName(const char moduleName[256]) //to comment
{
  DWORD idProcess[256];
  DWORD cb = sizeof(idProcess);
  DWORD cdNeeded;
  BOOL didigetlist = EnumProcesses((DWORD*)&idProcess, cb, &cdNeeded);
  wchar_t szProcessName[256] = L"unknown";
  int noProcess = cdNeeded / sizeof(DWORD);
  int searchPID = 0;
  for (int i = 0; i < noProcess; i++)
    {
      HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, idProcess[i]);
      if (NULL != hProcess)
        {
          HMODULE hMod;
          DWORD cbNeeded;
          BOOL didnumwork = EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded);
          DWORD y = GetLastError();
          if ( didnumwork )
            {
              DWORD diditwork = GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName) );
              _strlwr((char*)&szProcessName);
              DWORD x = GetLastError();
            }
        }
      if (!stricmp(szProcessName, moduleName)) //it's the one
        return (idProcess[i]); //we return it
    }
  return (searchPID);
}
