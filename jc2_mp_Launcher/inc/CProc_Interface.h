#ifndef JC2_H_INCLUDED
#define JC2_H_INCLUDED

#include <windows.h>
#include "psapi.h" //pour les fonctions specifique

#include <string>

#define MAXWAIT 10000

typedef unsigned char octet; //pour eviter d'utilisez le type BYTE de windows

DWORD GetProcessPIDByName(char moduleName[256]);
DWORD WINAPI remote_thread(void* arg); //la fonction qui rappelle la fonction dans le programme cible

class Proc_Interface
{
public:
    Proc_Interface(char proc_name[],bool debug_pri = false,std::string game_path = "");
    virtual ~Proc_Interface();

    virtual bool write_memory(DWORD Addr,const void* PATCH,int taille);
    virtual bool read_memory(DWORD Addr,void* resultat,int taille);

    HANDLE create_remote_thread(int adrr); //pour appeller une fonction dans le jeu
    bool insertDll(std::string dll,int method = 1); //pour injecter une dll
    void resume_process();

    virtual int alloc_memory(int Adrr,int taille); //pour allouer de la memoir sur le processus distant
    virtual bool dealloc_memory(int Adrr,int taille); //pour la dessallouer

    bool SetDebugumod(); //pour avoir des droits plus elever


private:
    DWORD m_PID; //Process Id.
    HANDLE m_Hproc;
    PROCESS_INFORMATION m_piLoadee;
};


inline PBYTE DetourGenMovEax(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xB8;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEbx(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBB;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEcx(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xB9;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEdx(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBA;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEsi(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBE;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEdi(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBF;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEbp(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBD;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenMovEsp(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0xBC;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenPush(PBYTE pbCode, UINT32 nValue)
{
    *pbCode++ = 0x68;
    *((UINT32*&)pbCode)++ = nValue;
    return pbCode;
}

inline PBYTE DetourGenPushad(PBYTE pbCode)
{
    *pbCode++ = 0x60;
    return pbCode;
}

inline PBYTE DetourGenPopad(PBYTE pbCode)
{
    *pbCode++ = 0x61;
    return pbCode;
}

inline PBYTE DetourGenJmp(PBYTE pbCode, PBYTE pbJmpDst, PBYTE pbJmpSrc = 0)
{
    if (pbJmpSrc == 0) {
        pbJmpSrc = pbCode;
    }
    *pbCode++ = 0xE9;
    *((INT32*&)pbCode)++ = pbJmpDst - (pbJmpSrc + 5);
    return pbCode;
}

inline PBYTE DetourGenCall(PBYTE pbCode, PBYTE pbJmpDst, PBYTE pbJmpSrc = 0)
{
    if (pbJmpSrc == 0) {
        pbJmpSrc = pbCode;
    }
    *pbCode++ = 0xE8;
    *((INT32*&)pbCode)++ = pbJmpDst - (pbJmpSrc + 5);
    return pbCode;
}

inline PBYTE DetourGenBreak(PBYTE pbCode)
{
    *pbCode++ = 0xcc;
    return pbCode;
}

inline PBYTE DetourGenRet(PBYTE pbCode)
{
    *pbCode++ = 0xc3;
    return pbCode;
}

inline PBYTE DetourGenNop(PBYTE pbCode)
{
    *pbCode++ = 0x90;
    return pbCode;
}

#endif // JC2_H_INCLUDED
