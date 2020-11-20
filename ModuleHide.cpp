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
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
