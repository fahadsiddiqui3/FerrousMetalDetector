// Header:
// File Name: libMetaDetector.h
// Author:		Fahad Siddiqui
// Date: 			24-Oct-18

#include "main.h"
#include "stm32f7xx_hal.h"
#include "typedef.h"
#ifndef __LIB_METALDETECTOR_H__
	#define __LIB_METALDETECTOR_H__
	
	#define UPDOWNCOUNTER_NOTDETECT_LIMIT		1
	#define UPDOWNCOUNTER_DETECT_LIMIT		  100
	void 							libMetalDetector_Median_Filter(void *pBuf ,int Index ,int Bufsize);
	void 							libMetalDetector_Moving_Average(void *pBuf ,uint16_t *pDestBuf ,int Index ,int Bufsize);
	Detector_State_te libMetalDetector_GetDetectorState(DataHandle_t* pDetectorData ,Detector_Intensity_te IntensityRange);
	
#endif
