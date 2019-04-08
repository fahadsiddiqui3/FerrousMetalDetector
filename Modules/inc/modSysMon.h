// Header:
// File Name: modSysMon.h
// Author:		Fahad Siddiqui
// Date: 			24-Oct-18

#include "typedef.h"
#include "main.h"
#include "stm32f7xx.h"

#ifndef __MOD_SYSMON_H__
	#define __MOD_SYSMON_H__

	void modSysMon_Init(DataHandle_t  *phandleData ,Detector_Mode_te *pMode);
	void modSysMon_SystemMonitorTask(void);

#endif
