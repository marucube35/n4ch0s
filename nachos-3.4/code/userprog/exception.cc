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

void AdvanceProgramCounter()
{
    // Advance program counters.
    machine->registers[PrevPCReg] = machine->registers[PCReg];
    machine->registers[PCReg] = machine->registers[NextPCReg];
    machine->registers[NextPCReg] = machine->registers[NextPCReg] + 4;
}

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
        printf("[Log]: Everything ok!\n");
        return;
    case PageFaultException:
        printf("[Error]: No valid translation found\n");
        interrupt->Halt();
    case ReadOnlyException:
        printf("[Error]: Write attempted to page marked  'read - only'\n");
        interrupt->Halt();
    case BusErrorException:
        printf("[Error]: Translation resulted in an invalid physical address\n");
        interrupt->Halt();
    case AddressErrorException:
        printf("[Error]: Unaligned reference or one that was beyond the end of the address space\n");
        interrupt->Halt();
    case OverflowException:
        printf("[Error]: Integer overflow in add or sub.\n");
        interrupt->Halt();
    case IllegalInstrException:
        printf("[Error]: Unimplemented or reserved instr.\n");
        interrupt->Halt();
    case NumExceptionTypes:
        printf("[Error]: Number exception types\n");
        interrupt->Halt();
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG('a', "Shutdown, initiated by user program.\n");
            printf("Shutdown, initiated by user program.\n");

            interrupt->Halt();
            break;
        case SC_Exec:
        {
            int virtAddr;
            char *programName;

            DEBUG('a', "SC_Exec call ...\n");
            DEBUG('a', "Reading virtual address of program name\n");

            // Lấy tham số tên chương trình từ thanh ghi r4
            virtAddr = machine->ReadRegister(4);
            DEBUG('a', "Reading program name.\n");

            // Lấy dữ liệu từ vùng nhớ của người dùng (đối số truyền vào)
            // thông qua địa chỉ ảo và sao chép vào vùng nhớ của hệ điều hành (programName)
            int MaxProgramLength = 32;
            programName = UserToSystem(virtAddr, MaxProgramLength + 1);

            // Trường hợp không đủ vùng nhớ để lưu tên chương trình
            if (programName == NULL)
            {
                DEBUG('a', "Not enough memory in system.\n");
                printf("Not enough memory in system.\n");

                machine->WriteRegister(2, -1); // trả về giá trị -1 cho thanh ghi r2 (lỗi)
                delete programName;
                return;
            }

            DEBUG('a', "Finish reading program name.\n");
            printf("\nCreate file: %s\n", programName);

            machine->WriteRegister(2, 0); // trả về giá trị 0 cho thanh ghi r2 (thành công)
            delete programName;
            break;
        }
        case SC_Create:
        {
            int virtAddr;
            char *fileName;

            DEBUG('a', "SC_Create call ...\n");
            DEBUG('a', "Reading virtual address of filename\n");

            // Lấy tham số tên tập tin từ thanh ghi r4
            virtAddr = machine->ReadRegister(4);
            DEBUG('a', "Reading filename.\n");

            // Lấy dữ liệu từ vùng nhớ thông qua địa chỉ ảo
            int MaxFileLength = 32;
            fileName = UserToSystem(virtAddr, MaxFileLength + 1);

            // Trường hợp không đủ vùng nhớ để lưu tên file
            if (fileName == NULL)
            {
                DEBUG('a', "Not enough memory in system.\n");
                printf("Not enough memory in system.\n");

                machine->WriteRegister(2, -1); // trả về giá trị -1 cho thanh ghi r2 (lỗi)
                delete fileName;
                return;
            }

            DEBUG('a', "Finish reading filename.\n");
            printf("\nCreate file: %s\n", fileName);

            // Trường hợp không thể tạo file
            if (!fileSystem->Create(fileName, 0))
            {
                printf("Error create file '%s'\n", fileName);

                machine->WriteRegister(2, -1); // trả về giá trị -1 cho thanh ghi r2 (lỗi)
                delete fileName;
                return;
            }

            machine->WriteRegister(2, 0); // trả về giá trị 0 cho thanh ghi r2 (thành công)
            delete fileName;
            break;
        }
        case SC_ReadInt:
        {
            char *buffer = new char[100];
            int length = gSynchConsole->Read(buffer, 100);
            int number = 0;

            try
            {
                for (int i = 0; i < length; i++)
                {
                    if (buffer[i] < '0' || buffer[i] > '9')
                    {
                        throw "Invalid input!\n";
                    }
                }

                number = atoi(buffer);
            }
            catch (const char *msg)
            {
                printf("%s", msg);
                delete buffer;
                return;
            }

            machine->WriteRegister(2, number); // trả về giá trị cho thanh ghi r2
            delete buffer;
            break;
        }
        case SC_PrintInt:
        {
            int number = machine->ReadRegister(4);
            char *buffer = new char[100];

            int temp = number;
            int numberOfDigits = 0;
            while (temp > 0)
            {
                temp /= 10;
                numberOfDigits++;
            }

            for (int i = numberOfDigits - 1; i >= 0; i--)
            {
                buffer[i] = char(number % 10 + 48);
                number /= 10;
            }

            gSynchConsole->Write(buffer, numberOfDigits);
            break;
        }
        case SC_ReadChar:
        {
            char *character = new char[255];
            int length = gSynchConsole->Read(character, 255);

            machine->WriteRegister(2, (int)character[0]); // trả về giá trị cho thanh ghi r2
            break;
        }
        case SC_PrintChar:
        {
            char character = (char)machine->ReadRegister(4);

            gSynchConsole->Write(&character, 1);
            break;
        }
        case SC_ReadString:
        {
            int virtAddr = machine->ReadRegister(4);
            int bufferSize = machine->ReadRegister(5);
            char *buffer = new char[bufferSize];

            int length = gSynchConsole->Read(buffer, bufferSize);
            SystemToUser(virtAddr, length, buffer);
            delete buffer;
            break;
        }
        case SC_PrintString:
        {
            int bufferSize = 255;
            int virtAddr = machine->ReadRegister(4);

            char *buffer = UserToSystem(virtAddr, bufferSize + 1);
            gSynchConsole->Write(buffer, bufferSize);
            delete buffer;
            break;
        }

        default:
            printf("\nUnexpected user mode exception (%d %d) \n", which,
                   type);
            interrupt->Halt();
            break;
        }

        AdvanceProgramCounter();
    }
}
