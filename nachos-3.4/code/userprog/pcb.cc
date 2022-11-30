#include "pcb.h"
#include "system.h"

void StartProcess_2(int id);

PCB::PCB(int id)
{
    this->parentID = currentThread->processID;
    this->joinsem = new Semaphore("joinsem", 0);
    this->exitsem = new Semaphore("joinsem", 0);
    this->multex = new Semaphore("joinsem", 1);
    this->thread = NULL;
}

PCB::~PCB()
{
    if (joinsem)
        delete joinsem;
    if (exitsem)
        delete exitsem;
    if (multex)
        delete multex;
    if (thread)
    {
        this->thread->Finish();
    }
}

// Tạo 1 thread mới có tên là filename và process là pid
int PCB::Exec(char *filename, int pid)
{
    this->multex->P();
    this->thread = new Thread(filename);
    if (!thread)
    {
        printf("\nCan't create thread, out of memory");
        this->multex->V();
        return -1;
    }

    this->thread->processID = pid;
    this->parentID = currentThread->processID;
    this->thread->Fork(StartProcess_2, (int)filename);

    return this->pid;
}