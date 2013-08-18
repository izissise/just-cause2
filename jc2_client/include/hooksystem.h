#ifndef HOOKSYSTEM_H_INCLUDED
#define HOOKSYSTEM_H_INCLUDED

#define UNUSED __pragma(warning(suppress:4100))

#define NAKED void _declspec(naked) //For hooking functions !

#include <windows.h>
#include <string.h>

void HookInstallMethod(void* addr, void* func);
void HookInstallCall(void* addr, void* func);
void CreateJump(void* from, void* to, BYTE* jmpArray);
void HookInstallJump(void* addr, void* jmpDest, int jmpCodeSize);

inline
void MemCpy(void* dest, const void* src, unsigned int size)
{
  memcpy(dest, src, size);
}

inline
void MemSet(void* dest, int value, unsigned int size)
{
  memset(dest, value, size);
}

template<class T, class U>
void MemPut(U ptr, const T value)
{
  *(T*)ptr = value;
}

template<class T, class U>
void MemSub(U ptr, const T value)
{
  *(T*)ptr -= value;
}

template<class T, class U>
void HookInstallInject(U from, const T* code, const int size)
{
  for(int i = 0; i < size; i++)
    {
      ((T*)from)[i] = code[i];
    }
}

#endif
