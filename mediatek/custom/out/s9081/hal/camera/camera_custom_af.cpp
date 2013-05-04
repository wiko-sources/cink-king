#include "camera_custom_af.h"

AF_COEF_T get_AF_Coef()
{
    AF_COEF_T sAFcoef;

    sAFcoef.i4AFS_STEP_MIN_ENABLE = 1;
    sAFcoef.i4AFS_STEP_MIN_MACRO  = 4;
    sAFcoef.i4AFS_STEP_MIN_NORMAL = 4;

    sAFcoef.i4AFC_THRES_MAIN        = 30;
    sAFcoef.i4AFC_THRES_SUB         = 20;
    
    sAFcoef.i4ZSD_HW_TH             = 12;
    sAFcoef.i4ZSD_AFS_THRES_MAIN    = 30;
    sAFcoef.i4ZSD_AFS_THRES_SUB     = 20;
    sAFcoef.i4ZSD_AFS_THRES_OFFSET  = 10000;
    sAFcoef.i4ZSD_AFC_THRES_MAIN    = 30;
    sAFcoef.i4ZSD_AFC_THRES_SUB     = 20;    
    sAFcoef.i4ZSD_AFC_THRES_OFFSET  = 10000;

    sAFcoef.i4ZSD_AFS_MONO_THRES    = 60;    
    sAFcoef.i4ZSD_AFS_MONO_OFFSET   = 100000;

    sAFcoef.i4AFS_WAIT              = 2; //1 AF frame wait
    sAFcoef.i4AFC_WAIT              = 10;

    sAFcoef.i4FV_CHANGE_OFFSET      = 1000;        
    sAFcoef.i4FV_STABLE_THRES       = 30;        
    sAFcoef.i4FV_STABLE_OFFSET      = 0;
    sAFcoef.i4FV_STABLE_CNT         = 5;                        

    sAFcoef.i4VIDEO_AFC_SPEED_ENABLE = 0;
    sAFcoef.i4VIDEO_AFC_NormalNum = 10;
    sAFcoef.i4VIDEO_AFC_TABLE[0] = 0;
    sAFcoef.i4VIDEO_AFC_TABLE[1] = 25;
    sAFcoef.i4VIDEO_AFC_TABLE[2] = 55;
    sAFcoef.i4VIDEO_AFC_TABLE[3] = 80;
    sAFcoef.i4VIDEO_AFC_TABLE[4] = 110;
    sAFcoef.i4VIDEO_AFC_TABLE[5] = 160;
    sAFcoef.i4VIDEO_AFC_TABLE[6] = 215;
    sAFcoef.i4VIDEO_AFC_TABLE[7] = 270;
    sAFcoef.i4VIDEO_AFC_TABLE[8] = 325;
    sAFcoef.i4VIDEO_AFC_TABLE[9] = 390;
    sAFcoef.i4VIDEO_AFC_TABLE[10] = 455;
    sAFcoef.i4VIDEO_AFC_TABLE[11] = 520;
    sAFcoef.i4VIDEO_AFC_TABLE[12] = 585;
    sAFcoef.i4VIDEO_AFC_TABLE[13] = 650;
    sAFcoef.i4VIDEO_AFC_TABLE[14] = 715;
    
	return sAFcoef;
}

