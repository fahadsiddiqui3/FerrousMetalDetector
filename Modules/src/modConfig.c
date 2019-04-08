/**
  ******************************************************************************
  * @file     modConfig.c
  * @author  	Fahad Siddiqui
  * @brief   	This file contains general configuration codes.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "modConfig.h"
#include "stdlib.h"
/** @addtogroup Modules
  * @{
  */

/** @defgroup modConfig
  * @brief  	This file contains general configuration codes.
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize the Mean Variation and hysterisis threshold variables
  * @param  none
  * @retval none
  */
void modConfig_Init(DataHandle_t *phandleData)
{
	 int16_t Difference = 0;
	
	for(int Channel = CHANNEL_1 ; Channel <= CHANNEL_4 ; Channel++)
	{
		Difference = phandleData->CalibData->Max_Calib[Channel] - phandleData->CalibData->Min_Calib[Channel];
		Difference = abs(Difference);
		phandleData->Mean_Variation[Channel] =  Difference / 2;
	}
	phandleData->Undetected_Threshold = 12;
}

/**
  * @}
  */ 

/**
  * @}
  */ 
/******************************** EOF *****************************************/
