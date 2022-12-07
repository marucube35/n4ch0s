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
}

PTable::~PTable()
{
    delete[] pcb;
    delete bm;
    delete bmsem;
}

int PTable::ExecUpdate(char *filename)
{
    //* Tránh tình trạng nạp 2 tiến trình cùng 1 lúc
    bmsem->P();

    //* Kiểm tra sự tồn tại của chương trình "name"
    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL)
    {
        printf("PTable::ExecUpdate: Unable to open file %s\n", filename);
        return -1;
    }

    //* So sánh tên chương trình và tên của currentThread để chắc chắn rằng chương trình này không gọi thực thi chính nó
    if (strcmp(currentThread->getName(), filename) == 0)
    {
        printf("PTable::ExecUpdate: Unable to execute itself %s\n", filename);
        return -1;
    }

    //* Tìm slot trống trong bảng PTable
    int index = GetFreeSlot();
    if (index < 0)
    {
        printf("PTable::ExecUpdate: No enough space for new process %s\n", filename);
        return -1;
    }

    pcb[index] = new PCB(index);
    pcb[index]->parentID = currentThread->pid;
    int pid = pcb[index]->Exec(filename, index);

    //* Thoát khỏi critical section
    bmsem->V();

    //* Trả về kết quả thực thi của PCB->Exec()
    return pid;
}

int PTable::JoinUpdate(int pid)
{
    //* Kiểm tra tính hợp lệ của processID pid
    int index = IsExist(pid);
    if (index < 0)
    {
        printf("PTable::JoinUpdate: Process %d does not exist\n", pid);
        return -1;
    }

    //* Kiểm tra tiến trình gọi Join có phải là cha của tiến trình có processID là id hay không
    if (currentThread->pid != pcb[index]->parentID)
    {
        printf("PTable::JoinUpdate: Process %d is not parent of process %d\n", currentThread->pid, pid);
        return -1;
    }

    //* Tăng numwait
    pcb[index]->IncNumWait();

    //* Gọi JoinWait để chờ tiến trình con thực hiện xong
    pcb[index]->JoinWait();

    int exitcode = pcb[index]->GetExitCode();

    //* Cho phép tiến trình con thoát
    pcb[index]->ExitRelease();

    return exitcode;
}

int PTable::ExitUpdate(int exitcode)
{
    //* Lấy processID của tiến trình hiện tại
    int pid = currentThread->pid;
    if (strcmp(currentThread->getName(), "main") == 0)
    {
        interrupt->Halt();
    }
    else
    {
        //* Đặt exitcode cho tiến trình gọi.
        pcb[pid]->SetExitCode(exitcode);

        //* Giải phóng tiến trình cha đang đợi nó (nếu có).
        pcb[pid]->JoinRelease();

        //* Xin tiến trình cha cho phép thoát.
        pcb[pid]->ExitWait();
    }

    Remove(pid);
    return exitcode;
}

int PTable::GetFreeSlot()
{
    bmsem->P();
    //* Tìm vị trí trống trong bitmap
    int index = bm->Find();
    bmsem->V();

    return index;
}

bool PTable::IsExist(int pid)
{
    for (int i = 0; i < psize; i++)
    {
        if (pcb[i] != NULL && pcb[i]->GetID() == pid)
            return true;
    }

    return false;
}

void PTable::Remove(int pid)
{
    for (int i = 0; i < psize; i++)
    {
        //* Tìm kiếm pid trong mảng pcb và tiến hành xóa nếu tìm thấy
        if (pcb[i] != NULL && pcb[i]->GetID() == pid)
        {
            bmsem->P();

            //* Xóa bit thứ i trong bitmap
            bm->Clear(i);

            //* Xóa tiến trình pid ra khỏi mảng pcb
            delete pcb[i];
            pcb[i] = NULL;
            bmsem->V();

            break;
        }
    }
}

char *PTable::GetFileName(int id)
{
    for (int i = 0; i < psize; i++)
    {
        //* Tìm kiếm pid trong mảng pcb và trả về tên nếu tìm thấy
        if (pcb[i] != NULL && pcb[i]->GetID() == id)
            return pcb[i]->GetFileName();
    }

    return NULL;
}