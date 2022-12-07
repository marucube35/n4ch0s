#include "copyright.h"
#include "syscall.h"
#define bufferSize 255

int main()
{
    int number;
    char character;
    char buffer[bufferSize];

    // Input/Output
    // Test ReadInt, PrintInt
    PrintString("Enter a number: ");
    PrintChar('\n');
    number = ReadInt();
    PrintString("You entered: ");
    PrintInt(number);
    PrintChar('\n');

    // Test ReadChar, PrintChar
    PrintString("Enter a char: ");
    PrintChar('\n');
    character = ReadChar();
    PrintString("You entered: ");
    PrintChar(character);
    PrintChar('\n');

    // Test ReadString, PrintString
    PrintString("Enter a string: ");
    PrintChar('\n');
    ReadString(buffer, bufferSize);
    PrintString("You entered: ");
    PrintString(buffer);
    PrintChar('\n');
}