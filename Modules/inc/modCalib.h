// Header:
// File Name: modCalib.h
// Author:		Fahad Siddiqui
// Date: 			24-Oct-18
#include "typedef.h"
#include "main.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#ifndef __MOD_CALIB_H__
	#define __MOD_CALIB_H__

	void modCalib_CalibrationTask(void);
	void modCalib_Init(DataHandle_t *phandleData);
#endif
