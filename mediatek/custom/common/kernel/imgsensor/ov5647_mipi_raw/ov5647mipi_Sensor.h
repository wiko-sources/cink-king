

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   CMOS sensor header file
 *
 ****************************************************************************/
#ifndef _OV5647MIPI_SENSOR_H
#define _OV5647MIPI_SENSOR_H

#define OV5647MIPI_DEBUG
#define OV5647MIPI_DRIVER_TRACE
//#define OV5647MIPI_TEST_PATTEM
#ifdef OV5647MIPI_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif

#define OV5647MIPI_FACTORY_START_ADDR 0
#define OV5647MIPI_ENGINEER_START_ADDR 10

#define MIPI_INTERFACE
 
typedef enum OV5647MIPI_group_enum
{
  OV5647MIPI_PRE_GAIN = 0,
  OV5647MIPI_CMMCLK_CURRENT,
  OV5647MIPI_FRAME_RATE_LIMITATION,
  OV5647MIPI_REGISTER_EDITOR,
  OV5647MIPI_GROUP_TOTAL_NUMS
} OV5647MIPI_FACTORY_GROUP_ENUM;

typedef enum OV5647MIPI_register_index
{
  OV5647MIPI_SENSOR_BASEGAIN = OV5647MIPI_FACTORY_START_ADDR,
  OV5647MIPI_PRE_GAIN_R_INDEX,
  OV5647MIPI_PRE_GAIN_Gr_INDEX,
  OV5647MIPI_PRE_GAIN_Gb_INDEX,
  OV5647MIPI_PRE_GAIN_B_INDEX,
  OV5647MIPI_FACTORY_END_ADDR
} OV5647MIPI_FACTORY_REGISTER_INDEX;

typedef enum OV5647MIPI_engineer_index
{
  OV5647MIPI_CMMCLK_CURRENT_INDEX = OV5647MIPI_ENGINEER_START_ADDR,
  OV5647MIPI_ENGINEER_END
} OV5647MIPI_FACTORY_ENGINEER_INDEX;

typedef struct _sensor_data_struct
{
  SENSOR_REG_STRUCT reg[OV5647MIPI_ENGINEER_END];
  SENSOR_REG_STRUCT cct[OV5647MIPI_FACTORY_END_ADDR];
} sensor_data_struct;

/* SENSOR PREVIEW/CAPTURE VT CLOCK */
#define OV5647MIPI_PREVIEW_CLK                     56000000
#define OV5647MIPI_CAPTURE_CLK                     80000000

#if 0
#define OV5647MIPI_COLOR_FORMAT                    SENSOR_OUTPUT_FORMAT_RAW_Gb //SENSOR_OUTPUT_FORMAT_RAW_R
#else
#define OV5647MIPI_COLOR_FORMAT                    SENSOR_OUTPUT_FORMAT_RAW_Gr
#endif

#define OV5647MIPI_MIN_ANALOG_GAIN				1	/* 1x */
#define OV5647MIPI_MAX_ANALOG_GAIN				32	/* 32x */


/* FRAME RATE UNIT */
#define OV5647MIPI_FPS(x)                          (10 * (x))

/* SENSOR PIXEL/LINE NUMBERS IN ONE PERIOD */
//#define OV5647MIPI_FULL_PERIOD_PIXEL_NUMS          2700 /* 9 fps */
#if 0
#define OV5647MIPI_FULL_PERIOD_PIXEL_NUMS          3055 /* 8 fps */
#define OV5647MIPI_FULL_PERIOD_LINE_NUMS           1968
#define OV5647MIPI_PV_PERIOD_PIXEL_NUMS            1630 /* 30 fps */
#define OV5647MIPI_PV_PERIOD_LINE_NUMS             984
#else
#define OV5647MIPI_FULL_PERIOD_PIXEL_NUMS          2700 /* 15 fps */
#define OV5647MIPI_FULL_PERIOD_LINE_NUMS           1974
#define OV5647MIPI_PV_PERIOD_PIXEL_NUMS            1896 /* 30 fps */
#define OV5647MIPI_PV_PERIOD_LINE_NUMS             984
#endif

#if 0
/* SENSOR START/END POSITION */
#define OV5647MIPI_FULL_X_START                    3
#define OV5647MIPI_FULL_Y_START                    11
#define OV5647MIPI_IMAGE_SENSOR_FULL_WIDTH         (2592 - 52) /* 2560 */
#define OV5647MIPI_IMAGE_SENSOR_FULL_HEIGHT        (1944 - 40) /* 1920 */
#define OV5647MIPI_PV_X_START                      1
#define OV5647MIPI_PV_Y_START                      1
#define OV5647MIPI_IMAGE_SENSOR_PV_WIDTH           (1280 - 16) /* 1264 */
#define OV5647MIPI_IMAGE_SENSOR_PV_HEIGHT          (960 - 12) /* 948 */
#else


#define OV5647MIPI_FULL_X_START                    4   //(1+16+6)
#define OV5647MIPI_FULL_Y_START                    4  //(1+12+4)
#define OV5647MIPI_IMAGE_SENSOR_FULL_WIDTH         (2560 - 32) //(2592 - 16) /* 2560 */
#define OV5647MIPI_IMAGE_SENSOR_FULL_HEIGHT        (1920 - 24) //(1944 - 12) /* 1920 */
#define OV5647MIPI_PV_X_START                      2
#define OV5647MIPI_PV_Y_START                      2
#define OV5647MIPI_IMAGE_SENSOR_PV_WIDTH           (1280 - 16) /* 1264 */
#define OV5647MIPI_IMAGE_SENSOR_PV_HEIGHT          (960 - 12) /* 948 */
#endif



/* SENSOR READ/WRITE ID */
#define OV5647MIPI_WRITE_ID (0x6c)
#define OV5647MIPI_READ_ID  (0x6d)

/* SENSOR ID */
//#define OV5647MIPI_SENSOR_ID						(0x5647)

/* SENSOR PRIVATE STRUCT */
typedef struct OV5647MIPI_sensor_STRUCT
{
  MSDK_SENSOR_CONFIG_STRUCT cfg_data;
  sensor_data_struct eng; /* engineer mode */
  MSDK_SENSOR_ENG_INFO_STRUCT eng_info;
  kal_uint8 mirror;
  kal_bool pv_mode;
  kal_bool video_mode;
  kal_bool NightMode;
  kal_uint16 normal_fps; /* video normal mode max fps */
  kal_uint16 night_fps; /* video night mode max fps */
  kal_uint16 FixedFps;
  kal_uint16 shutter;
  kal_uint16 gain;
  kal_uint32 pclk;
  kal_uint16 frame_height;
  kal_uint16 frame_height_BackUp;
  kal_uint16 line_length;  
} OV5647MIPI_sensor_struct;

//export functions
UINT32 OV5647MIPIOpen(void);
UINT32 OV5647MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 OV5647MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 OV5647MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 OV5647MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 OV5647MIPIClose(void);

#define Sleep(ms) mdelay(ms)

#endif 
