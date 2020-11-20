# HideModule
HideModule 基于断链

## TEB
Thread Information Block

记录相关的线程信息，每一个线程都有自己的 TEB, FS[0] 即是当前线程的 TEB

    mov eax, fs: [0]

## PEB 
Process Information Block 

存放进程信息，每个进程都有自己的 PEB 信息，TEB 偏移 0x30 即当前进程的PEB

    mov eax, fs: [0x30]
    mov PEB, eax

***
TEB 与 PEB 均在三环用户空间，结构体中有双向链表记录着模块的加载信息，很多基于API查询的方式都是在查这些结构体，此处的断链就是在链表中抹去这一项，即将此项从链表断开。
