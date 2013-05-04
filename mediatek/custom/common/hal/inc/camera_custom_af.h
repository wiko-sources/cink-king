#ifndef _AF_COEF_H
#define _AF_COEF_H

#include "MediaTypes.h"

    typedef struct
    {
        MINT32 i4AFS_STEP_MIN_ENABLE;
        MINT32 i4AFS_STEP_MIN_NORMAL;
        MINT32 i4AFS_STEP_MIN_MACRO;

        MINT32 i4AFC_THRES_MAIN;
        MINT32 i4AFC_THRES_SUB;
        
        MINT32 i4ZSD_HW_TH;
        MINT32 i4ZSD_AFS_THRES_MAIN;
        MINT32 i4ZSD_AFS_THRES_SUB;
        MINT32 i4ZSD_AFS_THRES_OFFSET;
        MINT32 i4ZSD_AFC_THRES_MAIN;
        MINT32 i4ZSD_AFC_THRES_SUB;    
        MINT32 i4ZSD_AFC_THRES_OFFSET;
        
        MINT32 i4ZSD_AFS_MONO_THRES;    
        MINT32 i4ZSD_AFS_MONO_OFFSET;

        MINT32 i4AFS_WAIT;
        MINT32 i4AFC_WAIT;
        
        MINT32 i4FV_CHANGE_OFFSET;        
        MINT32 i4FV_STABLE_THRES;        
        MINT32 i4FV_STABLE_OFFSET;
        MINT32 i4FV_STABLE_CNT;                        

        MINT32 i4VIDEO_AFC_SPEED_ENABLE;
        MINT32 i4VIDEO_AFC_NormalNum;
        MINT32 i4VIDEO_AFC_TABLE[30];
        
    } AF_COEF_T;

	AF_COEF_T get_AF_Coef();
	
#endif /* _AF_COEF_H */

