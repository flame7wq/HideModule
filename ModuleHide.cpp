// ModuleHide.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
    ULONG       Length;
    BOOLEAN     Initialized;
    PVOID       SsHandle;
    LIST_ENTRY  InLoadOrderModuleList;
    LIST_ENTRY  InMemoryOrderModuleList;
    LIST_ENTRY  InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY  InLoadOrderModuleList;
    LIST_ENTRY  InMemoryOrderModuleList;
    LIST_ENTRY  InInitializationOrderModuleList;
    void*       BaseAddress;
    void*       EntryPoint;
    ULONG       SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG       Flags;
    SHORT       LoadCount;
    SHORT       TlsIndex;
    HANDLE      SectionHandle;
    ULONG       CheckSum;
    ULONG       TimeDateStamp;
}LDR_MODULE, *PLDR_MODULE;

void HideModule(const char* szModuleName)
{
    HMODULE hMod = ::GetModuleHandleA(szModuleName);
    PLIST_ENTRY Head, Cur;
    PPEB_LDR_DATA ldr;
    PLDR_MODULE ldm;

    __asm {
        mov eax, fs: [0x30]
        mov ecx, [eax + 0x0c]
        mov ldr, ecx
    }
    Head = &(ldr->InLoadOrderModuleList);
    Cur = Head->Flink;
    do
    {
        // 宏，CONTAINING_RECORD 根据结构体中的某成员的地址来推算出该结构体整体的地址
        ldm = CONTAINING_RECORD(Cur, LDR_MODULE, InLoadOrderModuleList);

        if (hMod == ldm->BaseAddress)
        {
            ldm->InLoadOrderModuleList.Blink->Flink = ldm->InLoadOrderModuleList.Flink;
            ldm->InLoadOrderModuleList.Flink->Blink = ldm->InLoadOrderModuleList.Blink;
            ldm->InInitializationOrderModuleList.Blink->Flink = ldm->InInitializationOrderModuleList.Flink;
            ldm->InInitializationOrderModuleList.Flink->Blink = ldm->InInitializationOrderModuleList.Blink;
            ldm->InMemoryOrderModuleList.Blink->Flink = ldm->InMemoryOrderModuleList.Flink;
            ldm->InMemoryOrderModuleList.Flink->Blink = ldm->InMemoryOrderModuleList.Blink;
            break;
        }
        Cur = Cur->Flink;
    } while (Head != Cur);
}
int main()
{
    printf("*************任意键隐藏模块**************\n");
    getchar();
    HideModule("kernel32.dll");
    printf("******已成功隐藏模块 Kernel32.dll *******\n");
    getchar();
    MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
        return -1;

    if (Module32First(hModuleSnap, &me32))
    {
        do
        {
            wprintf(TEXT("%s\n"), me32.szExePath);
            printf("    Module address in the process: %p\n", me32.hModule);
        } while (Module32Next(hModuleSnap,&me32));
    }
    CloseHandle(hModuleSnap);
    return 1;
}
