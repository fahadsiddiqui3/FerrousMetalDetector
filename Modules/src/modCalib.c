/**
  ******************************************************************************
  * @file    modCalib.c
  * @author  Fahad Siddiqui
  * @brief   This file provides calibration trigger, sample buffer specific to 
  *	     calibration and calibration data calculation codes.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "modCalib.h"
#include "modUserCon.h"
#include "cmsis_os.h"
#include "libMetalDetector.h"
/** @addtogroup Modules
  * @{
  */

/** @defgroup modCalib
  * @brief 	Provides calibration trigger, sample buffer specific to 
  *		calibration and calibration data calculation codes.
  * @{
  */

/* External variables --------------------------------------------------------*/
extern void Init_System(void);
/* Private typedef Init_System-----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define CALIBRATION_BUFFER_SIZE						300
#define MEAN_MAX_LIMIT							3000	
#define MEAN_MIN_LIMIT							1000
#define MIN_FLICKER_COUNT_LIM						25
#define MAX_FLICKER_COUNT_LIM						75
#define RANGE_LIMIT							620
#define MAX_TRAILS							3
#define SUCCESS								255
#define AUTOCALIBRATION_TIMEOUT						18000  /*3Min*/
#define AUTOCALIBRATION_FLICKER_TIMEOUT 	6000   /*1Min*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t 				Calibration_Sample_Buffer[TOTAL_CHANNELS][CALIBRATION_BUFFER_SIZE];
static CalibData_t  				CalibData;
static Detector_Mode_te	 			*Mode;
static Detector_State_te 			*OverallState;
static int16_t				 	(*pUpDownCounter)[TOTAL_CHANNELS];			
/* Private function prototypes -----------------------------------------------*/
static void modCalib_MedianFilter(uint32_t SampleCount);
static void modCalib_GetCalibrationSamples(void);
static bool modCalib_GetTrigger(void);
static void modCalib_StoreCalibrationData(void);
static void modCalib_CalculateCalibrationData(void);
static void modCalib_Get_Mean_Min_Max(void);
static bool modCalib_CheckDataValidity(void);
static void modCalib_HandleCalibrationLedTask(uint16_t Trial);
static bool	modCalib_GetAutoCalibrationTrigger(void);
static void modCalib_CalibrationModeSelectTask(void);
static bool modCalib_Check_DetectedTickTimeout(void);
static bool modCalib_Check_FlickerTickTimeout(void);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Checks for trigger to start calibration.
  * @param  none
  * @retval True if trigger detected else false
  */
static bool modCalib_GetManualCalibrationTrigger(void)
{
		return modUserCon_GetCalibrationButtonState();

}
/**
  * @brief  Initialize the calibration variables,
  * @param  none
  * @retval none
  */
void modCalib_Init(DataHandle_t *phandleData)
{
	memset(&CalibData , 0 ,sizeof(CalibData));
	memset(&Calibration_Sample_Buffer , 0 ,sizeof(Calibration_Sample_Buffer));
	
	Mode         			       = &phandleData->Mode;
	OverallState 			       = &phandleData->OverallState;
	phandleData->CalibData 		       = &CalibData;
	pUpDownCounter         		       = phandleData->UpDownCounter;
	CalibData.AutoCalibrationTick_Detected = AUTOCALIBRATION_TIMEOUT;
	CalibData.CalibrationMode 	       = AUTO_CALIBRATION;
}

/**
  * @brief  Performs complete Calibration task
  * @param  none
  * @retval none
  */
void modCalib_CalibrationTask(void)
{
	uint16_t Trial = 0;

	modCalib_CalibrationModeSelectTask();
	
	if(modCalib_GetManualCalibrationTrigger() || modCalib_GetAutoCalibrationTrigger())
	{
		modUserCon_Init();
		*Mode = CALIBRATION_MODE;
		modUserCon_CalibrationLedOn();

		do{				

				modCalib_GetCalibrationSamples();
				modCalib_CalculateCalibrationData();

				if(modCalib_CheckDataValidity() == true)
				{
					Trial = SUCCESS;
					modCalib_StoreCalibrationData();
					//Initialize the parameters again
					Init_System();
				}
				else
				{
					Trial++;
				}
				
		}while(Trial < MAX_TRAILS);
	
		modCalib_HandleCalibrationLedTask(Trial); 
	}		

}
/**
  * @brief  Checks for trigger for Calibration modes and blink led accordingly
  * @param  Trials
  * @retval none
  */
static void modCalib_CalibrationModeSelectTask(void)
{
	if(modUserCon_GetCalibrationModeButtonState() == true)
	{
		CalibData.CalibrationMode =(Calibration_Mode_te) !CalibData.CalibrationMode;
		
		if(CalibData.CalibrationMode == AUTO_CALIBRATION)
		{
			modUserCon_CalibrationModeLedBlink(3);
		}
		else
		{
			modUserCon_CalibrationModeLedBlink(1);
		}
	}
}
/**
  * @brief  Handles Calibration Trigger when AUto Calibration is turned on
  * @param  Trials
  * @retval none
  */
static bool modCalib_GetAutoCalibrationTrigger(void)
{
	bool ret = false;
	
	if (CalibData.CalibrationMode == AUTO_CALIBRATION)
	{
		ret  = modCalib_Check_FlickerTickTimeout();
		ret |= modCalib_Check_DetectedTickTimeout();
	}

	return ret;
	
}
/**
  * @brief  Updates the Detected ticks for Each channel and intensity
  * @param  Trials
  * @retval none
  */
static bool modCalib_Check_DetectedTickTimeout()
{
	static uint32_t PreviousTick = 0;
	bool ret = false;
	
	if (CalibData.CalibrationMode == AUTO_CALIBRATION)
	{
		if(CalibData.AutoCalibrationTick_Detected >= AUTOCALIBRATION_TIMEOUT)
		{
			CalibData.AutoCalibrationTick_Detected = 0;
			PreviousTick = osKernelSysTick();	
			ret = true;	
		}
		
		if(*OverallState == DETECTED)
		{

				CalibData.AutoCalibrationTick_Detected += (osKernelSysTick() - PreviousTick );
				PreviousTick = osKernelSysTick();
			
		}
		else
		{
			CalibData.AutoCalibrationTick_Detected = 0;
			PreviousTick = osKernelSysTick();
		}	
	}
	
	return ret;
	
}
/**
  * @brief  Updates the flicker ticks for Each channel and intensity
  * @param  Trials
  * @retval none
  */
static bool modCalib_Check_FlickerTickTimeout(void)
{
	static uint32_t PreviousTick[TOTAL_INTENSITY_LEVEL][TOTAL_CHANNELS] = {0};
	bool ret = false;
		
	for(int Intensity = INTENSITY_LEVEL_1; Intensity <= INTENSITY_LEVEL_4 && !ret ; Intensity ++)
	{
		for(int Channel = CHANNEL_1; Channel <= CHANNEL_4 && !ret ; Channel ++)
		{

			if( pUpDownCounter[Intensity][Channel] >= MIN_FLICKER_COUNT_LIM && 
				pUpDownCounter[Intensity][Channel] <= MAX_FLICKER_COUNT_LIM )
			{

				CalibData.AutoCalibrationTick_Flicker[Intensity][Channel] += ( osKernelSysTick() - PreviousTick[Intensity][Channel]) ;
				PreviousTick[Intensity][Channel] = osKernelSysTick();

			}
			else
			{
				CalibData.AutoCalibrationTick_Flicker[Intensity][Channel] = 0;		
				PreviousTick[Intensity][Channel] = osKernelSysTick();					
			}	

			if(CalibData.AutoCalibrationTick_Flicker[Intensity][Channel] >= AUTOCALIBRATION_FLICKER_TIMEOUT)
			{
				memset(CalibData.AutoCalibrationTick_Flicker, 0 , sizeof(CalibData.AutoCalibrationTick_Flicker));
				PreviousTick[Intensity][Channel] = osKernelSysTick();				
				ret = true;	
			}

		}
	}
	
	return ret;
	
}
/**
  * @brief  Handles Calibration Led Tasks
  * @param  Trials
  * @retval none
  */
static void modCalib_HandleCalibrationLedTask(	uint16_t Trial)
{

	 if(Trial == SUCCESS)
	 {
		 modUserCon_CalibrationPassLedBlink(5);
	 }
	 else
	 {
		modUserCon_CalibrationFailLedOn();
		osDelay(3000);
	 }
	 
	 modUserCon_CalibrationFailLedOff();
	 modUserCon_CalibrationLedOff();

}
/**
  * @brief  Performs median filter for the calibration buffer sample of size 3
  * @param  none
  * @retval none
  */
static void modCalib_MedianFilter(uint32_t SampleCount)
{
	libMetalDetector_Median_Filter( Calibration_Sample_Buffer ,SampleCount ,CALIBRATION_BUFFER_SIZE);
}

/**
  * @brief  Gets all the Sensors Sample and stores in the caibration buffer, 
	*					Each sample delay is 10 ms.
  * @param  none
  * @retval none
  */
static void modCalib_GetCalibrationSamples(void)
{
	uint32_t SampleCount;
	uint32_t PreviousWakeTime = osKernelSysTick();	
	
	for(SampleCount = 0; SampleCount < CALIBRATION_BUFFER_SIZE ; SampleCount++ )
	{
		Get_ADC_Data(Calibration_Sample_Buffer,SampleCount);	
		modCalib_MedianFilter(SampleCount);
		osDelay(10);
	}

}
/**
  * @brief  Stores the Calibration data into non volatile memory
  * @param  CalibData_t *
  * @retval none
  */
static void modCalib_StoreCalibrationData(void)
{


}
/**
  * @brief  Calculates the Mean, Min and Max values of samples and puts into calibration data buff.
  * @param  CalibData_t *
  * @retval none
  */
static void modCalib_Get_Mean_Min_Max(void)
{
	memset(&CalibData.Max_Calib  , 0      , sizeof(CalibData.Max_Calib));
	memset(&CalibData.Mean_Calib , 0      , sizeof(CalibData.Mean_Calib));
	memset(&CalibData.Min_Calib  , 0xffff , sizeof(CalibData.Min_Calib));

	for(int Channel = CHANNEL_1; Channel<= CHANNEL_4; Channel++)
	{
		
		for( int Index = 3; Index < CALIBRATION_BUFFER_SIZE - 1; Index ++ )
		{

			if( Calibration_Sample_Buffer[Channel][Index] < CalibData.Min_Calib[Channel] )
			{
				CalibData.Min_Calib[Channel] = Calibration_Sample_Buffer[Channel][Index];
			}

			if( Calibration_Sample_Buffer[Channel][Index] > CalibData.Max_Calib[Channel] )
			{
				CalibData.Max_Calib[Channel] = Calibration_Sample_Buffer[Channel][Index];
			}

			CalibData.Mean_Calib[Channel] += Calibration_Sample_Buffer[Channel][Index];

		}
	
		CalibData.Mean_Calib[Channel] /= CALIBRATION_BUFFER_SIZE - 4;
	
	}
}
/**
  * @brief  Calculates the Calibration data.
  * @param  CalibData_t *
  * @retval none
  */
static void modCalib_CalculateCalibrationData(void)
{
		modCalib_Get_Mean_Min_Max();	
}
/**
  * @brief  Stores the Calibration data into non volatile memory
  * @param  CalibData_t *
  * @retval none
  */
static bool modCalib_CheckDataValidity(void)
{
	bool State = true;
	
	for(int Channel = CHANNEL_1; Channel <= CHANNEL_4 && State ; Channel++)
	{
		if( CalibData.Mean_Calib[Channel] > MEAN_MAX_LIMIT)
			State = false;
	
		if( CalibData.Mean_Calib[Channel] < MEAN_MIN_LIMIT)
			State = false;
		
		if( ( abs( CalibData.Max_Calib[Channel] - CalibData.Min_Calib[Channel]) ) > RANGE_LIMIT )
			State = false;
	}
	
	return State;
}


/**
  * @}
  */ 

/**
  * @}
  */ 
/******************************** EOF *****************************************/
