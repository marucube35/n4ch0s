#include "ptable.h"
#include "openfile.h"
#include "system.h"

PTable::PTable(int size)
{
    psize = size;

    for (int i = 0; i < size; i++)
    {
        //* Khởi tạo giá trị ban đầu cho các pcb là NULL
        pcb[i] = NULL;
    }

    bm = new BitMap(size);
    bmsem = new Semaphore("bmsem", 1);

    //* Khởi tạo pcb cho tiến trình main
    bm->Mark(0);
    pcb[0] = new PCB(0);
    pcb[0]->SetFileName(currentThread->getName());
}

PTable::~PTable()
{
    for (int i = 0; i < psize; i++)
    {
        if (pcb[i] != NULL)
        {
            delete pcb[i];
            pcb[i] = NULL;
        }
    }
    delete bm;
    delete bmsem;
}

int PTable::ExecUpdate(char *filename)
{
    printf("---------------------------------\n");
    printf("PTable::ExecUpdate: Current thread name: %s\n", currentThread->getName());
    printf("PTable::ExecUpdate: Current thread pid: %d\n", currentThread->pid);

    //* Tránh tình trạng nạp 2 tiến trình cùng 1 lúc
    bmsem->P();

    //* Kiểm tra sự tồn tại của chương trình "name"
    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL)
    {
        printf("PTable::ExecUpdate: Unable to open file %s\n", filename);
        return -1;
    }

    printf("PTable::ExecUpdate: Executing file %s\n", filename);

    //* So sánh tên chương trình và tên của currentThread để chắc chắn rằng chương trình này không gọi thực thi chính nó
    if (strcmp(currentThread->getName(), filename) == 0)
    {
        printf("PTable::ExecUpdate: Unable to execute itself %s\n", filename);
        bmsem->V();
        return -1;
    }

    //* Tìm slot trống trong bảng PTable
    int pid = GetFreeSlot();
    if (pid < 0)
    {
        printf("PTable::ExecUpdate: No enough space for new process %s\n", filename);
        bmsem->V();
        return -1;
    }

    printf("PTable::ExecUpdate: Create new process with pid %d\n", pid);

    pcb[pid] = new PCB(pid);
    pcb[pid]->SetFileName(filename);
    pcb[pid]->parentID = currentThread->pid;

    printf("PTable::ExecUpdate: Executing file %s with pid %d\n", filename, pid);

    pcb[pid]->Exec(filename, pid);

    //* Kết thúc critical section
    bmsem->V();

    //* Trả về kết quả thực thi của PCB->Exec()
    return pid;
}

int PTable::JoinUpdate(int pid)
{
    printf("---------------------------------\n");
    printf("PTable::ExecUpdate: Current thread name: %s\n", currentThread->getName());
    printf("PTable::ExecUpdate: Current thread pid: %d\n", currentThread->pid);

    //* Kiểm tra tính hợp lệ của processID pid
    if (!IsExist(pid))
    {
        printf("PTable::JoinUpdate: Process %d does not exist\n", pid);
        return -1;
    }

    printf("PTable::JoinUpdate: Process %d exists\n", pid);

    //* Kiểm tra tiến trình gọi Join có phải là cha của tiến trình có processID là id hay không
    if (currentThread->pid != pcb[pid]->parentID)
    {
        printf("PTable::JoinUpdate: Process %d is not parent of process %d\n", currentThread->pid, pid);
        return -1;
    }

    printf("PTable::JoinUpdate: Process %d is parent of process %d\n", currentThread->pid, pid);
    printf("PTable::JoinUpdate: Process %d is being joined\n", pid);

    //* Tăng numwait
    pcb[pcb[pid]->parentID]->IncNumWait();

    //* Gọi JoinWait để chờ tiến trình con thực hiện xong
    pcb[pid]->JoinWait();

    int exitcode = pcb[pid]->GetExitCode();

    //* Cho phép tiến trình con thoát
    pcb[pid]->ExitRelease();

    return exitcode;
}

int PTable::ExitUpdate(int exitcode)
{
    printf("---------------------------------\n");
    printf("PTable::ExecUpdate: Current thread name: %s\n", currentThread->getName());
    printf("PTable::ExecUpdate: Current thread pid: %d\n", currentThread->pid);

    //* Lấy processID của tiến trình hiện tại
    int pid = currentThread->pid;
    if (pid == 0)
    {
        printf("PTable::ExitUpdate: Exit main process\n");
        interrupt->Halt();
        return 0;
    }
    if (!IsExist(pid))
    {
        printf("PTable::ExitUpdate: Process %d does not exist\n", pid);
        return -1;
    }

    printf("PTable::ExitUpdate: Process %d is being exited\n", pid);

    //* Đặt exitcode cho tiến trình gọi.
    pcb[pid]->SetExitCode(exitcode);
    pcb[pcb[pid]->parentID]->DecNumWait();

    //* Giải phóng tiến trình cha đang đợi nó (nếu có).
    pcb[pid]->JoinRelease();

    //* Xin tiến trình cha cho phép thoát.
    pcb[pid]->ExitWait();

    Remove(pid);
    return exitcode;
}

int PTable::GetFreeSlot()
{
    //* Tìm vị trí trống trong bitmap
    return bm->Find();
}

bool PTable::IsExist(int pid)
{
    //* Kiểm tra tính hợp lệ của pid
    return bm->Test(pid);
}

void PTable::Remove(int pid)
{
    bmsem->P();
    //* Xóa bit thứ i trong bitmap
    bm->Clear(pid);

    //* Xóa tiến trình pid ra khỏi mảng pcb
    if (pcb[pid])
    {
        delete pcb[pid];
        pcb[pid] = NULL;
    }
    bmsem->V();
}

char *PTable::GetFileName(int id)
{
    //* Tìm kiếm pid trong mảng pcb và trả về tên nếu tìm thấy
    return pcb[id]->GetFileName();
}

void PTable::Print()
{
    bm->Print();
}
