#include "stable.h"

STable::STable()
{
    bm = new BitMap(MAX_SEMAPHORE);
}

STable::~STable()
{
    delete bm;
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (semTab[i] != NULL)
            delete semTab[i];
    }
}

int STable::IsExist(char *name)
{
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0)
            return i;
    }

    return -1;
}

int STable::Create(char *name, int init)
{
    //* Kiểm tra semaphore "name" có tồn tại chưa
    if (IsExist(name))
        return -1;

    //* Kiểm tra slot trống
    int index = bm->Find();
    if (index == -1)
        return -1;
    
    semTab[index] = new Sem(name, init);
    return 0;
}

int STable::Wait(char *name)
{
    int index = IsExist(name);
    if (index > 0)
    {
        semTab[index]->Wait();
        return 0;
    }
    else
        return -1;
}

int STable::Signal(char *name)
{
    int index = IsExist(name);

    if (index != -1)
    {
        semTab[index]->Signal();
        return 0;
    }
    else
        return -1;
}