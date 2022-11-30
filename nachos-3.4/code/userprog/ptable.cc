#include "ptable.h"
#include "system.h"

PTable::PTable(int size)
{
    if (size < 0 || size > MAX_PROCESS)
        return;

    // Khởi tạo 'size' đối tượng pcb để lưu 'size' process.
    this->psize = size;

    // Cấp phát vùng nhớ cho bm và bmsem
    this->bm = new BitMap(size);
    this->bmsem = new Semaphore("bmsem", 1);

    // Gán giá trị ban đầu là null.
    for (int i = 0; i < size; i++)
        pcb[i] = NULL;
}

PTable::~PTable()
{
    // Giải phóng vùng nhớ của bm và bmsem
    if (bm)
        delete bm;
    if (bmsem)
        delete bmsem;

    // Giải phóng vùng nhớ của các pcb
    delete[] pcb;
}

int PTable::ExecUpdate(char *name)
{
    bmsem->P();

    // Kiểm tra tính hợp lệ của tên tiến trình
    if (!name)
    {
        printf("\n[PTable.Exec] Input name is empty\n");
        bmsem->V();
        return -1;
    }

    // Kiểm tra sự tồn tại của tiến trình
    OpenFile *executable = fileSystem->Open(name);
    if (executable == NULL)
    {
        printf("\n[PTable.Exec] %s does not exists\n", name);
        bmsem->V();
        return -1;
    }

    // So sánh tên chương trình và tên của currentThread để chắc chắn rằng chương trình này không gọi thực thi chính nó
    if (strcmp(name, currentThread->getName()) == 0)
    {
        printf("\n[PTable.Exec] Can not execute itself\n");
        bmsem->V();
        return -1;
    }

    // Tìm chỗ trống trong bảng để lưu thông tin cho tiến trình mới
    int freeSlotIndex = this->GetFreeSlot();
    if (freeSlotIndex < 0)
    {
        printf("\n[PTable.Exec] There is no free slot.\n");
        bmsem->V();
        return -1;
    }

    // Khởi tạo tiến trình mới
    pcb[freeSlotIndex] = new PCB(freeSlotIndex);
    pcb[freeSlotIndex]->SetFileName(name);
    pcb[freeSlotIndex]->parentID = currentThread->processID;

    // Gọi thực thi phương thức Exec của lớp PCB
    int pid = pcb[freeSlotIndex]->Exec(name, freeSlotIndex);

    bmsem->V();

    return pid;
}

int PTable::GetFreeSlot()
{
    return bm->Find();
}

char *PTable::GetFileName(int id)
{
    if (id < 0 || id >= psize)
        return NULL;

    return pcb[id]->GetFileName();
}