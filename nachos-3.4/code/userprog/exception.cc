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

int MaxNameLength = 32;

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
        printf("\n[Log]: Everything ok!\n");
        return;
    case PageFaultException:
        printf("\n[Error]: No valid translation found\n");
        interrupt->Halt();
    case ReadOnlyException:
        printf("\n[Error]: Write attempted to page marked  'read - only'\n");
        interrupt->Halt();
    case BusErrorException:
        printf("\n[Error]: Translation resulted in an invalid physical address\n");
        interrupt->Halt();
    case AddressErrorException:
        printf("\n[Error]: Unaligned reference or one that was beyond the end of the address space\n");
        interrupt->Halt();
    case OverflowException:
        printf("\n[Error]: Integer overflow in add or sub.\n");
        interrupt->Halt();
    case IllegalInstrException:
        printf("\n[Error]: Unimplemented or reserved instr.\n");
        interrupt->Halt();
    case NumExceptionTypes:
        printf("\n[Error]: Number exception types\n");
        interrupt->Halt();
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG('a', "Shutdown, initiated by user program.\n");
            printf("Shutdown, initiated by user program.\n");

            interrupt->Halt();
            break;
        }
        case SC_Exec:
        {
            int MaxProgramLength = 32;
            int virtAddr = machine->ReadRegister(4);
            char *filename = UserToSystem(virtAddr, MaxProgramLength + 1);

            //* Tr?????ng h???p kh??ng ????? v??ng nh??? ????? l??u t??n ch????ng tr??nh
            if (filename == NULL)
            {
                printf("\n[SC_Exec]: Not enough memory in system.\n");

                machine->WriteRegister(2, -1); // tr??? v??? gi?? tr??? -1 cho thanh ghi r2 (l???i)

                delete filename;
                break;
            }

            //* T???o ti???n tr??nh m???i
            int pid = pTab->ExecUpdate(filename);
            machine->WriteRegister(2, pid); // tr??? v??? gi?? tr??? pid cho thanh ghi r2 (th??nh c??ng)

            delete filename;
            break;
        }
        case SC_Join:
        {
            //* ?????c id c???a ti???n tr??nh c???n Join t??? thanh ghi r4
            int pid = machine->ReadRegister(4);

            //* G???i th???c hi???n pTab->JoinUpdate(id)
            int exitcode = pTab->JoinUpdate(pid);

            machine->WriteRegister(2, exitcode);

            break;
        }
        case SC_Exit:
        {
            //* ?????c exitcode t??? thanh ghi r4
            int exitstatus = machine->ReadRegister(4);

            if (exitstatus != 0)
                break;

            //* G???i th???c hi???n pTab->ExitUpdate(exitstatus)
            pTab->ExitUpdate(exitstatus);

            machine->WriteRegister(2, exitstatus);
            currentThread->Finish();
            break;
        }
        case SC_CreateSemaphore:
        {
            int virtAddr = machine->ReadRegister(4);
            int initVal = machine->ReadRegister(5);
            char* name = UserToSystem(virtAddr, MaxNameLength + 1);

            int index = semTab->Create(name, initVal);
            printf("\n[SC_CreateSemaphore]: Semaphore created at index: %d\n", index);

            machine->WriteRegister(2, index);
            AdvanceProgramCounter();
            break;
        }
        case SC_Signal:
        {
            int virtAddr = machine->ReadRegister(4);
            char *name = UserToSystem(virtAddr, MaxNameLength + 1);
            printf("\n[SC_Signal]: Semaphore name: %s, current value: %d\n", name, semTab->GetValue(name));

            if (!semTab->IsExist(name))
            {
                printf("\n[SC_Signal]: Semaphore is not exist.\n");
                machine->WriteRegister(2, -1);
                AdvanceProgramCounter();
                break;
            }

            int result = semTab->Signal(name);
            machine->WriteRegister(2, result);
            AdvanceProgramCounter();
            break;
        }
        case SC_Wait:
        {
            int virtAddr = machine->ReadRegister(4);
            char *name = UserToSystem(virtAddr, MaxNameLength + 1);
            printf("\n[SC_Wait]: Semaphore name: %s, current value: %d\n", name, semTab->GetValue(name));

            if (!semTab->IsExist(name))
            {
                printf("\n[SC_Wait]: Semaphore is not exist.\n");
                machine->WriteRegister(2, -1);
                AdvanceProgramCounter();
                break;
            }

            int result = semTab->Wait(name);
            machine->WriteRegister(2, result);
            AdvanceProgramCounter();
            break;
        }
        case SC_Create:
        {
            int virtAddr;
            char *fileName;

            // L???y tham s??? t??n t???p tin t??? thanh ghi r4
            virtAddr = machine->ReadRegister(4);

            // L???y d??? li???u t??? v??ng nh??? th??ng qua ?????a ch??? ???o
            fileName = UserToSystem(virtAddr, MaxNameLength + 1);

            // Tr?????ng h???p kh??ng ????? v??ng nh??? ????? l??u t??n file
            if (fileName == NULL)
            {
                printf("\n[SC_Create]: Not enough memory in system.\n");

                machine->WriteRegister(2, -1); // tr??? v??? gi?? tr??? -1 cho thanh ghi r2 (l???i)

                delete fileName;
                break;
            }

            printf("\n[SC_Create]: Create file: %s\n", fileName);

            // Tr?????ng h???p kh??ng th??? t???o file
            if (!fileSystem->Create(fileName, 0))
            {
                printf("\n[SC_Create]: Error create file '%s'\n", fileName);

                machine->WriteRegister(2, -1); // tr??? v??? gi?? tr??? -1 cho thanh ghi r2 (l???i)
                delete fileName;
                break;
            }

            machine->WriteRegister(2, 0); // tr??? v??? gi?? tr??? 0 cho thanh ghi r2 (th??nh c??ng)
            delete fileName;
            break;
        }
        case SC_Open:
        {
            int virtAddr = machine->ReadRegister(4);
            int type = machine->ReadRegister(5);
            char *filename = UserToSystem(virtAddr, MaxNameLength);

            int freeSlot = fileSystem->FindFreeSlot();
            if (freeSlot != -1)
            {
                if (type == 0 || type == 1)
                {
                    if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL)
                    {
                        machine->WriteRegister(2, freeSlot);
                    }
                }
                else if (type == 2)
                {
                    machine->WriteRegister(2, 0);
                }
                else
                {
                    machine->WriteRegister(2, 1);
                }

                delete[] filename;
                break;
            }

            machine->WriteRegister(2, -1);
            delete[] filename;
            break;
        }
        case SC_Close:
        {
            int fid = machine->ReadRegister(4);
            if (fid >= 0 && fid <= 14)
            {
                if (fileSystem->openf[fid])
                {
                    delete fileSystem->openf[fid];
                    fileSystem->openf[fid] = NULL;
                    machine->WriteRegister(2, 0);
                    break;
                }
            }

            machine->WriteRegister(2, -1);
            break;
        }
        case SC_Read:
        {
            int virtAddr = machine->ReadRegister(4);
            int charcount = machine->ReadRegister(5);
            int id = machine->ReadRegister(6);
            int OldPos;
            int NewPos;
            char *buf;
            if (id < 0 || id > 14)
            {
                printf("\n[SC_Read]: ID is invalid.\n");
                machine->WriteRegister(2, -1);
                break;
            }
            if (fileSystem->openf[id] == NULL)
            {
                printf("\n[SC_Read]: File is not existed\n");
                machine->WriteRegister(2, -1);
                break;
            }
            if (fileSystem->openf[id]->type == 3)
            {
                printf("\n[SC_Read]: Cannot read stdout file.\n");
                machine->WriteRegister(2, -1);
                break;
            }
            OldPos = fileSystem->openf[id]->GetCurrentPos();
            buf = UserToSystem(virtAddr, charcount);
            if (fileSystem->openf[id]->type == 2)
            {

                int size = gSynchConsole->Read(buf, charcount);
                SystemToUser(virtAddr, size, buf);
                machine->WriteRegister(2, size);
                delete buf;
                break;
            }
            if ((fileSystem->openf[id]->Read(buf, charcount)) > 0)
            {
                NewPos = fileSystem->openf[id]->GetCurrentPos();
                SystemToUser(virtAddr, NewPos - OldPos, buf);
                machine->WriteRegister(2, NewPos - OldPos);
            }
            else
            {
                machine->WriteRegister(2, -2);
            }

            delete buf;
            break;
        }
        case SC_Write:
        {
            int virtAddr = machine->ReadRegister(4);
            int charcount = machine->ReadRegister(5);
            int id = machine->ReadRegister(6);
            int OldPos;
            int NewPos;
            char *buf;

            if (id < 0 || id > 14)
            {
                printf("\n[SC_Write]: ID is invalid\n");
                machine->WriteRegister(2, -1);
                break;
            }
            if (fileSystem->openf[id] == NULL)
            {
                printf("\n[SC_Write]: This file is not existed\n");
                machine->WriteRegister(2, -1);
                break;
            }
            if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
            {
                printf("\n[SC_Write]: This file has no writing permission\n");
                machine->WriteRegister(2, -1);
                break;
            }
            OldPos = fileSystem->openf[id]->GetCurrentPos();
            buf = UserToSystem(virtAddr, charcount);
            if (fileSystem->openf[id]->type == 0)
            {
                if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
                {
                    NewPos = fileSystem->openf[id]->GetCurrentPos();
                    machine->WriteRegister(2, NewPos - OldPos);
                    delete buf;
                    break;
                }
            }
            if (fileSystem->openf[id]->type == 3)
            {
                int i = 0;
                while (buf[i] != 0 && buf[i] != '\n')
                {
                    gSynchConsole->Write(buf + i, 1);
                    i++;
                }
                buf[i] = '\n';
                gSynchConsole->Write(buf + i, 1);
                machine->WriteRegister(2, i - 1);
                delete buf;
                break;
            }
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
                        throw "\n[SC_ReadInt]: Invalid input!\n";
                    }
                }

                number = atoi(buffer);
            }
            catch (const char *msg)
            {
                printf("%s", msg);
                delete buffer;
                break;
            }

            machine->WriteRegister(2, number); // tr??? v??? gi?? tr??? cho thanh ghi r2
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

            machine->WriteRegister(2, (int)character[0]); // tr??? v??? gi?? tr??? cho thanh ghi r2
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
            printf("\n[Error]: Unexpected user mode exception (%d %d) \n", which,
                   type);
            interrupt->Halt();
            break;
        }

        AdvanceProgramCounter();
    }
}