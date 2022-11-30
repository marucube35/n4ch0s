#include "stable.h"

STable::STable()
{
    bm = new BitMap(MAX_SEMAPHORE);

    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        semTab[i] = NULL;
    }
}

STable::~STable()
{
    if (bm)
        delete bm;

    delete[] semTab;
}

int STable::Create(char *name)
{
    if (name)
    {
       
    }
    else
    {
        
    }

    return -1;
}