/*******************************************************************************
 *
 * Filename:
 * ---------
 * cfg_audio_file.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 * This file is the header of audio customization related function or definition.
 *
 * Author:
 * -------
 * Ning.F
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 * 
 * 08 26 2012 weiguo.li
 * [ALPS00347285] [Need Patch] [Volunteer Patch]LGE AudioGainTable modification
 * .
 * 
 * 07 29 2012 weiguo.li
 * [ALPS00319405] ALPS.JB.BSP.PRA check in CR for Jades
 * .
 *
 * Jun 22 2009 mtk01352
 * [DUMA00007771] Moving modem side customization to AP
 *
 *
 * Apr 29 2009 mtk80306
 * [DUMA00116080] revise the customization of nvram
 * revise nvram customization
 *
 * Mar 21 2009 mtk80306
 * [DUMA00112158] fix the code convention.
 * modify code convention
 *
 * Mar 9 2009 mtk80306
 * [DUMA00111088] nvram customization
 * nvram customization
 *
 *
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef _CFG_AUDIO_FILE_H
#define _CFG_AUDIO_FILE_H

#define CUSTOM_VOLUME_STEP (7)
#define NB_FIR_NUM (45)
#define NB_FIR_INDEX_NUM   (6)
#define SPEECH_COMMON_NUM (12)
#define SPEECH_PARA_MODE_NUM     (8)
#define SPEECH_PARA_NUM                (16)
#define AUDIO_EQ_PARAM_NUM         (8)

#define WB_FIR_NUM (90)
#define WB_FIR_INDEX_NUM   (6)

#define HD_REC_MODE_INDEX_NUM   (30)
#define HD_REC_FIR_INDEX_NUM    (8)
/* audio nvram structure definition*/
typedef enum
{
    VOL_NORMAL   = 0 ,
    VOL_HEADSET      ,
    VOL_HANDFREE     ,
    MAX_VOL_CATE
} volume_category_enum;

typedef enum
{
    VOL_TYPE_TON  = 0 ,
    VOL_TYPE_KEY      ,
    VOL_TYPE_MIC      ,
    VOL_TYPE_FMR      ,
    VOL_TYPE_SPH      ,
    VOL_TYPE_SID	    ,
    VOL_TYPE_MEDIA    ,
    MAX_VOL_TYPE
} volume_type_enum;

#define     NUM_ABF_PARAM 44
#define     NUM_ABFWB_PARAM 76

typedef struct _AUDIO_CUSTOM_EXTRA_PARAM_STRUCT
{
    /* ABF parameters */
    unsigned short ABF_para[NUM_ABF_PARAM + NUM_ABFWB_PARAM];    //with WB
} AUDIO_CUSTOM_EXTRA_PARAM_STRUCT;

#define CFG_FILE_SPEECH_DUAL_MIC_SIZE    sizeof(AUDIO_CUSTOM_EXTRA_PARAM_STRUCT)
#define CFG_FILE_SPEECH_DUAL_MIC_TOTAL   1

typedef struct _AUDIO_CUSTOM_PARAM_STRUCT
{
    /* volume setting */
    unsigned char volume[MAX_VOL_CATE][MAX_VOL_TYPE];
    /* speech enhancement */
    unsigned short speech_common_para[SPEECH_COMMON_NUM];
    unsigned short speech_mode_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
    unsigned short speech_volume_para[4];//in the feature, should extend to [MAX_VOL_CATE][MAX_VOL_TYPE][4]
    /* debug info */
    unsigned short debug_info[16];
    /* speech input FIR */
    short          sph_in_fir[NB_FIR_INDEX_NUM][NB_FIR_NUM];
    /* speech output FIR */
    short          sph_out_fir[NB_FIR_INDEX_NUM][NB_FIR_NUM];
    /* digital gain of DL speech */
    unsigned short Digi_DL_Speech;
    /* digital gain of uplink speech */
    unsigned short Digi_Microphone;
    /* FM record volume*/
    unsigned short FM_Record_Volume;
    /* user mode : normal mode, earphone mode, loud speaker mode */
    unsigned short Bluetooth_Sync_Type;
    unsigned short Bluetooth_Sync_Length;
    unsigned short bt_pcm_in_vol;
    unsigned short bt_pcm_out_vol;
    unsigned short user_mode;
    /* auto VM record setting */
    unsigned short bSupportVM;
    unsigned short bAutoVM;
    // mic bias
    unsigned short uMicbiasVolt;

} AUDIO_CUSTOM_PARAM_STRUCT;

#define CFG_FILE_SPEECH_REC_SIZE        sizeof(AUDIO_CUSTOM_PARAM_STRUCT)
#define CFG_FILE_SPEECH_REC_TOTAL   1

typedef struct _AUDIO_CUSTOM_WB_A2M_PARAM_STRUCT_
{
    /* WB speech enhancement */
    unsigned short speech_mode_wb_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
    /* WB speech input/output FIR */
    short          sph_wb_fir[WB_FIR_INDEX_NUM][WB_FIR_NUM];
    /* in_out flag */
    short          input_out_fir_flag; // 0: input, 1: output
} AUDIO_CUSTOM_WB_A2M_PARAM_STRUCT;

typedef struct _AUDIO_CUSTOM_WB_PARAM_STRUCT
{
    /* WB speech enhancement */
    unsigned short speech_mode_wb_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
    /* WB speech input FIR */
    short          sph_wb_in_fir[WB_FIR_INDEX_NUM][WB_FIR_NUM];
    /* WB speech output FIR */
    short          sph_wb_out_fir[WB_FIR_INDEX_NUM][WB_FIR_NUM];
} AUDIO_CUSTOM_WB_PARAM_STRUCT;

#define CFG_FILE_WB_SPEECH_REC_SIZE        sizeof(AUDIO_CUSTOM_WB_PARAM_STRUCT)
#define CFG_FILE_WB_SPEECH_REC_TOTAL   1

typedef struct _AUDIO_ACF_CUSTOM_PARAM_STRUCT
{
    /* Compensation Filter HSF coeffs       */
    /* BesLoudness also uses this coeffs    */
    unsigned int bes_loudness_hsf_coeff[9][4];

    /* Compensation Filter BPF coeffs       */
    unsigned int bes_loudness_bpf_coeff[4][6][3];
    unsigned int bes_loudness_DRC_Forget_Table[9][2];
    unsigned int bes_loudness_WS_Gain_Max;
    unsigned int bes_loudness_WS_Gain_Min;
    unsigned int bes_loudness_Filter_First;
    char bes_loudness_Gain_Map_In[5];
    char bes_loudness_Gain_Map_Out[5];

} AUDIO_ACF_CUSTOM_PARAM_STRUCT;

#define CFG_FILE_AUDIO_COMPFLT_REC_SIZE        sizeof(AUDIO_ACF_CUSTOM_PARAM_STRUCT)
#define CFG_FILE_AUDIO_COMPFLT_REC_TOTAL   1
#define CFG_FILE_HEADPHONE_COMPFLT_REC_TOTAL   1

typedef struct _AUDIO_EFFECT_CUSTOM_PARAM_STRUCT
{
    //surround parameters
    int bsrd_level;
    unsigned int Distance1;
    unsigned int Distance2;
    int bsrd_band_select;

    //bass parameters
    unsigned int bass_CutoffFreq;
    int bass_IsVB;

    //EQ parameters
    short Normal_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Dance_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Bass_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Classical_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Treble_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Party_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Pop_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Rock_Gain_dB_level[AUDIO_EQ_PARAM_NUM];

    //loudness mode
    int LoudEnhancemode;

    // time stretch
    int Time_TD_FD;
    int Time_TS_Ratio;

} AUDIO_EFFECT_CUSTOM_PARAM_STRUCT;

#define CFG_FILE_AUDIO_EFFECT_REC_SIZE        sizeof(AUDIO_EFFECT_CUSTOM_PARAM_STRUCT)
#define CFG_FILE_AUDIO_EFFECT_REC_TOTAL   1

typedef struct _AUDIO_PARAM_MED_STRUCT
{
    short speech_input_FIR_coeffs[SPEECH_PARA_MODE_NUM][NB_FIR_NUM];
    short speech_output_FIR_coeffs[SPEECH_PARA_MODE_NUM][NB_FIR_INDEX_NUM][NB_FIR_NUM];
    short select_FIR_output_index[SPEECH_PARA_MODE_NUM];
    short select_FIR_intput_index[SPEECH_PARA_MODE_NUM];
    short speech_mode_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
} AUDIO_PARAM_MED_STRUCT;

#define CFG_FILE_AUDIO_PARAM_MED_REC_SIZE        sizeof(AUDIO_PARAM_MED_STRUCT)
#define CFG_FILE_AUDIO_PARAM_MED_REC_TOTAL   1


typedef struct _AUDIO_VOLUME_CUSTOM_STRUCT
{
    unsigned char audiovolume_ring[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_key[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_mic[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_fmr[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_sph[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_sid[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_media[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_matv[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
} AUDIO_VOLUME_CUSTOM_STRUCT;

#define CFG_FILE_AUDIO_VOLUME_CUSTOM_REC_SIZE        sizeof(AUDIO_VOLUME_CUSTOM_STRUCT)
#define CFG_FILE_AUDIO_VOLUME_CUSTOM_REC_TOTAL   1


/********************************************************************
*   Audio Gain Table
*********************************************************************/
#define GAIN_TABLE_LEVEL (20)

#define VOICE_GAIN_TABLE_LEVEL (6+1)
#define SYSTEM_GAIN_TABLE_LEVEL (7+1)
#define RING_GAIN_TABLE_LEVEL (7+1)
#define MUSIC_GAIN_TABLE_LEVEL (13+1)
#define ALARM_GAIN_TABLE_LEVEL (7+1)
#define NOTIFICATION_GAIN_TABLE_LEVEL (7+1)
#define BLUETOOTHSCO_GAIN_TABLE_LEVEL (15+1)
#define ENFORCEAUDIBLE_GAIN_TABLE_LEVEL (7+1)
#define DTMF_GAIN_TABLE_LEVEL (15+1)
#define TTS_GAIN_TABLE_LEVEL (15+1)
#define FM_GAIN_TABLE_LEVEL (13+1)
#define MATV_GAIN_TABLE_LEVEL (13+1)

//speech should equal to voice call level
#define SPEECH_GAIN_TABLE_LEVLE (VOICE_GAIN_TABLE_LEVEL)

enum AUDIO_GAIN_TYPE {
	AUDIO_GAIN_DEFAULT 		    =-1,
	AUDIO_GAIN_VOICE_CALL		= 0,
	AUDIO_GAIN_SYSTEM			= 1,
	AUDIO_GAIN_RING			    = 2,
	AUDIO_GAIN_MUSIC			= 3,
	AUDIO_GAIN_ALARM			= 4,
	AUDIO_GAIN_NOTIFICATION	    = 5,
	AUDIO_GAIN_BLUETOOTH_SCO	= 6,
	AUDIO_GAIN_ENFORCED_AUDIBLE = 7, // Sounds that cannot be muted by user and must be routed to speaker
	AUDIO_GAIN_DTMF			    = 8,
	AUDIO_GAIN_TTS 			    = 9,
	AUDIO_GAIN_FM				= 10,
	AUDIO_GAIN_MAX_STREAM       = 10, //max index of stream
	AUDIO_GAIN_MIC              = 11,
	AUDIO_GAIN_SIDETONE         = 12,
	AUDIO_GAIN_SPEECH           = 13,
    NUM_AUDIO_GAIN_TYPES
};

enum GAIN_OUTPUT_DEVICE
{
	GAIN_OUTPUT_RECEIVER = 0,
	GAIN_OUTPUT_HEADSET  = 1,
	GAIN_OUTPUT_SPEAKER  = 2,
	NUM_GAIN_OUTPUT_DEVICES
};

enum MICROPHONE_DEVICEGAIN{
    GAIN_IDLE_RECORD_MIC =0,
    GAIN_IDLE_RECORD_HEADSET ,
    GAIN_INCALL_RECEIVER,
    GAIN_INCALL_HEADSET,        
    GAIN_INCALL_SPEAKER,
    GAIN_VOIP_RECEIVER, 
    GAIN_VOIP_HEADSET,
    GAIN_VOIP_SPEAKER,
    GAIN_FM_RECORDING, 
    GAIN_TTY_DEVICE,
    GAIN_VOICE_RECOGNITION,
    NUM_OF_MICGAINS
};

enum SIDETONE_DEVICEGAIN{
    GAIN_SIDETONE_RECEIVER =0,
    GAIN_SIDETONE_HEADSET,
    GAIN_SIDETONE_SPEAKER,
    NUM_OF_SIDETONEGAINS
};

enum GAIN_SPEEECH{
	SPEECH_RECEIVER = 0,
    SPEECH_HEADSET  = 1,
    SPEECH_SPEAKER  = 2,
    NUM_OF_SPEECHSGAINS
};

typedef struct _AUDIO_GAIN_CONTROL_STRUCT
{
    unsigned char u8Gain_digital;
    unsigned int u32Gain_PGA_Amp;
} AUDIO_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_GAIN_CONTROL_STRUCT
{
    AUDIO_GAIN_CONTROL_STRUCT streamGain[NUM_GAIN_OUTPUT_DEVICES][GAIN_TABLE_LEVEL]; 
}STREAM_GAIN_CONTROL_STRUCT;

typedef struct _STREAMS_GAIN_CONTROL_STRUCT
{
    STREAM_GAIN_CONTROL_STRUCT voiceCall; 
    STREAM_GAIN_CONTROL_STRUCT system; 
    STREAM_GAIN_CONTROL_STRUCT ring; 
    STREAM_GAIN_CONTROL_STRUCT music; 
    STREAM_GAIN_CONTROL_STRUCT alarm; 
    STREAM_GAIN_CONTROL_STRUCT notification; 
    STREAM_GAIN_CONTROL_STRUCT blueToothSco; 
    STREAM_GAIN_CONTROL_STRUCT enforceAudible; 
    STREAM_GAIN_CONTROL_STRUCT dtmf; 
    STREAM_GAIN_CONTROL_STRUCT tts; 
    STREAM_GAIN_CONTROL_STRUCT fm;
}STREAMS_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_MICROPHONE_GAIN_CONTROL_STRUCT
{
    unsigned char micGain[GAIN_TABLE_LEVEL];
} STREAM_MICROPHONE_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_SIDETONE_GAIN_CONTROL_STRUCT
{
    unsigned char sidetoneGain[GAIN_TABLE_LEVEL];
} STREAM_SIDETONE_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_SPEECH_GAIN_CONTROL_STRUCT
{
    AUDIO_GAIN_CONTROL_STRUCT speechGain[NUM_GAIN_OUTPUT_DEVICES][GAIN_TABLE_LEVEL];
} STREAM_SPEECH_GAIN_CONTROL_STRUCT;

typedef struct _AUDIO_GAIN_TABLE_STRUCT
{
    STREAM_GAIN_CONTROL_STRUCT voiceCall; 
    STREAM_GAIN_CONTROL_STRUCT system; 
    STREAM_GAIN_CONTROL_STRUCT ring; 
    STREAM_GAIN_CONTROL_STRUCT music; 
    STREAM_GAIN_CONTROL_STRUCT alarm; 
    STREAM_GAIN_CONTROL_STRUCT notification; 
    STREAM_GAIN_CONTROL_STRUCT blueToothSco; 
    STREAM_GAIN_CONTROL_STRUCT enforceAudible; 
    STREAM_GAIN_CONTROL_STRUCT dtmf; 
    STREAM_GAIN_CONTROL_STRUCT tts; 
    STREAM_GAIN_CONTROL_STRUCT fm;
    STREAM_MICROPHONE_GAIN_CONTROL_STRUCT microphoneGain;
    STREAM_SIDETONE_GAIN_CONTROL_STRUCT sidetoneGain;
    STREAM_SPEECH_GAIN_CONTROL_STRUCT speechGain;
} AUDIO_GAIN_TABLE_STRUCT;

#define CFG_FILE_AUDIO_GAIN_TABLE_CUSTOM_REC_SIZE        sizeof(AUDIO_GAIN_TABLE_STRUCT)
#define CFG_FILE_AUDIO_GAIN_TABLE_CUSTOM_REC_TOTAL   1


//#if defined(MTK_HD_RECORD_SUPPORT)

#define SPC_MAX_NUM_RECORD_SPH_MODE 30
#define SPC_MAX_NUM_RECORD_INPUT_FIR 8
typedef struct _AUDIO_HD_RECORD_PARAM_STRUCT
{
    /* HD RECORD Mode Num & FIR Num*/
    unsigned short hd_rec_mode_num; // max(hd_rec_fir_num) == 30 ??
    unsigned short hd_rec_fir_num;  // max(hd_rec_fir_num) == 4 ??

    /* HD RECORD Speech Enhancement */
    unsigned short hd_rec_speech_mode_para[SPC_MAX_NUM_RECORD_SPH_MODE][SPEECH_PARA_NUM]; // the contain only have hd_rec_mode_num effective values

    /* HD RECORD FIR */
    short          hd_rec_fir[SPC_MAX_NUM_RECORD_INPUT_FIR][WB_FIR_NUM]; // the contain only have hd_rec_fir_num effective values

    /* HD RECORD FIR Mapping (ex, map[sph_mode] = FIR3) */
    unsigned short hd_rec_map_to_fir_for_ch1[SPC_MAX_NUM_RECORD_SPH_MODE];
    unsigned short hd_rec_map_to_fir_for_ch2[SPC_MAX_NUM_RECORD_SPH_MODE];

    /* HD RECORD Device Mode Mapping (ex, map[sph_mode] = SPH_MODE_NORMAL ) */
    unsigned char hd_rec_map_to_dev_mode[SPC_MAX_NUM_RECORD_SPH_MODE];

    /* HD RECORD Input Source Mapping (ex, map[sph_mode] = BT Earphone mic)*/
    unsigned char hd_rec_map_to_input_src[SPC_MAX_NUM_RECORD_SPH_MODE];

    /* HD RECORD mode is stereo or not (ex, map[sph_mode] = 0(mono), 1(stereo) )*/
    unsigned char hd_rec_map_to_stereo_flag[SPC_MAX_NUM_RECORD_SPH_MODE];

} AUDIO_HD_RECORD_PARAM_STRUCT;


#define CFG_FILE_AUDIO_HD_REC_PAR_SIZE   sizeof(AUDIO_HD_RECORD_PARAM_STRUCT)
#define CFG_FILE_AUDIO_HD_REC_PAR_TOTAL  1



#define MAX_HD_REC_SCENES 10  // max #scene = 10 (10 * 3 = 30 = max modes)

enum HD_REC_DEVICE_SOURCE_T
{
    HD_REC_DEVICE_SOURCE_HANDSET     = 0,
    HD_REC_DEVICE_SOURCE_HEADSET     = 1,
    HD_REC_DEVICE_SOURCE_BT_EARPHONE = 2,
    NUM_HD_REC_DEVICE_SOURCE
};

typedef struct
{
    unsigned char num_voice_rec_scenes;
    unsigned char num_video_rec_scenes;
    unsigned char scene_table[MAX_HD_REC_SCENES][NUM_HD_REC_DEVICE_SOURCE];
} AUDIO_HD_RECORD_SCENE_TABLE_STRUCT;

#define CFG_FILE_AUDIO_HD_REC_SCENE_TABLE_SIZE   sizeof(AUDIO_HD_RECORD_SCENE_TABLE_STRUCT)
#define CFG_FILE_AUDIO_HD_REC_SCENE_TABLE_TOTAL  1
//#endif  //MTK_HD_RECORD_SUPPORT

#endif // _CFG_AUDIO_FILE_H

