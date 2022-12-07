#include "ptable.h"

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
    for (int i = 0; i < MAX_PROCESS; i++)
        delete[] pcb[i];
    delete[] pcb;
    delete bm;
    delete bmsem;
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

char* PTable::GetFileName(int id)
{
    for (int i = 0; i < psize; i++)
    {
        //* Tìm kiếm pid trong mảng pcb và trả về tên nếu tìm thấy
        if (pcb[i] != NULL && pcb[i]->GetID() == id)
            return pcb[i]->GetFileName();
    }

    return NULL;
}