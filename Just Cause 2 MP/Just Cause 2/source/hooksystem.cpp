#include "hooksystem.h"

#define MAX_JUMPCODE_SIZE 50

void HookInstallMethod(void* addr, void* func)
{
  MemPut<void*>(addr, func);
}

void HookInstallCall(void* addr, void* func)
{
  int offset = (int)func - ((int)addr + 5);
  MemPut<BYTE>(addr, 0xE8);
  MemPut<int>((int)addr + 1, offset);
}

void CreateJump(void* from, void* to, BYTE* jmpArray)
{
  jmpArray[0] = 0xE9;
  MemPut<int> (&(jmpArray[1]), (int)to - ((int)from + 5));
}

void HookInstallJump(void* addr, void* jmpDest, int jmpCodeSize)
{
  BYTE jumpByte[MAX_JUMPCODE_SIZE];
  MemSet(jumpByte, 0x90, MAX_JUMPCODE_SIZE);
  CreateJump(addr, jmpDest, jumpByte);
  MemCpy(addr, jumpByte, jmpCodeSize);
}
