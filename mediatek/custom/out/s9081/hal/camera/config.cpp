#include <stdlib.h>
#include <stdio.h>
#include <cutils/properties.h>
#include "camera_custom_if.h"

namespace NSCamCustom
{
/*******************************************************************************
* 
*******************************************************************************/
#include "cfg_tuning.h"
#include "cfg_facebeauty_tuning.h"
#include "flicker_tuning.h"
//
#include "cfg_setting_imgsensor.h"
#include "cfg_tuning_imgsensor.h"

/*******************************************************************************
* custom exif
*******************************************************************************/
#define EN_CUSTOM_EXIF_INFO
MINT32 custom_SetExif(void **ppCustomExifTag)
{
#ifdef EN_CUSTOM_EXIF_INFO
#define CUSTOM_EXIF_STRING_MAKE  "TINNO"
#define CUSTOM_EXIF_STRING_MODEL "SmartPhone"
#define CUSTOM_EXIF_STRING_SOFTWARE "Camera Application"
static customExifInfo_t exifTag = {CUSTOM_EXIF_STRING_MAKE,CUSTOM_EXIF_STRING_MODEL,CUSTOM_EXIF_STRING_SOFTWARE};
    if (0 != ppCustomExifTag) {
    	char valueMAKE[32] = {'\0'};
    	char valueMODEL[32] = {'\0'};
    	property_get("ro.product.brand", valueMAKE, CUSTOM_EXIF_STRING_MAKE);
    	property_get("ro.product.model", valueMODEL, CUSTOM_EXIF_STRING_MODEL);
    	memcpy(exifTag.strMake,valueMAKE,sizeof(valueMAKE));
    	memcpy(exifTag.strModel,valueMODEL,sizeof(valueMODEL));
        *ppCustomExifTag = (void*)&exifTag;
    }
    return 0;
#else
    return -1;
#endif
}

/*******************************************************************************
* LCM physical orienation 
*   Return:
*       0   : no inverse
*       1   : inverse
*       -1  : error
*******************************************************************************/
MUINT32
getLCMPhysicalOrientation()
{
    return ::atoi(MTK_LCM_PHYSICAL_ROTATION); 
}

/*******************************************************************************
* Author : cotta
* Functionality : custom flashlight gain between preview/capture flash
*******************************************************************************/
#define FLASHLIGHT_CALI_LED_GAIN_PRV_TO_CAP_10X 10
MUINT32 custom_GetFlashlightGain10X(void)
{
    // x10 , 1 mean 0.1x gain
    //10 means no difference. use torch mode for preflash and cpaflash
    //> 10 means capture flashlight is lighter than preflash light. < 10 is opposite condition.

    return (MUINT32)FLASHLIGHT_CALI_LED_GAIN_PRV_TO_CAP_10X;
}

MUINT32 custom_BurstFlashlightGain10X(void)
{
	return FLASHLIGHT_CALI_LED_GAIN_PRV_TO_CAP_10X;
}

/*******************************************************************************
* 
*******************************************************************************/
};  //NSCamCustom

