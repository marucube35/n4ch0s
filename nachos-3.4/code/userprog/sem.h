#ifndef SEM_H
#define SEM_H
#include "synch.h"

class Sem
{

private:
    char name[50];
    Semaphore *sem; // Tạo Semaphore để quản lý
public:
    // khởi tạo đối tượng Sem. Gán giá trị ban đầu là null
    // nhớ khởi tạo bm sử dụng
    Sem(char *na, int i)
    {
        strcpy(this->name, na);
        sem = new Semaphore(name, i);
    }
    ~Sem()
    {
        delete sem;
    }
    void Wait()
    {
        sem->P();
    }
    void Signal()
    {
        sem->V();
    }
    char *GetName()
    {
        // hủy các đối tượng đã tạo
        // thực hiện thao tác chờ
        // thực hiện thao tác giải phóng Semaphore
        // Trả về tên của Semaphore
        return name;
    }
};

#endif