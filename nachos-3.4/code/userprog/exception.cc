// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "syscall.h"
#include "system.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

char *UserToSystem(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // need for terminal string

    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);

    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;

        if (oneChar == 0)
            break;
    }

    return kernelBuf;
}

int SystemToUser(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;

    int i = 0;
    int oneChar = 0;

    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);

    return i;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case NoException:
        return;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG('a', "Shutdown, initiated by user program.\n");
            printf("Shutdown, initiated by user program.\n");

            interrupt->Halt();
            break;
        case SC_Create:
        {
            int virtAddr;
            char *filename;

            DEBUG('a', "SC_Create call ...\n");
            DEBUG('a', "Reading virtual address of filename\n");

            // Lấy tham số tên tập tin từ thanh ghi r4
            virtAddr = machine->ReadRegister(4);
            DEBUG('a', "Reading filename.\n");

            // Lấy dữ liệu từ vùng nhớ thông qua địa chỉ ảo
            int MaxFileLength = 32;
            filename = UserToSystem(virtAddr, MaxFileLength + 1);

            // Trường hợp không đủ vùng nhớ để lưu tên file
            if (filename == NULL)
            {
                DEBUG('a', "Not enough memory in system.\n");
                printf("Not enough memory in system.\n");

                machine->WriteRegister(2, -1); // trả về giá trị -1 cho thanh ghi r2 (lỗi)
                delete filename;
                return;
            }

            DEBUG('a', "Finish reading filename.\n");
            printf("\nCreate file: %s\n", filename);

            // Trường hợp không thể tạo file
            if (!fileSystem->Create(filename, 0))
            {
                printf("Error create file '%s'\n", filename);

                machine->WriteRegister(2, -1); // trả về giá trị -1 cho thanh ghi r2 (lỗi)
                delete filename;
                return;
            }

            machine->WriteRegister(2, 0); // trả về giá trị 0 cho thanh ghi r2 (thành công)
            delete filename;
            break;
        }
        default:
            printf("\n Unexpected user mode exception (%d %d)", which,
                   type);
            interrupt->Halt();
        }
    }
}
