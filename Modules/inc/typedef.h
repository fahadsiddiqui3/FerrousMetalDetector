// Header:
// File Name: typedef.h
// Author:		Fahad Siddiqui
// Date: 			24-Oct-18

#include "stdint.h"

#ifndef __TYPEDEF_H__
	#define __TYPEDEF_H__

	#define TOTAL_CHANNELS					4
	#define TOTAL_INTENSITY_LEVEL		4

	/*************   ENUMS ******************/
	typedef enum {
			INTENSITY_LEVEL_1 = 0,
			INTENSITY_LEVEL_2,
			INTENSITY_LEVEL_3,
			INTENSITY_LEVEL_4,
	}Detector_Intensity_te; 

	typedef enum {
		NOT_DETECTED = 0,
		DETECTED,
	}Detector_State_te;
	
	typedef enum {
		DEBOUNCE_NOTOK = 0,
		DEBOUNCE_OK,
		DEBOUNCE_UNDEFINED,
	}Debounce_State_te;
	
	typedef enum {
			CHANNEL_1 = 0,
			CHANNEL_2,
			CHANNEL_3,
			CHANNEL_4,
	}Detector_Channels_te;
	
	typedef enum {
		CALIBRATION_MODE = 0,
		MONITOR_MODE,
	}Detector_Mode_te;
	
	typedef enum {
		AUTO_CALIBRATION = 0,
		MANUAL_CALIBRATION,
	}Calibration_Mode_te;
	
	/******************TYPEDEF STRUCTS*************************/
	
	typedef struct{
		uint16_t  					Min_Calib [TOTAL_CHANNELS];
		uint16_t						Max_Calib [TOTAL_CHANNELS];
		uint32_t						Mean_Calib[TOTAL_CHANNELS];
		uint32_t						AutoCalibrationTick_Flicker[TOTAL_INTENSITY_LEVEL][TOTAL_CHANNELS];
		uint32_t						AutoCalibrationTick_Detected;
		Calibration_Mode_te CalibrationMode;
	}CalibData_t;
	
	typedef struct {
		uint16_t IntensityRange_1;
		uint16_t IntensityRange_2;
		uint16_t IntensityRange_3;
		uint16_t IntensityRange_4;
	}IntensityData_t;
	
	typedef struct
	{
			Detector_Mode_te				Mode;
			Detector_State_te				OverallState;
			Detector_State_te				ChannelState[TOTAL_INTENSITY_LEVEL][TOTAL_CHANNELS];
			Detector_Intensity_te		Intensity;
			CalibData_t							*CalibData;
		  int16_t									Mean_Variation[TOTAL_CHANNELS];
			int16_t									Undetected_Threshold;
			uint16_t								MovingAvgSample[TOTAL_CHANNELS];
			int16_t								  UpDownCounter[TOTAL_INTENSITY_LEVEL][TOTAL_CHANNELS];					
	}DataHandle_t;
		
#endif
