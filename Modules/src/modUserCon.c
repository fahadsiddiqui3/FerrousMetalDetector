/**
  ******************************************************************************
  * @file    modUserCon.c
  * @author  Fahad Siddiqui
  * @brief   Contains Push button checks,Led Handling and Buzzer handling api
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "modUserCon.h"

/** @addtogroup Modules
  * @{
  */

/** @defgroup modUserCon
  * @brief 		Push button checks,Led Handling and Buzzer handling api
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
  * @brief  Initialize the User Control variables
  * @param  none
  * @retval none
  */
void modUserCon_Init(void)
{
	modUserCon_AlertLedOff();
	modUserCon_CalibrationLedOff();
	modUserCon_CalibrationFailLedOff();
	modUserCon_BuzzerOff();
}
/**
  * @brief  Turns On the Alert Led when a Metal is detected
  * @param  none
  * @retval none
  */
void modUserCon_AlertLedOn(Detector_State_te (*pChannel)[TOTAL_CHANNELS] )
{
			for(int Channel = CHANNEL_1 ; Channel <= CHANNEL_4 ; Channel++)
			{
				for(int Intensity = INTENSITY_LEVEL_1; Intensity <= INTENSITY_LEVEL_4 ; Intensity ++)
				{
					if(pChannel[Intensity][Channel] == DETECTED)
					{
						switch(Channel)
						{
							case CHANNEL_1 :HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
															break;
							case CHANNEL_2 :HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
															break;
							case CHANNEL_3 :HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
															break;
							case CHANNEL_4 :HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_SET);
															break;
							default : break;
						}
						break;
					}
					else
					{
						switch(Channel)
						{
							case CHANNEL_1 :HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
								break;
							case CHANNEL_2 :HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
							break;
							case CHANNEL_3 :HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
							break;
							case CHANNEL_4 :HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_RESET);
							break;
							default : break;
						}
					}
				}
			}
			
}
/**
  * @brief  Turns Off the Alert Led when a Metal is detected
  * @param  none
  * @retval none
  */
void modUserCon_AlertLedOff(void)
{
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);								
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);						
		HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_RESET);
}
/**
  * @brief  Turns On the Calibration Led when a Metal is detected
  * @param  none
  * @retval none
  */
void modUserCon_CalibrationLedOn(void)
{
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);

}
/**
  * @brief  Turns Off the Monitor Mode LED 
  * @param  none
  * @retval none
  */
void modUserCon_MonitorModeLedOff(void)
{
	HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_RESET);
}
/**
  * @brief  Turns On Monitor Mode LED 
  * @param  none
  * @retval none
  */
void modUserCon_MonitorModeLedOn(void)
{
//	HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_SET);
}

/**
  * @brief  Turns Off the Calibration Led when a Metal is detected
  * @param  none
  * @retval none
  */
void modUserCon_CalibrationLedOff(void)
{
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
}
/**
  * @brief  Turns On the Calibration Fail Led. 
  * @param  none
  * @retval none
  */
void modUserCon_CalibrationFailLedOn(void)
{
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
}
/**
  * @brief  Turns Off the Calibration Fail Led. 
  * @param  none
  * @retval none
  */
void modUserCon_CalibrationFailLedOff(void)
{

	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
}
/**
  * @brief  Blinks the Calibration Pass Led. 
  * @param  Count
  * @retval none
  */
void modUserCon_CalibrationPassLedBlink(uint8_t Count)
{
	for(int i = 0; i< Count*2 ; i++)
	{
		HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);
		HAL_Delay(100);
	}
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
}
/**
  * @brief  Reads the trigger Pin for Calibration.
  * @param  none
  * @retval True if Pin pressed else false
  */
bool modUserCon_GetCalibrationButtonState(void)
{
	if(HAL_GPIO_ReadPin(CTR_GPIO_Port,CTR_Pin) == GPIO_PIN_RESET)
	{
		while(HAL_GPIO_ReadPin(CTR_GPIO_Port,CTR_Pin) == GPIO_PIN_RESET);
		return true;
	}
	
	return false;
}
/**
  * @brief  Turns On the Buzzer
  * @param  none
  * @retval none
  */
void modUserCon_BuzzerOn(void)
{


}
/**
  * @brief  Turns Off the Buzzer
  * @param  none
  * @retval none
  */
void modUserCon_BuzzerOff(void)
{


}
/**
  * @brief  Blinks the Calibration Mode Led. 
  * @param  Count
  * @retval none
  */
void modUserCon_CalibrationModeLedBlink(uint8_t Count)
{
	for(int i = 0; i< Count*2 ; i++)
	{
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		HAL_Delay(100);
	}
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
	
}
bool modUserCon_GetCalibrationModeButtonState(void)
{
	if(HAL_GPIO_ReadPin(UP_GPIO_Port,UP_Pin) == GPIO_PIN_RESET)
	{
		while(HAL_GPIO_ReadPin(UP_GPIO_Port,UP_Pin) == GPIO_PIN_RESET);
		return true;
	}
	return false;
}
/**
  * @}
  */ 

/**
  * @}
  */ 
/******************************** EOF *****************************************/
