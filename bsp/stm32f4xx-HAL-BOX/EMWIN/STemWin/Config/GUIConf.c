#include "GUI.h"

//����EMWIN�ڴ��С
#define GUI_NUMBYTES  (40*1024)
#define GUI_BLOCKSIZE 0X80  //���С


//GUI_X_Config
//��ʼ����ʱ�����,��������emwin��ʹ�õ��ڴ�
void GUI_X_Config(void) {
	static U32 aMemory[GUI_NUMBYTES / 4];
	
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
}
