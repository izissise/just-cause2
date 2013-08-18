#include "cheat.h"
#include "hooksystem.h"

void SetMultiRope(bool b)
{
    if(b)
    {
        //activate multirope !
        MemPut<BYTE>(0x0067024C, 0xEB); /*jmp 0067026B*/
        MemPut<BYTE>(0x00793CF3, 0xEB); /*jmp 00793D11*/
        MemPut<BYTE>(0x007D0D4B, 0xEB); /*jmp 007D0D52*/
    }
    else
    {
        //desactivate it !
        MemPut<BYTE>(0x0067024C, 0x7C);
        MemPut<BYTE>(0x00793CF3, 0x74);
        MemPut<BYTE>(0x007D0D4B, 0x75);
    }
}

void SetStrongRope(bool b)
{
    if(b)
    {
        //activate multirope !
        MemPut<BYTE>(0x005B87C0, 0xEB); /*jmp 005B87DF*/
        MemPut<BYTE>(0x0074CEFD, 0xEB); /*jmp 00793D11*/
        MemPut<BYTE>(0x0074CEFE, 0x29);
    }
    else
    {
        //desactivate it !
        MemPut<BYTE>(0x005B87C0, 0x76); /*jmp 005B87DF*/
        MemPut<BYTE>(0x0074CEFD, 0x75); /*jmp 00793D11*/
        MemPut<BYTE>(0x0074CEFE, 0x05);
    }
}

void SetGodmod(bool b)
{
    int adrrHOOKGodmod = 0x007250ED;
    const int codeSize = 6;
    BYTE godmodCodeEnab[codeSize + 1] = "\xE9\x42\x01\x00\x00\x90"; /*jmp 00725234 nop*/
    BYTE godmodCodeDisa[codeSize + 1] = "\x0F\x86\x41\x01\x00\x00";

    if(b)
    {
        //activate godmod !
        HookInstallInject(adrrHOOKGodmod, godmodCodeEnab, codeSize);
    }
    else
    {
        //desactivate it !
        HookInstallInject(adrrHOOKGodmod, godmodCodeDisa, codeSize);
    }

}

void SetNoWanted(bool b)
{
    const int codeSize = 3;
    BYTE codeDisa1[codeSize+1] = "\x0F\x2F\xC1"; /*comiss xmm0,xmm1*/
    BYTE codeDisa2[codeSize+1] = "\x0F\x28\xC2"; /*movaps xmm0,xmm2*/
    BYTE codeEna[codeSize+1] = "\x0F\x57\xC0"; /*xorps xmm0,xmm0*/

    if(b)
    {
        //activate no wanted !
        HookInstallInject(0x0052324E,codeEna,codeSize);
        HookInstallInject(0x0052327B,codeEna,codeSize);
        HookInstallInject(0x0051567B,codeEna,codeSize);
        HookInstallInject(0x005156AC,codeEna,codeSize);
        HookInstallInject(0x0072A546,codeEna,codeSize);
        HookInstallInject(0x0072A575,codeEna,codeSize);
        HookInstallInject(0x0076DF7A,codeEna,codeSize);
        HookInstallInject(0x0076DFA8,codeEna,codeSize);
    }
    else
    {
        //desactivate it !
        HookInstallInject(0x0052324E,codeDisa1,codeSize);
        HookInstallInject(0x0052327B,codeDisa2,codeSize);
        HookInstallInject(0x0051567B,codeDisa1,codeSize);
        HookInstallInject(0x005156AC,codeDisa2,codeSize);
        HookInstallInject(0x0072A546,codeDisa1,codeSize);
        HookInstallInject(0x0072A575,codeDisa2,codeSize);
        HookInstallInject(0x0076DF7A,codeDisa1,codeSize);
        HookInstallInject(0x0076DFA8,codeDisa2,codeSize);
    }
}

int RETN_Infinite_ammo_no_reload = 0x007F7FFE;
NAKED infiniteAmmoNoReload()
{
    ASM
    {
        mov [esi+0x148],0x3E7
        mov eax,[esi+0x148]
        jmp RETN_Infinite_ammo_no_reload
    }
}

void SetInfiniteAmmo(bool b)
{
    const int codeSize = 6;
    BYTE codeDisa[codeSize+1] = "\x8B\x86\x48\x01\x00\x00";

    if(b)
    {
        //activate infinite_ammo !
        HookInstallJump((void*)0x007F7FF8, infiniteAmmoNoReload, codeSize);
    }
    else
    {
        //desactivate it !
        HookInstallInject(0x007F7FF8, codeDisa, codeSize);
    }
}


int RETN_Infinite_grenade = 0x00614EAA;
NAKED infiniteGrenade()
{
    ASM
    {
        mov [eax+ebx*4+0xD4],0x63
        cmp dword ptr [eax+ebx*4+0xD4],00
        jmp RETN_Infinite_grenade
    }
}

void SetInfiniteGrenade(bool b)
{
    const int codeSize = 8;
    BYTE codeDisa[codeSize+1] = "\x83\xBC\x98\xD4\x00\x00\x00\x00";

    if(b)
    {
        //activate infinite_grenade !
        HookInstallJump((void*)0x00614EA2, infiniteGrenade,codeSize);
    }
    else
    {
        //desactivate it !
        HookInstallInject(0x00614EA2, codeDisa, codeSize);
    }
}

int RETN_Infinite_hack_time = 0x005B38DA;
NAKED infiniteHackTime()
{
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}
    ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop} ASM {nop}

    ASM
    {
        jmp RETN_Infinite_hack_time
    }
}

void SetInfiniteHackTime(bool b)
{
    const int codeSize = 5;
    BYTE codeDisa[codeSize + 1] = "\xF3\x0F\x5E\x46\x44";

    if(b)
    {
        //activate infinite_grenade !
        HookInstallJump((void*)0x005B38D5, infiniteHackTime, codeSize);
    }
    else
    {
        //desactivate it !
        HookInstallInject(0x005B38D5, codeDisa, codeSize);
    }
}
