#include "copyright.h"
#include "system.h"

char *User2System(int virtAddr, int limit);
int System2User(int virtAddr, int len, char *buffer);
int doSC_Create();
int doSC_Exit();
int doSC_Close();
int doSC_Seek();
int doSC_Read();
int doSC_Write();
int doSC_Open();