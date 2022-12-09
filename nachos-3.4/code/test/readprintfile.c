#include "syscall.h"
#define MAX_LENGTH 255
typedef int OpenFileID;

int main() {
	int status;
	OpenFileID file;
	char buffer[MAX_LENGTH];
	// Open test.txt
	file = Open("myfile.txt", 0);
	// If open successes
	if (file != -1) {
	Write("This is just a test\n", 30, file);
	status = Close(file);
	if (status != 0) return 1;
}
	file = Open("myfile.txt", 0);
	// If open successes
	if (file != -1) {
	Read(buffer, MAX_LENGTH, file);
	status = Close(file);
	if (status != 0) return 1;
}
	PrintString(buffer);
	return 0;
}
