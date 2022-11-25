#include "copyright.h"
#include "syscall.h"

int main()
{
    int bufferSize = 255;
    char buffer[bufferSize];
    ReadString(buffer, bufferSize);
    PrintString(buffer);
}