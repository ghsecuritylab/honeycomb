#include "GUI.h"

//设置EMWIN内存大小
#define GUI_NUMBYTES  (40*1024)
#define GUI_BLOCKSIZE 0X80  //块大小


//GUI_X_Config
//初始化的时候调用,用来设置emwin所使用的内存
void GUI_X_Config(void) {
	static U32 aMemory[GUI_NUMBYTES / 4];
	
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
}
