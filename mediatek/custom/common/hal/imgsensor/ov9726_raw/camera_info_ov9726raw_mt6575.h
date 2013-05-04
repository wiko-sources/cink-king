#ifndef _CAMERA_INFO_OV9726RAW_MT6575_H
#define _CAMERA_INFO_OV9726RAW_MT6575_H

/*******************************************************************************
*   Configuration
********************************************************************************/
#define SENSOR_ID                           OV9726_SENSOR_ID
#define SENSOR_DRVNAME                      SENSOR_DRVNAME_0V9726_RAW
#define INCLUDE_FILENAME_ISP_REGS_PARAM     "camera_isp_regs_ov9726raw_mt6575.h"
#define INCLUDE_FILENAME_ISP_PCA_PARAM      "camera_isp_pca_ov9726raw_mt6575.h"

/*******************************************************************************
*   
********************************************************************************/

#if defined(ISP_SUPPORT)

#define OV9726RAW_CAMERA_AUTO_DSC CAM_AUTO_DSC
#define OV9726RAW_CAMERA_PORTRAIT CAM_PORTRAIT
#define OV9726RAW_CAMERA_LANDSCAPE CAM_LANDSCAPE
#define OV9726RAW_CAMERA_SPORT CAM_SPORT
#define OV9726RAW_CAMERA_FLOWER CAM_FLOWER
#define OV9726RAW_CAMERA_NIGHTSCENE CAM_NIGHTSCENE
#define OV9726RAW_CAMERA_DOCUMENT CAM_DOCUMENT
#define OV9726RAW_CAMERA_ISO_ANTI_HAND_SHAKE CAM_ISO_ANTI_HAND_SHAKE
#define OV9726RAW_CAMERA_ISO100 CAM_ISO100
#define OV9726RAW_CAMERA_ISO200 CAM_ISO200
#define OV9726RAW_CAMERA_ISO400 CAM_ISO400
#define OV9726RAW_CAMERA_ISO800 CAM_ISO800
#define OV9726RAW_CAMERA_ISO1600 CAM_ISO1600
#define OV9726RAW_CAMERA_VIDEO_AUTO CAM_VIDEO_AUTO
#define OV9726RAW_CAMERA_VIDEO_NIGHT CAM_VIDEO_NIGHT
#define OV9726RAW_CAMERA_NO_OF_SCENE_MODE CAM_NO_OF_SCENE_MODE

#endif
#endif
