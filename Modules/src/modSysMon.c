/**
  ******************************************************************************
  * @file    modSysMon.c
  * @author  Fahad Siddiqui
  * @brief   Contains Sample quality evaluation api, Different intensity 
	*					 monitoring api and publishing of data codes.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "modSysMon.h"
#include "string.h"
#include "stdbool.h"
#include "libMetalDetector.h"
#include "adc.h"
#include "cmsis_os.h"
#include "modUserCon.h"
/** @addtogroup Modules
  * @{
  */

/** @defgroup modSysMon
  * @brief 		Contains Sample quality evaluation api, Different intensity 
	*						monitoring api and publishing of data api.
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define NORMAL_MODE_SAMPLE_BUFFER_SIZE	300
#define DETECTED_LIT_TIME								2000
/* Private variables ---------------------------------------------------------*/
static DataHandle_t  				*pDetectordata;
static Detector_Mode_te				*Mode;
static uint16_t 				 Normal_Mode_Sample_Buffer[TOTAL_CHANNELS][NORMAL_MODE_SAMPLE_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void              modSysMon_ResetUpdownCounter   (void);
static void              modSysMon_UpdateStateToUser    (void);
static void              modSysMon_UpdateOverallState   (void);
static void              modSysMon_DetectForIntensity   (Detector_Intensity_te Intensity);
static void              modSysMon_GetNormalModeSamples (uint16_t SampleIndex);
static void              modSysMon_MovingAverage        (uint16_t SampleIndex);
static void              modSysMon_MedianFilter         (uint16_t SampleIndex);
static Detector_State_te modSysMon_CheckForStateChange  (void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the System Monitor variables
  * @param  none
  * @retval none
  */
void modSysMon_Init(DataHandle_t *phandleData, Detector_Mode_te *pMode)
{
	Mode        		    = pMode;
	pDetectordata 		    = phandleData;
	*Mode 			    = MONITOR_MODE;
	pDetectordata->OverallState = NOT_DETECTED;
	modSysMon_ResetUpdownCounter();
	modUserCon_MonitorModeLedOn();
}

/**
  * @brief  Performs complete System Monitor task
  * @param  none
  * @retval none
  */
void modSysMon_SystemMonitorTask(void)
{
	static uint16_t SampleIndex = 0;

	modSysMon_GetNormalModeSamples(SampleIndex);
	
	modSysMon_MedianFilter(SampleIndex);
	modSysMon_MovingAverage(SampleIndex);
	
	modSysMon_DetectForIntensity(INTENSITY_LEVEL_1);
	modSysMon_DetectForIntensity(INTENSITY_LEVEL_2);
	modSysMon_DetectForIntensity(INTENSITY_LEVEL_3);
	modSysMon_DetectForIntensity(INTENSITY_LEVEL_4);
	
	modSysMon_UpdateOverallState();
	modSysMon_UpdateStateToUser();
	
	SampleIndex++;
	SampleIndex = SampleIndex < NORMAL_MODE_SAMPLE_BUFFER_SIZE ? SampleIndex : 0;
}
/**
  * @brief  Performs moving average for the normal mode buffer sample of size 10
  * @param  SampleIndex Current Index at which the sample has been taken
  * @retval none
  */
static void modSysMon_MovingAverage(uint16_t SampleIndex)
{
	/* Perform the moving average of size 10 */
	libMetalDetector_Moving_Average(Normal_Mode_Sample_Buffer, pDetectordata->MovingAvgSample,SampleIndex, NORMAL_MODE_SAMPLE_BUFFER_SIZE);
}

/**
  * @brief  Performs median filter for the calibration buffer sample of size 3
  * @param  SampleIndex Current Index at which the sample has been taken
  * @retval none
  */
static void modSysMon_MedianFilter(uint16_t SampleIndex)
{
	libMetalDetector_Median_Filter(Normal_Mode_Sample_Buffer, SampleIndex, NORMAL_MODE_SAMPLE_BUFFER_SIZE);
}
/**
  * @brief  Gets all the Sensors Sample and stores in the Normal buffer.
  * @param  none
  * @retval none
  */
static void modSysMon_GetNormalModeSamples(uint16_t SampleIndex)
{
	Get_ADC_Data(Normal_Mode_Sample_Buffer,SampleIndex);	
}

/**
  * @brief  Scans for 4 different intensities for all the channels
  * @param  Intensity for detection, check Detector_Intensity_te in typedef for possible arguments 
  * @retval none
  */
static void modSysMon_DetectForIntensity(Detector_Intensity_te Intensity)
{
	pDetectordata->Intensity = Intensity;	
	modSysMon_CheckForStateChange();
}

/**
  * @brief  Check for state change
  * @param  none
  * @retval return DETECTED if detected else return NOTDETECTED 
  */
static Detector_State_te modSysMon_CheckForStateChange(void)
{
	return libMetalDetector_GetDetectorState(pDetectordata , pDetectordata->Intensity);
}
/**
  * @brief  Updates state for all the channels and overall state
  * @param  none
  * @retval none
  */
static void modSysMon_UpdateOverallState(void)
{	
	Detector_State_te StateToUpdate = NOT_DETECTED;
	
	for(int Intensity = INTENSITY_LEVEL_1; Intensity <= INTENSITY_LEVEL_4 ; Intensity ++)
	{
			for( int Channel = CHANNEL_1; Channel <= CHANNEL_4; Channel++ )
			{
				if(pDetectordata->ChannelState[Intensity][Channel] == DETECTED)
				{
					StateToUpdate = DETECTED;
				}
			}
	}
	
	pDetectordata->OverallState = StateToUpdate;
}
/**
  * @brief  Updates detector state to the user
  * @param  StateToUpdate
  * @retval none
  */
static void modSysMon_UpdateStateToUser(void)
{
	modUserCon_AlertLedOn(pDetectordata->ChannelState);
}
/**
  * @brief 	Resets the UpdownCounter to initial limit
  * @param  StateToUpdate
  * @retval none
  */
static void modSysMon_ResetUpdownCounter(void)
{
	memset(pDetectordata->UpDownCounter, 0, sizeof(pDetectordata->UpDownCounter) );

}

/**
  * @}
  */ 

/**
  * @}
  */ 
/******************************** EOF *****************************************/
