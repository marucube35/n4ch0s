#include "pcb.h"
#include "addrspace.h"
#include "system.h"

void StartProcess_2(int id)
{
    // Lấy tên tiến trình dựa trên id
    char *filename = pTab->GetFileName(id);

    // Cấp phát không gian địa chỉ cho tiến trình
    AddrSpace *space = new AddrSpace(filename);
    currentThread->space = space;

    // TODO: format all log
    if (space == NULL)
    {
        printf("\nPCB::Exec : Can't create AddSpace.");
        return;
    }

    space->InitRegisters(); // set the initial register values
    space->RestoreState();  // load page table register

    machine->Run(); // jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

PCB::PCB(int id)
{
    // Khởi tạo các thông tin của PCB
    this->pid = id;
    this->parentID = 0;

    // Gán parentID của tiến trình là processID của tiểu trình hiện tại
    this->parentID = currentThread->processID;

    // Khởi tạo giá trị của các semapore
    this->joinsem = new Semaphore("joinsem", 0);
    this->exitsem = new Semaphore("joinsem", 0);
    this->multex = new Semaphore("joinsem", 1);

    this->thread = NULL;
}

PCB::~PCB()
{
    // Giải phóng vùng nhớ cho các semaphore và thread
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
    // Tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
    this->multex->P();

    // Tạo thread mới
    this->thread = new Thread(filename);
    if (!thread)
    {
        printf("\nCan't create thread, out of memory");
        this->multex->V();
        return -1;
    }

    // Gán processID của thread là pid của tiến trình
    this->thread->processID = pid;

    // Gán parentID của tiến trình là processID của thread hiện tại
    this->parentID = currentThread->processID;

    // Chạy thread
    this->thread->Fork(StartProcess_2, pid);

    return this->pid;
}