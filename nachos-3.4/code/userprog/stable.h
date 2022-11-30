#ifndef STABLE_H
#define STABLE_H
#define MAX_SEMAPHORE 10
#include "bitmap.h"
#include "sem.h"
#include "synch.h"

class STable
{
private:
    BitMap *bm;                 // quản lý slot trống
    Sem *semTab[MAX_SEMAPHORE]; // quản lý tối đa 10 đối tượng Sem

public:
    STable();
    ~STable();                // hủy các đối tượng đã tạo
    int Create(char *name);   // kiểm tra Semaphore “name” chưa tồn tại thì tạo Semaphore mới. Ngược lại, báo lỗi.
    int Wait(char *name);     // Nếu tồn tại Semaphore “name” thì gọi this->P() để thực thi. Ngược lại, báo lỗi.
    int Signal(char *name);   // Nếu tồn tại Semaphore “name” thì gọi this->V() để thực thi. Ngược lại, báo lỗi.
    int FindFreeSlot(int id); // Tìm slot trống.
};

#endif