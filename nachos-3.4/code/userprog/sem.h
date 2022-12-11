// stable.h
#ifndef SEM_H
#define SEM_H
#include "synch.h"

class Sem
{
private:
    char name[50];
    Semaphore *sem; // Tạo Semaphore để quản lý
public:
    Sem(char *semName, int initVal)
    {
        strcpy(this->name, semName);
        sem = new Semaphore(name, initVal);
    }
    ~Sem() { delete sem; }

    void Wait() { sem->P(); }        // Thực hiện thao tác chờ
    void Signal() { sem->V(); }      // Thực hiện thao tác giải phóng Semaphore
    char *GetName() { return name; } // Trả về tên của Semaphore
    int GetValue() { return sem->getValue(); }
};

#endif