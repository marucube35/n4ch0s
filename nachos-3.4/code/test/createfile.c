#include "syscall.h"
#define MAX_LENGTH 255

int main() {
	// Call for enter file name
	int success;
	char buffer[MAX_LENGTH];
	PrintString("Enter file name: \n");
    	ReadString(buffer, MAX_LENGTH);
    	PrintString("You entered: ");
    	PrintString(buffer);
    	PrintChar('\n');
	success = CreateFile(buffer);
	if (success == -1) {
		return -1;
	}
	return 0;
}
