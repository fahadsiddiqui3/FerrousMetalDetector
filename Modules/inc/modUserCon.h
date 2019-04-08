// Header:
// File Name: modUserCon.h
// Author:		Fahad Siddiqui
// Date: 			24-Oct-18
#include "typedef.h"
#include "stdbool.h"
#include "stm32f7xx.h"

#ifndef __MOD_USERCON_H__
	#define __MOD_USERCON_H__
	
	void modUserCon_Init(void);
	bool modUserCon_GetCalibrationButtonState(void);
	void modUserCon_AlertLedOn(Detector_State_te (*pChannel)[TOTAL_CHANNELS] );
	void modUserCon_AlertLedOff(void);
	void modUserCon_CalibrationLedOff(void);
	void modUserCon_CalibrationFailLedOff(void);
	void modUserCon_MonitorModeLedOff(void);
	void modUserCon_MonitorModeLedOn(void);
	void modUserCon_BuzzerOff(void);
	void modUserCon_CalibrationLedOn(void);
	void modUserCon_CalibrationFailLedOn(void);
	void modUserCon_Buzzern(void);
	void modUserCon_CalibrationPassLedBlink(uint8_t Count);
	bool modUserCon_GetCalibrationModeButtonState(void);
    void modUserCon_CalibrationModeLedBlink(uint8_t Count);
#endif
