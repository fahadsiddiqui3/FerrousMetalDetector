#include "main.h"
#include "stm32f7xx_hal.h"

#ifndef __FILEOP_H__
	#define __FILEOP_H__

	void FILEOP_Get_Filename(char *FileName,uint16_t Length, uint16_t Counter);	
	int Get_Header_Check_Status(void);
	void Set_Header_Check_Status(uint8_t Status);
#endif
