// stable.h
#ifndef STABLE_H
#define STABLE_H
#include "bitmap.h"
#include "synch.h"
#include "sem.h"
#define MAX_SEMAPHORE 10

class STable
{
private:
    BitMap *bm;
    Sem *semTab[MAX_SEMAPHORE];

public:
    STable();
    ~STable();

    int IsExist(char *name);          // Kiểm tra xem Semaphore “name” đã tồn tại chưa. Chưa thì trả về -1
    int Create(char *name, int init); // Kiểm tra Semaphore “name” chưa tồn tại thì tạo
    int Wait(char *name);             // Nếu tồn tại Semaphore “name” thì gọi this->P() để thực thi.
    int Signal(char *name);           // Nếu tồn tại Semaphore “name” thì gọi this->V() để thực thi.
};
#endif