#include "CProc_Interface.h"

#include <iostream>

using namespace std;

Proc_Interface::Proc_Interface(char proc_name[],bool debug_pri,std::string game_path)
{
    m_PID=0;
    m_Hproc=0;


    if(debug_pri) //privilege superieur
    {
        if(this->SetDebugumod())
        {
            cout << "Privilege set !" << endl;
        }
        else
        {
            cout << "Unable to set privilege :(" << endl;
        } //<- si on a besoin des privilege !
    }

    if(game_path.size() > 1) //si on la envoyer en parametre
    {
        STARTUPINFO siLoadee;
        memset( &m_piLoadee, 0, sizeof ( PROCESS_INFORMATION ) ); //<- fonctiosn standart O_o
        memset( &siLoadee, 0, sizeof ( STARTUPINFO ) );

        string strDir = game_path.substr(0,game_path.find_last_of("/\\")); //on recupere le repertoir du jeu
        //start the game
        if ( 0 == CreateProcess(game_path.c_str(),NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,strDir.c_str(),&siLoadee,&m_piLoadee ) )
        {
            cout << "[-] Process can't be created: " << GetLastError () <<endl;
        }
        else
        {
            m_Hproc = m_piLoadee.hProcess;
            cout << "[+] Process created and opened" << endl;
            //you have to resume the thread because it is created suspend
        }
    }
    else if(strlen(proc_name) > 2)
    {


        m_PID = GetProcessPIDByName(proc_name);
        while(!m_PID)
        {
            // Si pas de fenêtre trouvée, On s'arrête là.
            cout << "\n[-] Game not Found :(\n-->Il n'est sans doute pas lance.\n-->Reaissaye dans 10 sec" << endl;
            Sleep(10000);
            m_PID = GetProcessPIDByName(proc_name);//on reassaie
        }


        cout << "[+] Process Id found : " << m_PID << "\n-->Open process..." << endl;

//On ouvre le process en mode all access et on obtient le handle.
        m_Hproc = OpenProcess(PROCESS_ALL_ACCESS,NULL,m_PID);

        if(m_Hproc)  //Si on réussi à ouvrire le process.
        {
            cout << "[+] Process successfuly opened!\n" <<endl;
        }
        else   // Si on a réussi à lire le process --> message d'erreur.
        {
            cout << "[-] Process can't be opened\n-->Operation Canceled."<<endl;
        }
    }

}

Proc_Interface::~Proc_Interface()
{
    CloseHandle(m_Hproc);
}

void Proc_Interface::resume_process()
{
    ResumeThread( m_piLoadee.hThread );
}


bool Proc_Interface::write_memory(DWORD Addr,const void* PATCH,int taille)
{
// Là c'est crutial, on va écrire dans la mémoire du process.
// Hproc, le handle du process
// Addr, l'address où l'on veut écrire
    unsigned int taille_patch = taille;
    SIZE_T nbWritte = 0;

    if(WriteProcessMemory (m_Hproc,(LPVOID)Addr,(BYTE*)PATCH,taille_patch, &nbWritte))
    {

        if(nbWritte==taille_patch)  //On vérifie que les 3 Bytes ont bien étée écrites.
        {
            //cout << "[+] Patch Succefull: " << nbWritte << " octet write" << endl;
            return 0;
        }
        else   //Aussi-non, message d'erreur.
        {
            cout << "[-] Failled to patch memory :(\n-->Unknows reason" <<endl;
            return 1;
        }
    }
    else   //Si on arrive pas à écrire dans la mémoire.
    {
        cout << "[-] Memory can't be written\n-->Operation Canceled." << endl;
        return 1;
    }

}

bool Proc_Interface::read_memory(DWORD Addr,void* resultat,int taille)
{
    SIZE_T nbRead = 0;
    unsigned int taille_valeur = taille;

    if(ReadProcessMemory(m_Hproc,(LPVOID)Addr,resultat,taille_valeur,&nbRead))
    {
        if(nbRead==taille_valeur)  //On vérifie que les Bytes ont bien étée écrites.
        {
            //cout << "[+] Read Succefull: " << nbRead << " octet read" << endl;
            return 0;
        }
        else   //ou si-non, message d'erreur.
        {
            cout << "[-] Failled to read memory :(\n-->Unknows reason" <<endl;
            return 1;
        }
    }
    else   //Si on arrive pas à écrire dans la mémoire.
    {
        cout << "[-] Memory can't be readden\n-->Operation Canceled." << endl;
        return 1;
    }

}

int Proc_Interface::alloc_memory(int Adrr,int taille) //Mettre l'adresse a NULL sinon on n'a pas de preference
{
    //plus d'infos -> http://msdn.microsoft.com/en-us/library/aa366890%28v=vs.85%29.aspx
    int temp_adrr;
    LPVOID convertable_adrr;
    convertable_adrr = VirtualAllocEx(m_Hproc,(LPVOID)Adrr,(SIZE_T)taille,MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN,PAGE_EXECUTE_READWRITE);
    //MEM_commit pour mettre a 0 la memoire
    if(convertable_adrr == NULL) //si il y a eu une erreur
    {
        std::cout << "Erreur impossible d'allouer la memoire" << std::endl;
        //on prend l'erreur windows
        DWORD erreur = GetLastError();
        std::cout << erreur << std::endl;
    }
    temp_adrr = (int)convertable_adrr;

    return temp_adrr;
}

bool Proc_Interface::dealloc_memory(int Adrr,int taille)
{
    //plus d'infos -> http://msdn.microsoft.com/en-us/library/aa366890%28v=vs.85%29.aspx
    if(VirtualFreeEx(m_Hproc,(LPVOID)Adrr,(SIZE_T)taille,MEM_DECOMMIT))
        return 0;
    else
        return 1;
}

bool Proc_Interface::insertDll(std::string dll,int method)
{
        //Find the address of the LoadLibrary api, luckily for us, it is loaded in the same address for every process
        HMODULE hLocKernel32 = GetModuleHandle("KERNEL32.DLL");
        FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "LoadLibraryA");

    if(method == 1)
    {

        bool res = false;

        // cout << "[+] Adress LoadLibraryA: " << std::hex << hLocLoadLibrary << std::dec << endl;

        //Allocate memory to hold the path to the Dll File in the process's memory
        dll += '\0';
        int pLibPathRemote = this->alloc_memory(NULL,dll.size());

        std::cout << "[+] Injecting dll " << dll << std::endl;

        if(pLibPathRemote)
        {
            //we unprotect the memory allocated
            VirtualProtectEx(m_Hproc,(LPVOID)pLibPathRemote,dll.size()+10, PAGE_EXECUTE_READWRITE,NULL);
            //Write the path to the Dll File in the location just created
            this->write_memory(pLibPathRemote,dll.c_str(),dll.size());
            //Create a remote thread that starts begins at the LoadLibrary function and is passed are memory pointer
            HANDLE hRemoteThread = CreateRemoteThread(m_Hproc, NULL, 0, reinterpret_cast < LPTHREAD_START_ROUTINE > (hLocLoadLibrary),(void*)pLibPathRemote,CREATE_SUSPENDED,NULL);
            ResumeThread(hRemoteThread);

            if (hRemoteThread)
            {
                //Wait for the thread to finish
                res = (bool)WaitForSingleObject(hRemoteThread, MAXWAIT) != WAIT_TIMEOUT;
                /* Get the return of the remotely loaded DLL module */
                DWORD ret = 0;
                GetExitCodeThread (hRemoteThread, &(ret));
                if(!ret)
                {
                    std::cout << "[-]ERROR Remote Thread fail with exit code: "<< ret <<" on the process you wanted to inject GetLastError() = " << GetLastError() << std::endl;
                    res = 0;
                }
            }
            else
            {
                cout << "Thread can't be created !" << endl;
            }

            //Free the memory created on the other process
            this->dealloc_memory(pLibPathRemote,dll.size());


        }
        else
        {
            std::cout << "[-] ERROR: Dll injection failed" << std::endl;
        }

        return res;
    }
    else if(method == 2)
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
        ZeroMemory(&cxt, sizeof(cxt)); //on met a zero le context
        cxt.ContextFlags = CONTEXT_FULL;
        GetThreadContext(m_piLoadee.hThread, &cxt);

        nCodeBase = (cxt.Esp - sizeof(code)) & ~0x1fu;        // Cache-line align.
        pbCode = code.rbCode;

        CopyMemory(code.szLibFile,(dll.c_str()),dll.size());
        pbCode = DetourGenPush(pbCode, nCodeBase + offsetof(Code, szLibFile));
        pbCode = DetourGenCall(pbCode,reinterpret_cast<PBYTE>(hLocLoadLibrary),
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

        return 1;

    }
}


bool Proc_Interface::SetDebugumod()
{
    HANDLE TokenHandle;
    LUID lpLuid;
    TOKEN_PRIVILEGES NewState;
    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &TokenHandle))
        return 0;
    if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &lpLuid))
    {
        CloseHandle(TokenHandle);
        return 0;
    }
    NewState.PrivilegeCount = 1;
    NewState.Privileges[0].Luid = lpLuid;
    NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if(!AdjustTokenPrivileges(TokenHandle, FALSE, &NewState, sizeof(NewState), NULL,
                              NULL))
    {
        CloseHandle(TokenHandle);
        return 0;
    }
    CloseHandle(TokenHandle);
    return true;
}


HANDLE Proc_Interface::create_remote_thread(int addr)
{
    LPTHREAD_START_ROUTINE fonc = remote_thread; //onvertir l'adress en tant que pointeur sur fonction
    HANDLE thread_handle = CreateRemoteThread(m_Hproc,NULL,0,fonc,&addr,0,NULL);
    /*pas de descripteur de securiter ni de taille de stack, on demmare le thread directe*/
    return thread_handle;
}




DWORD WINAPI remote_thread(void* arg)
{
    int adrr = *((int*)arg);//le pointeur contient ladresse de la focntion a appeller

//on call l'adress
    __asm__ __volatile__("call %%eax" //quelle merde la syntaxe de gcc
                     :"=a"(adrr)
                        );

    return (DWORD)NULL;

}

DWORD GetProcessPIDByName(char moduleName[256]) //a commenter et a eclaircir
{

//get pid

    DWORD idProcess[256];

    DWORD cb = sizeof(idProcess);

    DWORD cdNeeded;

    BOOL didigetlist = EnumProcesses((DWORD*)&idProcess, cb,&cdNeeded);

    char szProcessName[256] = "unknown";

    int noProcess=cdNeeded/sizeof(DWORD);

    int searchPID=0;

    int i=0;

    for (i=0; i < noProcess; i++ )

    {

        HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,idProcess[i]);

        if (NULL != hProcess)

        {

            HMODULE hMod;

            DWORD cbNeeded;

            BOOL didnumwork = EnumProcessModules( hProcess, &hMod, sizeof(hMod),&cbNeeded);

            DWORD y=GetLastError();

            if ( didnumwork )

            {

                DWORD diditwork=GetModuleBaseName( hProcess, hMod, szProcessName,sizeof(szProcessName) );

                _strlwr((char*)&szProcessName);

                DWORD x = GetLastError();

            }

        }

        if(!stricmp(szProcessName,moduleName)) //si c'est le bon
        {

            searchPID=idProcess[i]; //on le met

            break;

        }

    }

    return searchPID;

}
