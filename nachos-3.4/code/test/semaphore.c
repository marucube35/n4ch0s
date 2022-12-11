#include "syscall.h"

int main()
{
    int i = 0;
    int semID = CreateSemaphore("sem", 1);
    Wait("sem");
    Signal("sem");

}
