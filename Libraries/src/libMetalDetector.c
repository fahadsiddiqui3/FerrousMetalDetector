/**
  ******************************************************************************
  * @file    libMetalDetector.c
  * @author  Fahad Siddiqui
  * @brief   Contains median filter, moving average and state change logic codes.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "libMetalDetector.h"
#include "stdlib.h"
#include "stdbool.h"
/** @addtogroup Libraries
  * @{
  */

/** @defgroup libMetalDetector
  * @brief 	  Contains median filter, moving average and state change logic codes.
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define INTENSITY_1_COUNT								12
#define INTENSITY_2_COUNT								31
#define INTENSITY_3_COUNT								62
#define INTENSITY_4_COUNT								124
#define MOVING_AVERAGE_SIZE							10
#define MEDIAN_FILTER_SIZE							3
/* Private variables ---------------------------------------------------------*/
static const int32_t  Intensity[] = { 
																			 INTENSITY_1_COUNT, 	/*10mv Count  Smallest Object*/
																			 INTENSITY_2_COUNT,	  /*25mv	Count*/
																			 INTENSITY_3_COUNT,	  /*50mv	Count*/
																			 INTENSITY_4_COUNT  	/*100mv	Count  Biggest Object*/
																	};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Using bubble sort and taking the median i.e. middle value of 
	*					the scan buff and returning it to the calling function 
  * @param  pointer to array Scan_Buff
  * @retval sorted median sample
  */
static uint16_t libMetalDetector_Sort_Median_Values(uint16_t Scan_Buff[])
{	 
		uint32_t m,n,temp;
	
		for(m=0;m<3;m++)
		{
			for(n=m+1;n<3;n++)
			{
				if(Scan_Buff[m]>Scan_Buff[n])
				{
					temp = Scan_Buff[m];
					Scan_Buff[m] = Scan_Buff[n];
					Scan_Buff[n] = temp;
				}
			}
		}
		
		return Scan_Buff[1];
}

/**
  * @brief  			Median Filter is used to avoid glitches and noise. Here our filter depends upon previous 2samples.
	*								Current sample and previous two samples are moved to filter buffers and then they are sorted and
	*		  					median of those sorted values is the valid sample at the current buffer index.
  * @param 				pBuf pointer to array of Integers
  * @param 				Index Index at which the filtered sample should be placed
  * @param 				Bufsize Size of the buffer for resolving 2d pointer
  * @retval 			none
  */
void libMetalDetector_Median_Filter(void *pBuf ,int Index ,int Bufsize)
{

	uint32_t SubIndex;
	uint16_t Filter_Buffer[MEDIAN_FILTER_SIZE];
	uint32_t FilterIndex;
	uint16_t (*pADCBuffer)[Bufsize];
	
	if(Index < MEDIAN_FILTER_SIZE)
	{		
		return;
	}
	
	pADCBuffer = pBuf;
	
	for(SubIndex = CHANNEL_1; SubIndex <= CHANNEL_4; SubIndex++)
	{
		
			for(FilterIndex = 0; FilterIndex < MEDIAN_FILTER_SIZE ;FilterIndex++)
			{
				Filter_Buffer[FilterIndex]	=	pADCBuffer[SubIndex][ Index - FilterIndex ];
				
			}

			pADCBuffer[SubIndex][Index - 1] = libMetalDetector_Sort_Median_Values(Filter_Buffer);;
	}

}
/**
  * @brief  		Moving average is used to take the average of current and previous values of the buffer.
	*							This implementation is of order 10
  * @param 			pBuf pointer to array of Integers
  * @param 			Index Index at which the filtered sample should be placed
  * @param 			Bufsize Size of the buffer for resolving 2d pointer
  * @retval 		none
  */
void libMetalDetector_Moving_Average(void *pBuf ,uint16_t *pDestBuf ,int Index ,int Bufsize)
{
	uint32_t SubIndex;
	uint32_t MovingAvgIndex;
	uint32_t SumOfSamples;
	uint16_t (*pADCBuffer)[Bufsize];
	
	if(Index < MOVING_AVERAGE_SIZE - 1)
	{		
		return;
	}
	
	pADCBuffer = pBuf;
	
	for(SubIndex = CHANNEL_1; SubIndex <= CHANNEL_4; SubIndex++)
	{
			SumOfSamples = 0;
		
			for(MovingAvgIndex = Index - (MOVING_AVERAGE_SIZE - 1) ; MovingAvgIndex <= Index ; MovingAvgIndex++)		
			{
				
				 SumOfSamples += pADCBuffer[SubIndex][MovingAvgIndex];
			}
		
			pDestBuf[SubIndex] = SumOfSamples / MOVING_AVERAGE_SIZE;
	
	}


}
/**
  * @brief  Updates the UpdownCounter for debounce
  * @param  DebounceDirection , Used to determine whether increment or decrement the updown counter
	*					pDetectordata, Data Handle pointer
	*					Channel at which the updown counter has to be updated
  * @retval DEBOUNCE_OK if the updown counter reaches its threshold
	*					else DEBOUNCE_NOTOK;
  */
static Debounce_State_te libMetalDetector_UpdateUpdownCounter(DataHandle_t* pDetectordata,Detector_State_te DebounceDirection , uint8_t Channel)
{
	
	if (DebounceDirection == DETECTED)
	{
		pDetectordata->UpDownCounter[pDetectordata->Intensity][Channel]++;
		
		if(pDetectordata->UpDownCounter[pDetectordata->Intensity][Channel] > UPDOWNCOUNTER_DETECT_LIMIT)
		{
			pDetectordata->UpDownCounter[pDetectordata->Intensity][Channel] = UPDOWNCOUNTER_DETECT_LIMIT;
			
			return DEBOUNCE_OK;
		}
	}
	
	if (DebounceDirection == NOT_DETECTED)
	{
		pDetectordata->UpDownCounter[pDetectordata->Intensity][Channel]--;
		
		if(pDetectordata->UpDownCounter[pDetectordata->Intensity][Channel] <= UPDOWNCOUNTER_NOTDETECT_LIMIT)
		{
			pDetectordata->UpDownCounter[pDetectordata->Intensity][Channel] = UPDOWNCOUNTER_NOTDETECT_LIMIT;
			return DEBOUNCE_OK;
		}		
	}
	
	return DEBOUNCE_NOTOK;
}
/**
  * @brief  Checks if some metal is detcted or not for the given intensity range;
  * @param  none
  * @retval none
  */
Detector_State_te libMetalDetector_GetDetectorState(DataHandle_t* pDetectorData ,Detector_Intensity_te IntensityRange)
{
		int32_t  					iSampleThreshold = 0;
		int32_t 					ThresholdTocheck;
		Detector_State_te State = NOT_DETECTED;
	
		for( int Channel = CHANNEL_1 ; Channel <= CHANNEL_4; Channel++ )
		{
			
			iSampleThreshold = pDetectorData->MovingAvgSample[Channel] - pDetectorData->CalibData->Mean_Calib[Channel] ;
			iSampleThreshold = abs(iSampleThreshold);
			ThresholdTocheck =  Intensity[IntensityRange];
			
			if(iSampleThreshold < abs(pDetectorData->Mean_Variation[Channel] - pDetectorData->Undetected_Threshold ) )
			{
				if(libMetalDetector_UpdateUpdownCounter(pDetectorData, NOT_DETECTED,Channel) == DEBOUNCE_OK)
				{
  				pDetectorData->ChannelState[pDetectorData->Intensity][Channel] = NOT_DETECTED;
  				State = NOT_DETECTED;
				}
			}
			
		  else if(iSampleThreshold > abs(pDetectorData->Mean_Variation[Channel] + ThresholdTocheck) )
			{
				if(libMetalDetector_UpdateUpdownCounter(pDetectorData, DETECTED,Channel) == DEBOUNCE_OK)
				{
					pDetectorData->ChannelState[pDetectorData->Intensity][Channel] = DETECTED;		
					State = DETECTED;
				}
			}
			
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
