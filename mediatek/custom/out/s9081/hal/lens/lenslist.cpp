#include <utils/Log.h>
#include <utils/Errors.h>
#include <fcntl.h>
#include <math.h>

#include "MediaHal.h"

//#include "lens_custom_cfg.h"
//#include "msdk_lens_exp.h"
#include "camera_custom_lens.h"
//#include "lens.h"
//nclude "image_sensor.h"
#include "kd_imgsensor.h"

extern PFUNC_GETLENSDEFAULT pDummy_getDefaultData;

#if defined(SENSORDRIVE)
extern PFUNC_GETLENSDEFAULT pSensorDrive_getDefaultData;
#endif

#if defined(FM50AF)
extern PFUNC_GETLENSDEFAULT pFM50AF_getDefaultData;
#endif

#if defined(FM51AF)
extern PFUNC_GETLENSDEFAULT pFM51AF_getDefaultData;
#endif

#if defined(FM52AF)
extern PFUNC_GETLENSDEFAULT pFM52AF_getDefaultData;
#endif

#if defined(OV5648AF)
extern PFUNC_GETLENSDEFAULT pOV5648AF_getDefaultData;
#endif

#if defined(MT9P017AF)
extern PFUNC_GETLENSDEFAULT pMT9P017AF_getDefaultData;
#endif

#if defined(OV8825AF)
extern PFUNC_GETLENSDEFAULT pOV8825AF_getDefaultData;
#endif

#if defined(OV8826AF)
extern PFUNC_GETLENSDEFAULT pOV8826AF_getDefaultData;
#endif

#if defined(IMX105AF)
extern PFUNC_GETLENSDEFAULT pIMX105AF_getDefaultData;
#endif

#if defined(OV8827AF)
extern PFUNC_GETLENSDEFAULT pOV8827AF_getDefaultData;
#endif

MSDK_LENS_INIT_FUNCTION_STRUCT LensList[MAX_NUM_OF_SUPPORT_LENS] =
{
	{DUMMY_SENSOR_ID, DUMMY_LENS_ID, "Dummy", pDummy_getDefaultData},

#if defined(SENSORDRIVE)
	{DUMMY_SENSOR_ID, SENSOR_DRIVE_LENS_ID, "kd_camera_hw", pSensorDrive_getDefaultData},	

    //  for backup lens, need assign correct SensorID
    //{OV5642_SENSOR_ID, SENSOR_DRIVE_LENS_ID, "kd_camera_hw", pSensorDrive_getDefaultData},
#endif

#if defined(FM50AF)
	{OV5647_SENSOR_ID, FM50AF_LENS_ID, "FM50AF", pFM50AF_getDefaultData},
#endif

#if defined(FM51AF)
	{OV5646_SENSOR_ID, FM51AF_LENS_ID, "FM51AF", pFM51AF_getDefaultData},
#endif

#if defined(FM52AF)
	{OV5645_SENSOR_ID, FM52AF_LENS_ID, "FM52AF", pFM52AF_getDefaultData},
#endif

#if defined(OV5648AF)
	{OV5648_SENSOR_ID, OV5648AF_LENS_ID, "OV5648AF", pOV5648AF_getDefaultData},
#endif

#if defined(MT9P017AF)
	{MT9P017MIPI_SENSOR_ID, MT9P017AF_LENS_ID, "MT9P017AF", pMT9P017AF_getDefaultData},
#endif

#if defined(OV8825AF)
	{OV8825_SENSOR_ID, OV8825AF_LENS_ID, "OV8825AF", pOV8825AF_getDefaultData},
#endif

#if defined(OV8826AF)
	{OV8826_SENSOR_ID, OV8826AF_LENS_ID, "OV8826AF", pOV8826AF_getDefaultData},
#endif

    //  for new added lens, need assign correct SensorID
#if defined(IMX105AF)
	{IMX105_SENSOR_ID, IMX105AF_LENS_ID, "IMX105AF", pIMX105AF_getDefaultData},
#endif

#if defined(OV8827AF)
	{OV8827_SENSOR_ID, OV8827AF_LENS_ID, "OV8827AF", pOV8826AF_getDefaultData},
#endif
};

UINT32 GetLensInitFuncList(PMSDK_LENS_INIT_FUNCTION_STRUCT pLensList)
{
    memcpy(pLensList, &LensList[0], sizeof(MSDK_LENS_INIT_FUNCTION_STRUCT)* MAX_NUM_OF_SUPPORT_LENS);
    return MHAL_NO_ERROR;
} // GetLensInitFuncList()






