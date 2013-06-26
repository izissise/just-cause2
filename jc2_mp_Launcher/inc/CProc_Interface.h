#ifndef JC2_H_INCLUDED
#define JC2_H_INCLUDED

#include <string>
#include <sstream>
#include <windows.h>
#include "psapi.h" //pour les fonctions specifique
#include "CError.h"

#define MAXWAIT 10000

DWORD GetProcessPIDByName(const char moduleName[256]);
DWORD WINAPI remote_thread(void* arg); //Function called by atrget process

class Proc_Interface
{
public:
  Proc_Interface(std::string proc_name, bool debug_pri = false, std::string game_path = "");
  virtual ~Proc_Interface();

  virtual bool write_memory(void* Addr, const void* PATCH, unsigned int size);
  virtual bool read_memory(void* Addr, void* buffer, unsigned int size);

  HANDLE create_remote_thread(void* addr); //to call a remote func in the remote proc
  bool insertDll(std::string dll, int method = 1); //inject a dll
  void resume_process(); //resume paused process

  virtual void* alloc_memory(void* Adrr, int size); //alloc memory on the remote proceess, and return the base ptr
  virtual bool dealloc_memory(void* Adrr, int size); //free memory on the remote proc

  bool SetDebugumod(); //set this process elevated right


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

template<typename T>
std::string to_string(const T & Value)
{
  std::ostringstream oss;
  oss << Value;
  return oss.str();
}

#endif // JC2_H_INCLUDED
