#include "pcb.h"

PCB::PCB()
{
    joinsem = new Semaphore("joinsem", 1);
    exitsem = new Semaphore("exitsem", 1);
    multex = new Semaphore("multex", 1);
    exitcode = 0;
    numwait = 0;
}

PCB::PCB(int id)
{
    joinsem = new Semaphore("joinsem", 1);
    exitsem = new Semaphore("exitsem", 1);
    multex = new Semaphore("multex", 1);
    exitcode = 0;
    numwait = 0;
    parentID = id;
}

PCB::~PCB()
{
    delete joinsem;
    delete exitsem;
    delete multex;
}