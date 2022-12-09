#include "syscall.h"
#define MAX_LENGTH 255
typedef int OpenFileID;

int main()
{
	int status;
	OpenFileID file;
	char filename[MAX_LENGTH];
	char fileBuffer[MAX_LENGTH];

	PrintString("Enter file name: \n");
	ReadString(filename, MAX_LENGTH);

	// Open filename
	file = Open(filename, 0);
	// If open successes
	if (file != -1)
	{
		Write("This is just a test\n", 30, file);
		status = Close(file);
		if (status != 0)
			return 1;
	}
	file = Open(filename, 0);
	// If open successes
	if (file != -1)
	{
		Read(fileBuffer, MAX_LENGTH, file);
		status = Close(file);
		if (status != 0)
			return 1;
	}
	PrintString(fileBuffer);
	return 0;
}
