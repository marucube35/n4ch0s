#include "pcb.h"
#include "system.h"
#include "addrspace.h"

void StartProcess_2(int pid);

PCB::PCB(int id)
{
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    multex = new Semaphore("multex", 1);
    thread = NULL;

    exitcode = numwait = 0;
    pid = id;
}

PCB::~PCB()
{
    delete joinsem;
    delete exitsem;
    delete multex;
}

int PCB::Exec(char *name, int pid)
{
    printf("---------------------------------\n");

    //* Tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
    //* Bắt đầu critical section
    multex->P();

    printf("PCB::Exec: Creating thread %s\n", name);

    //* Tạo thread mới có tên là name
    thread = new Thread(name);

    //* Kiểm tra xem thread có được tạo thành công hay không
    if (!thread)
    {
        printf("PCB::Exec: Not enough memory for creating thread %s\n", name);
        //* Kết thúc critical section
        multex->V();
    }

    //* Đặt processID của tiến trình mới tạo là pid
    thread->pid = pid;

    //* Đặt parentID của tiến trình mới tạo là processID của tiến trình gọi thực thi Exec
    thread->parentID = currentThread->pid;

    printf("PCB::Exec: Thread %s created with pid %d\n", pTab->GetFileName(pid), pid);

    //* Gọi thực thi Fork với hàm StartProcess_2
    thread->Fork(StartProcess_2, pid);

    //* Kết thúc critical section
    multex->V();

    return pid;
}

void PCB::IncNumWait()
{
    multex->P();
    numwait++;
    multex->V();
}

void PCB::DecNumWait()
{
    multex->P();
    numwait--;
    multex->V();
}

void PCB::SetFileName(char *fn)
{
    //* Bắt buộc phải cấp phát vùng nhớ mới
    filename = new char[strlen(fn) + 1];
    //* và sao chép giá trị
    strcpy(filename, fn);
}

void StartProcess_2(int pid)
{
    printf("---------------------------------\n");
    printf("StartProcess_2: Executing process %d\n", pid);

    //* Lấy tên tiến trình dựa trên pid
    char *filename = pTab->GetFileName(pid);

    printf("StartProcess_2: Executing file %s\n", filename);

    //* Khởi tạo vùng nhớ để lưu code
    AddrSpace *space = new AddrSpace(filename);
    currentThread->space = space;

    space->InitRegisters(); // set the initial register values
    space->RestoreState();  // load page table register

    machine->Run(); // jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}
