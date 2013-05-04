/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2005
 *
 *  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 *  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
 *  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 *  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 *  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 *  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
 *  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
 *  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 *  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 *  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 *  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
 *  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
 *  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
 *  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
 *  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
 *  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Filename:   
 * ---------
 *   sp0838yuv_sub_Sensor.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   Image sensor driver function
 *   V1.2.3
 *
 * Author:
 * -------
 *   Leo
 *
 *=============================================================
 *             HISTORY
 * Below this line, this part is controlled by GCoreinc. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log$
 * 2012.02.29  kill bugs
 *   
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by GCoreinc. DO NOT MODIFY!!
 *=============================================================
 ******************************************************************************/
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "sp0838yuv_Sensor.h"
#include "sp0838yuv_Camera_Sensor_para.h"
#include "sp0838yuv_CameraCustomized.h"

//#include <mt6575_gpio.h>

#define SP0838_subYUV_DEBUG
#ifdef SP0838_subYUV_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif

static DEFINE_SPINLOCK(sp0838_yuv_drv_lock);

extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);

kal_uint16 SP0838_sub_write_cmos_sensor(kal_uint8 addr, kal_uint8 para)
{
	char puSendCmd[2] = {(char)(addr & 0xFF) , (char)(para & 0xFF)};

	iWriteRegI2C(puSendCmd , 2, SP0838_SUB_WRITE_ID);
}
kal_uint16 SP0838_sub_read_cmos_sensor(kal_uint8 addr)
{
	kal_uint16 get_byte=0;
	char puSendCmd = { (char)(addr & 0xFF) };
	iReadRegI2C(&puSendCmd , 1, (u8*)&get_byte, 1, SP0838_SUB_WRITE_ID);

	return get_byte;
}


/*******************************************************************************
 * // Adapter for Winmo typedef
 ********************************************************************************/
#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x,...)
#define TEXT

kal_bool   SP0838_sub_MPEG4_encode_mode = KAL_FALSE;
kal_uint16 SP0838_sub_dummy_pixels = 0, SP0838_sub_dummy_lines = 0;
kal_bool   SP0838_sub_MODE_CAPTURE = KAL_FALSE;
kal_bool   SP0838_sub_NIGHT_MODE = KAL_FALSE;

kal_uint32 SP0838_sub_isp_master_clock;
static kal_uint32 SP0838_sub_g_fPV_PCLK = 24;

kal_uint8 SP0838_sub_sensor_write_I2C_address = SP0838_SUB_WRITE_ID;
kal_uint8 SP0838_sub_sensor_read_I2C_address = SP0838_SUB_READ_ID;

UINT8 SP0838_subPixelClockDivider=0;

kal_uint8 sp0838_isBanding = 0; // 0: 50hz  1:60hz

MSDK_SENSOR_CONFIG_STRUCT SP0838_subSensorConfigData;



/*************************************************************************
 * FUNCTION
 *	SP0838_sub_SetShutter
 *
 * DESCRIPTION
 *	This function set e-shutter of SP0838_sub to change exposure time.
 *
 * PARAMETERS
 *   iShutter : exposured lines
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
void SP0838_sub_Set_Shutter(kal_uint16 iShutter)
{
} /* Set_SP0838_sub_Shutter */


/*************************************************************************
 * FUNCTION
 *	SP0838_sub_read_Shutter
 *
 * DESCRIPTION
 *	This function read e-shutter of SP0838_sub .
 *
 * PARAMETERS
 *  None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
kal_uint16 SP0838_sub_Read_Shutter(void)
{
	kal_uint8 temp_reg1, temp_reg2;
	kal_uint16 shutter;

	temp_reg1 = SP0838_sub_read_cmos_sensor(0x04);
	temp_reg2 = SP0838_sub_read_cmos_sensor(0x03);

	shutter = (temp_reg1 & 0xFF) | (temp_reg2 << 8);

	return shutter;
} /* SP0838_sub_read_shutter */


/*************************************************************************
 * FUNCTION
 *	SP0838_sub_write_reg
 *
 * DESCRIPTION
 *	This function set the register of SP0838_sub.
 *
 * PARAMETERS
 *	addr : the register index of SP0838_sub
 *  para : setting parameter of the specified register of SP0838_sub
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
void SP0838_sub_write_reg(kal_uint32 addr, kal_uint32 para)
{
	SP0838_sub_write_cmos_sensor(addr, para);
} /* SP0838_sub_write_reg() */


/*************************************************************************
 * FUNCTION
 *	SP0838_sub_read_cmos_sensor
 *
 * DESCRIPTION
 *	This function read parameter of specified register from SP0838_sub.
 *
 * PARAMETERS
 *	addr : the register index of SP0838_sub
 *
 * RETURNS
 *	the data that read from SP0838_sub
 *
 * GLOBALS AFFECTED   
 *
 *************************************************************************/
kal_uint32 SP0838_sub_read_reg(kal_uint32 addr)
{
	return SP0838_sub_read_cmos_sensor(addr);
} /* OV7670_read_reg() */


/*************************************************************************
* FUNCTION
*	SP0838_sub_awb_enable
*
* DESCRIPTION
*	This function enable or disable the awb (Auto White Balance).
*
* PARAMETERS
*	1. kal_bool : KAL_TRUE - enable awb, KAL_FALSE - disable awb.
*
* RETURNS
*	kal_bool : It means set awb right or not.
*
*************************************************************************/
static void SP0838_sub_awb_enable(kal_bool enalbe)
{
#if 0
	kal_uint16 ae_value = 0 , awb_value=0;

	SP0838_sub_write_cmos_sensor(0xfd,0x00);
	ae_value = SP0838_sub_read_cmos_sensor(0x32);
	awb_value = SP0838_sub_read_cmos_sensor(0x32);

	if(0x01==ae_enable)
	{
		ae_value |= 0x05;       
	}
	else if(0x00==ae_enable)
	{
		ae_value &= 0xf0;
	}

	if(0x01==awb_enable)  
	{
		awb_value |= 0x10;
	}
	else if(0x00==awb_enable)
	{
		awb_value &= 0xef;
	}	

	SP0838_sub_write_cmos_sensor(0x32, ae_value);
	SP0838_sub_write_cmos_sensor(0x32, awb_value);
#else
	{	 
	kal_uint16 temp_AWB_reg = 0;

	temp_AWB_reg = SP0838_sub_read_cmos_sensor(0x42);

	if (enalbe)
	{
		SP0838_sub_write_cmos_sensor(0x42, (temp_AWB_reg |0x02));
	}
	else
	{
		SP0838_sub_write_cmos_sensor(0x42, (temp_AWB_reg & (~0x02)));
	}

	}
#endif

	return 0;
}



/*************************************************************************
 * FUNCTION
 *	SP0838_sub_config_window
 *
 * DESCRIPTION
 *	This function config the hardware window of SP0838_sub for getting specified
 *  data of that window.
 *
 * PARAMETERS
 *	start_x : start column of the interested window
 *  start_y : start row of the interested window
 *  width  : column widht of the itnerested window
 *  height : row depth of the itnerested window
 *
 * RETURNS
 *	the data that read from SP0838_sub
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
void SP0838_sub_config_window(kal_uint16 startx, kal_uint16 starty, kal_uint16 width, kal_uint16 height)
{
} /* SP0838_sub_config_window */


/*************************************************************************
 * FUNCTION
 *	SP0838_sub_SetGain
 *
 * DESCRIPTION
 *	This function is to set global gain to sensor.
 *
 * PARAMETERS
 *   iGain : sensor global gain(base: 0x40)
 *
 * RETURNS
 *	the actually gain set to sensor.
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
kal_uint16 SP0838_sub_SetGain(kal_uint16 iGain)
{
	return iGain;
}


/*************************************************************************
 * FUNCTION
 *	SP0838_sub_NightMode
 *
 * DESCRIPTION
 *	This function night mode of SP0838_sub.
 *
 * PARAMETERS
 *	bEnable: KAL_TRUE -> enable night mode, otherwise, disable night mode
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
void SP0838_subNightMode(kal_bool bEnable)
{
	// kal_uint8 temp = SP0838_read_cmos_sensor(0x3B);
	if(bEnable)//night mode
	{ 					
		if(SP0838_sub_MPEG4_encode_mode == KAL_TRUE)
		{
			if(sp0838_isBanding== 0)
			{
				//Video record night 24M 50hz 20-10FPS maxgain:0x78                 
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x62);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5f);
				SP0838_sub_write_cmos_sensor(0xf5,0x78);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xb6);
				SP0838_sub_write_cmos_sensor(0x0f,0x60);
				SP0838_sub_write_cmos_sensor(0x16,0x60);
				SP0838_sub_write_cmos_sensor(0x17,0x9e);
				SP0838_sub_write_cmos_sensor(0x18,0xa6);
				SP0838_sub_write_cmos_sensor(0x1b,0x60);
				SP0838_sub_write_cmos_sensor(0x1c,0xa6);
				SP0838_sub_write_cmos_sensor(0xb4,0x20);
				SP0838_sub_write_cmos_sensor(0xb5,0x3a);
				SP0838_sub_write_cmos_sensor(0xb6,0x5e);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x42);
				SP0838_sub_write_cmos_sensor(0xbf,0x42);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x78);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x41);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x78);
				SP0838_sub_write_cmos_sensor(0xcb,0xa );
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x1f);
				SP0838_sub_write_cmos_sensor(0xfd,0x00);			
				//dbg_print(" video 50Hz night\r\n");			

				SENSORDB(" video 50Hz night\r\n");
			}
			else if(sp0838_isBanding == 1)
			{
				//Video record night 24M 60hz 20-10FPS maxgain:0x78               
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x51);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5c);
				SP0838_sub_write_cmos_sensor(0xf5,0x75);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xb7);
				SP0838_sub_write_cmos_sensor(0x0f,0x5d);
				SP0838_sub_write_cmos_sensor(0x16,0x5d);
				SP0838_sub_write_cmos_sensor(0x17,0x9f);
				SP0838_sub_write_cmos_sensor(0x18,0xa7);
				SP0838_sub_write_cmos_sensor(0x1b,0x5d);
				SP0838_sub_write_cmos_sensor(0x1c,0xa7);
				SP0838_sub_write_cmos_sensor(0xb4,0x21);
				SP0838_sub_write_cmos_sensor(0xb5,0x3d);
				SP0838_sub_write_cmos_sensor(0xb6,0x4d);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x42);
				SP0838_sub_write_cmos_sensor(0xbf,0x42);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x41);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x78);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x78);
				SP0838_sub_write_cmos_sensor(0xcb,0xc );
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x1f);
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				//dbg_print(" video 60Hz night\r\n");		
				SENSORDB(" video 60Hz night\r\n");
			}
		}	
		else 
		{  
			if(sp0838_isBanding== 0)
			{
				//caprure preview night 24M 50hz 20-6FPS 
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x62);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5f);
				SP0838_sub_write_cmos_sensor(0xf5,0x78);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xc0);
				SP0838_sub_write_cmos_sensor(0x0f,0x60);
				SP0838_sub_write_cmos_sensor(0x16,0x60);
				SP0838_sub_write_cmos_sensor(0x17,0xa8);
				SP0838_sub_write_cmos_sensor(0x18,0xb0);
				SP0838_sub_write_cmos_sensor(0x1b,0x60);
				SP0838_sub_write_cmos_sensor(0x1c,0xb0);
				SP0838_sub_write_cmos_sensor(0xb4,0x20);
				SP0838_sub_write_cmos_sensor(0xb5,0x3a);
				SP0838_sub_write_cmos_sensor(0xb6,0x5e);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x42);
				SP0838_sub_write_cmos_sensor(0xbf,0x42);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x41);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x41);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x78);
				SP0838_sub_write_cmos_sensor(0xcb,0x10);
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x1f);
				SP0838_sub_write_cmos_sensor(0xfd,0x00);			
				//dbg_print(" priview 50Hz night\r\n");			
				SENSORDB(" priview 50Hz night\r\n");	
			}  
			else if(sp0838_isBanding== 1)
			{
				//caprure preview night 24M 60hz 20-6FPS
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x51);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5c);
				SP0838_sub_write_cmos_sensor(0xf5,0x75);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xc1);
				SP0838_sub_write_cmos_sensor(0x0f,0x5d);
				SP0838_sub_write_cmos_sensor(0x16,0x5d);
				SP0838_sub_write_cmos_sensor(0x17,0xa9);
				SP0838_sub_write_cmos_sensor(0x18,0xb1);
				SP0838_sub_write_cmos_sensor(0x1b,0x5d);
				SP0838_sub_write_cmos_sensor(0x1c,0xb1);
				SP0838_sub_write_cmos_sensor(0xb4,0x21);
				SP0838_sub_write_cmos_sensor(0xb5,0x3d);
				SP0838_sub_write_cmos_sensor(0xb6,0x4d);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x42);
				SP0838_sub_write_cmos_sensor(0xbf,0x42);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x41);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x41);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x78);
				SP0838_sub_write_cmos_sensor(0xcb,0x14);
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x1f);
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				//	dbg_print(" priview 60Hz night\r\n");	
				SENSORDB(" priview 60Hz night\r\n");	
			}
		} 		
	}
	else    // daylight mode
	{
		if(SP0838_sub_MPEG4_encode_mode == KAL_TRUE)
		{
			if(sp0838_isBanding== 0)
			{
				//Video record daylight 24M 50hz fix 20FPS maxgain:0x78 
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x62);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5f);
				SP0838_sub_write_cmos_sensor(0xf5,0x78);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xa9);
				SP0838_sub_write_cmos_sensor(0x0f,0x60);
				SP0838_sub_write_cmos_sensor(0x16,0x60);
				SP0838_sub_write_cmos_sensor(0x17,0x91);
				SP0838_sub_write_cmos_sensor(0x18,0x99);
				SP0838_sub_write_cmos_sensor(0x1b,0x60);
				SP0838_sub_write_cmos_sensor(0x1c,0x99);
				SP0838_sub_write_cmos_sensor(0xb4,0x20);
				SP0838_sub_write_cmos_sensor(0xb5,0x3a);
				SP0838_sub_write_cmos_sensor(0xb6,0x5e);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x78);
				SP0838_sub_write_cmos_sensor(0xbf,0x42);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x41);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x41);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x78);
				SP0838_sub_write_cmos_sensor(0xcb,0x5 );
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x0f);		
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				//dbg_print(" video 50Hz normal\r\n");			
				SENSORDB(" video 50Hz normal\r\n");				
			}
			else if(sp0838_isBanding == 1)
			{
				//Video record daylight 24M 60hz fix 20FPS
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x51);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5c);
				SP0838_sub_write_cmos_sensor(0xf5,0x75);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xa9);
				SP0838_sub_write_cmos_sensor(0x0f,0x5d);
				SP0838_sub_write_cmos_sensor(0x16,0x5d);
				SP0838_sub_write_cmos_sensor(0x17,0x91);
				SP0838_sub_write_cmos_sensor(0x18,0x99);
				SP0838_sub_write_cmos_sensor(0x1b,0x5d);
				SP0838_sub_write_cmos_sensor(0x1c,0x99);
				SP0838_sub_write_cmos_sensor(0xb4,0x21);
				SP0838_sub_write_cmos_sensor(0xb5,0x3d);
				SP0838_sub_write_cmos_sensor(0xb6,0x4d);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x42);
				SP0838_sub_write_cmos_sensor(0xbf,0x78);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x41);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x41);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x78);
				SP0838_sub_write_cmos_sensor(0xcb,0x6 );
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x0f);
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				//dbg_print(" video 60Hz normal\r\n");			
				SENSORDB(" video 60Hz normal \n");	
			}
		}
		else 
		{
			//	SENSORDB(" SP0838_banding=%x\r\n",SP0838_banding);
			if(sp0838_isBanding== 0)
			{
				//caprure preview normal 24M 50hz 20-8FPS maxgain:0x78                  
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x62);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5f);
				SP0838_sub_write_cmos_sensor(0xf5,0x78);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xb2);
				SP0838_sub_write_cmos_sensor(0x0f,0x60);
				SP0838_sub_write_cmos_sensor(0x16,0x60);
				SP0838_sub_write_cmos_sensor(0x17,0xa2);
				SP0838_sub_write_cmos_sensor(0x18,0xaa);
				SP0838_sub_write_cmos_sensor(0x1b,0x60);
				SP0838_sub_write_cmos_sensor(0x1c,0xaa);
				SP0838_sub_write_cmos_sensor(0xb4,0x20);
				SP0838_sub_write_cmos_sensor(0xb5,0x3a);
				SP0838_sub_write_cmos_sensor(0xb6,0x5e);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x42);
				SP0838_sub_write_cmos_sensor(0xbf,0x42);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x41);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x70);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x70);
				SP0838_sub_write_cmos_sensor(0xcb,0xc );
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x0f);
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				//	dbg_print(" priview 50Hz normal\r\n");
				SENSORDB(" priview 50Hz normal\r\n");
			}
			else if(sp0838_isBanding== 1)
			{
				//caprure preview normal 24M 60hz 20-8FPS maxgain:0x78				  
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				SP0838_sub_write_cmos_sensor(0x05,0x0 );
				SP0838_sub_write_cmos_sensor(0x06,0x0 );
				SP0838_sub_write_cmos_sensor(0x09,0x1 );
				SP0838_sub_write_cmos_sensor(0x0a,0x76);//0xa0);//Ronlus test 0x76);
				SP0838_sub_write_cmos_sensor(0xf0,0x51);
				SP0838_sub_write_cmos_sensor(0xf1,0x0 );
				SP0838_sub_write_cmos_sensor(0xf2,0x5c);
				SP0838_sub_write_cmos_sensor(0xf5,0x75);
				SP0838_sub_write_cmos_sensor(0xfd,0x01);
				SP0838_sub_write_cmos_sensor(0x00,0xb4);
				SP0838_sub_write_cmos_sensor(0x0f,0x5d);
				SP0838_sub_write_cmos_sensor(0x16,0x5d);
				SP0838_sub_write_cmos_sensor(0x17,0xa4);
				SP0838_sub_write_cmos_sensor(0x18,0xac);
				SP0838_sub_write_cmos_sensor(0x1b,0x5d);
				SP0838_sub_write_cmos_sensor(0x1c,0xac);
				SP0838_sub_write_cmos_sensor(0xb4,0x21);
				SP0838_sub_write_cmos_sensor(0xb5,0x3d);
				SP0838_sub_write_cmos_sensor(0xb6,0x4d);
				SP0838_sub_write_cmos_sensor(0xb9,0x40);
				SP0838_sub_write_cmos_sensor(0xba,0x4f);
				SP0838_sub_write_cmos_sensor(0xbb,0x47);
				SP0838_sub_write_cmos_sensor(0xbc,0x45);
				SP0838_sub_write_cmos_sensor(0xbd,0x43);
				SP0838_sub_write_cmos_sensor(0xbe,0x42);
				SP0838_sub_write_cmos_sensor(0xbf,0x42);
				SP0838_sub_write_cmos_sensor(0xc0,0x42);
				SP0838_sub_write_cmos_sensor(0xc1,0x41);
				SP0838_sub_write_cmos_sensor(0xc2,0x41);
				SP0838_sub_write_cmos_sensor(0xc3,0x41);
				SP0838_sub_write_cmos_sensor(0xc4,0x41);
				SP0838_sub_write_cmos_sensor(0xc5,0x41);
				SP0838_sub_write_cmos_sensor(0xc6,0x41);
				SP0838_sub_write_cmos_sensor(0xca,0x70);
				SP0838_sub_write_cmos_sensor(0xcb,0xf );
				SP0838_sub_write_cmos_sensor(0x14,0x20);
				SP0838_sub_write_cmos_sensor(0x15,0x0f);
				SP0838_sub_write_cmos_sensor(0xfd,0x00);
				//dbg_print(" priview 60Hz normal\r\n");   
				SENSORDB(" priview 60Hz normal\r\n");
			}
		}
	}  
}

/*************************************************************************
* FUNCTION
*	SP0838_sub_Sensor_Init
*
* DESCRIPTION
*	This function apply all of the initial setting to sensor.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
*************************************************************************/
void SP0838_Sub_Sensor_Init(void)
{//modify by sp_yjp,20130114
	SP0838_sub_write_cmos_sensor(0xfd , 0x00);
	SP0838_sub_write_cmos_sensor(0x1b , 0x02);

	SP0838_sub_write_cmos_sensor(0x27 , 0xe8);
	SP0838_sub_write_cmos_sensor(0x28 , 0x0B);
	SP0838_sub_write_cmos_sensor(0x32 , 0x00);
	SP0838_sub_write_cmos_sensor(0x22 , 0xc0);
	SP0838_sub_write_cmos_sensor(0x26 , 0x10);
	SP0838_sub_write_cmos_sensor(0x31 , 0x30);  ///Upside/mirr/Pclk inv/sub
	SP0838_sub_write_cmos_sensor(0x5f , 0x21);  ///Bayer order
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);  ///P1
	SP0838_sub_write_cmos_sensor(0x25 , 0x1a);  ///Awb start
	SP0838_sub_write_cmos_sensor(0x26 , 0xfb);
	SP0838_sub_write_cmos_sensor(0x28 , 0x75);///0x61
	SP0838_sub_write_cmos_sensor(0x29 , 0x4e);///0x49
	SP0838_sub_write_cmos_sensor(0xfd , 0x00);
	SP0838_sub_write_cmos_sensor(0xe7 , 0x03);
	SP0838_sub_write_cmos_sensor(0xe7 , 0x00);
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);
	SP0838_sub_write_cmos_sensor(0x31 , 0x60); ///64
	SP0838_sub_write_cmos_sensor(0x32 , 0x18);
	SP0838_sub_write_cmos_sensor(0x4d , 0xdc);
	SP0838_sub_write_cmos_sensor(0x4e , 0x53);///0x6b
	SP0838_sub_write_cmos_sensor(0x41 , 0x8c);
	SP0838_sub_write_cmos_sensor(0x42 , 0x57);///0x66
	SP0838_sub_write_cmos_sensor(0x55 , 0xff);
	SP0838_sub_write_cmos_sensor(0x56 , 0x00);
	SP0838_sub_write_cmos_sensor(0x59 , 0x82);
	SP0838_sub_write_cmos_sensor(0x5a , 0x00);
	SP0838_sub_write_cmos_sensor(0x5d , 0xff);
	SP0838_sub_write_cmos_sensor(0x5e , 0x6f);
	SP0838_sub_write_cmos_sensor(0x57 , 0xff);
	SP0838_sub_write_cmos_sensor(0x58 , 0x00);
	SP0838_sub_write_cmos_sensor(0x5b , 0xff);
	SP0838_sub_write_cmos_sensor(0x5c , 0xa8);
	SP0838_sub_write_cmos_sensor(0x5f , 0x75);
	SP0838_sub_write_cmos_sensor(0x60 , 0x00);
	SP0838_sub_write_cmos_sensor(0x2d , 0x00);
	SP0838_sub_write_cmos_sensor(0x2e , 0x00);
	SP0838_sub_write_cmos_sensor(0x2f , 0x00);
	SP0838_sub_write_cmos_sensor(0x30 , 0x00);
	SP0838_sub_write_cmos_sensor(0x33 , 0x00);
	SP0838_sub_write_cmos_sensor(0x34 , 0x00);
	SP0838_sub_write_cmos_sensor(0x37 , 0x00);
	SP0838_sub_write_cmos_sensor(0x38 , 0x00);  ///awb end
	SP0838_sub_write_cmos_sensor(0xfd , 0x00);  ///P0
	SP0838_sub_write_cmos_sensor(0x33 , 0x6f);  ///LSC BPC EN
	SP0838_sub_write_cmos_sensor(0x51 , 0x3f);  ///BPC debug start
	SP0838_sub_write_cmos_sensor(0x52 , 0x09);
	SP0838_sub_write_cmos_sensor(0x53 , 0x00);
	SP0838_sub_write_cmos_sensor(0x54 , 0x00);
	SP0838_sub_write_cmos_sensor(0x55 , 0x10);  ///BPC debug end
	SP0838_sub_write_cmos_sensor(0x4f , 0x08);  ///blueedge
	SP0838_sub_write_cmos_sensor(0x50 , 0x08);
	SP0838_sub_write_cmos_sensor(0x57 , 0x10);  ///Raw filter debut start
	SP0838_sub_write_cmos_sensor(0x58 , 0x10);  ///4
	SP0838_sub_write_cmos_sensor(0x59 , 0x10);  ///4
	SP0838_sub_write_cmos_sensor(0x58 , 0x10);  ///4
	SP0838_sub_write_cmos_sensor(0x59 , 0x10);  ///4
	SP0838_sub_write_cmos_sensor(0x56 , 0x70);
	SP0838_sub_write_cmos_sensor(0x5a , 0x02);
	SP0838_sub_write_cmos_sensor(0x5b , 0x02);
	SP0838_sub_write_cmos_sensor(0x5c , 0x20);  ///Raw filter debut end 
	SP0838_sub_write_cmos_sensor(0x65 , 0x03);  ///Sharpness debug start
	SP0838_sub_write_cmos_sensor(0x66 , 0x01);
	SP0838_sub_write_cmos_sensor(0x67 , 0x03);
	SP0838_sub_write_cmos_sensor(0x68 , 0x46);
	SP0838_sub_write_cmos_sensor(0x69 , 0x7f);
	SP0838_sub_write_cmos_sensor(0x6a , 0x01);
	SP0838_sub_write_cmos_sensor(0x6b , 0x04);
	SP0838_sub_write_cmos_sensor(0x6c , 0x01);
	SP0838_sub_write_cmos_sensor(0x6d , 0x03);  ///Edge gain normal
	SP0838_sub_write_cmos_sensor(0x6e , 0x46);  ///Edge gain normal
	SP0838_sub_write_cmos_sensor(0x6f , 0x7f);
	SP0838_sub_write_cmos_sensor(0x70 , 0x01);
	SP0838_sub_write_cmos_sensor(0x71 , 0x05);
	SP0838_sub_write_cmos_sensor(0x72 , 0x01);
	SP0838_sub_write_cmos_sensor(0x73 , 0x03);
	SP0838_sub_write_cmos_sensor(0x74 , 0x46);
	SP0838_sub_write_cmos_sensor(0x75 , 0x7f);
	SP0838_sub_write_cmos_sensor(0x76 , 0x01);  ///Sharpness debug end
	SP0838_sub_write_cmos_sensor(0xcb , 0x00);///07  ///HEQ&Saturation debug start 
	SP0838_sub_write_cmos_sensor(0xcc , 0x04);
	SP0838_sub_write_cmos_sensor(0xce , 0xff);
	SP0838_sub_write_cmos_sensor(0xcf , 0x10);
	SP0838_sub_write_cmos_sensor(0xd0 , 0x20);
	SP0838_sub_write_cmos_sensor(0xd1 , 0x00);
	SP0838_sub_write_cmos_sensor(0xd2 , 0x1c);
	SP0838_sub_write_cmos_sensor(0xd3 , 0x16);
	SP0838_sub_write_cmos_sensor(0xd4 , 0x00);
	SP0838_sub_write_cmos_sensor(0xd6 , 0x1c);
	SP0838_sub_write_cmos_sensor(0xd7 , 0x16);
	SP0838_sub_write_cmos_sensor(0xdd , 0x70);  ///Contrast
	SP0838_sub_write_cmos_sensor(0xde , 0x90);  ///HEQ&Saturation debug end
#if 0	//old CCM param,20130114
	SP0838_sub_write_cmos_sensor(0x7f , 0xd7);    ///Color Correction start 
	SP0838_sub_write_cmos_sensor(0x80 , 0xbc);
	SP0838_sub_write_cmos_sensor(0x81 , 0xed);
	SP0838_sub_write_cmos_sensor(0x82 , 0xd7);
	SP0838_sub_write_cmos_sensor(0x83 , 0xd4);
	SP0838_sub_write_cmos_sensor(0x84 , 0xd6);
	SP0838_sub_write_cmos_sensor(0x85 , 0xff);
	SP0838_sub_write_cmos_sensor(0x86 , 0x89);///55 
	SP0838_sub_write_cmos_sensor(0x87 , 0xf8);///2c 
	SP0838_sub_write_cmos_sensor(0x88 , 0x3c);
	SP0838_sub_write_cmos_sensor(0x89 , 0x33);
	SP0838_sub_write_cmos_sensor(0x8a , 0x0f);
#endif

#if 1   //add CCM param by sp_yjp,20130114
	SP0838_sub_write_cmos_sensor(0x7f , 0xef);    ///Color Correction start 
	SP0838_sub_write_cmos_sensor(0x80 , 0xa4);
	SP0838_sub_write_cmos_sensor(0x81 , 0xed);
	SP0838_sub_write_cmos_sensor(0x82 , 0xd7);
	SP0838_sub_write_cmos_sensor(0x83 , 0xba);
	SP0838_sub_write_cmos_sensor(0x84 , 0xef);
	SP0838_sub_write_cmos_sensor(0x85 , 0xff);
	SP0838_sub_write_cmos_sensor(0x86 , 0x89);///55 
	SP0838_sub_write_cmos_sensor(0x87 , 0xf8);///2c 
	SP0838_sub_write_cmos_sensor(0x88 , 0x3c);
	SP0838_sub_write_cmos_sensor(0x89 , 0x33);
	SP0838_sub_write_cmos_sensor(0x8a , 0x0f);
#endif




	///0x7f , 0xef
	///0x80 , 0xa4
	///0x81 , 0xed
	///0x82 , 0xd7
	///0x83 , 0xba
	///0x84 , 0xef
	///0x83 , 0xba
	///0x84 , 0xef
	///0x85 , 0xff
	///0x86 , 0x89
	///0x87 , 0xf8
	///0x88 , 0x3c
	///0x89 , 0x33
	///0x8a , 0x0f




#if 0	//old param,20130114
	SP0838_sub_write_cmos_sensor(0x8b , 0x0 );  ///gamma start
	SP0838_sub_write_cmos_sensor(0x8c , 0x1a);
	SP0838_sub_write_cmos_sensor(0x8d , 0x29);
	SP0838_sub_write_cmos_sensor(0x8e , 0x41);
	SP0838_sub_write_cmos_sensor(0x8f , 0x62);
	SP0838_sub_write_cmos_sensor(0x90 , 0x7c);
	SP0838_sub_write_cmos_sensor(0x91 , 0x90);
	SP0838_sub_write_cmos_sensor(0x92 , 0xa2);
	SP0838_sub_write_cmos_sensor(0x93 , 0xaf);
	SP0838_sub_write_cmos_sensor(0x94 , 0xbc);
	SP0838_sub_write_cmos_sensor(0x95 , 0xc5);
	SP0838_sub_write_cmos_sensor(0x96 , 0xcd);
	SP0838_sub_write_cmos_sensor(0x97 , 0xd5);
	SP0838_sub_write_cmos_sensor(0x98 , 0xdd);
	SP0838_sub_write_cmos_sensor(0x99 , 0xe5);
	SP0838_sub_write_cmos_sensor(0x9a , 0xed);
	SP0838_sub_write_cmos_sensor(0x9b , 0xf5);
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);  ///P1
	SP0838_sub_write_cmos_sensor(0x8d , 0xfd);
	SP0838_sub_write_cmos_sensor(0x8e , 0xff);  ///gamma end
#endif

#if 1	//add by sp_yjp,20140114
	SP0838_sub_write_cmos_sensor(0xfd , 0x00);
	SP0838_sub_write_cmos_sensor(0x8b , 0x00);  ///gamma start
	SP0838_sub_write_cmos_sensor(0x8c , 0x11);
	SP0838_sub_write_cmos_sensor(0x8d , 0x1e);
	SP0838_sub_write_cmos_sensor(0x8e , 0x33);
	SP0838_sub_write_cmos_sensor(0x8f , 0x58);
	SP0838_sub_write_cmos_sensor(0x90 , 0x75);
	SP0838_sub_write_cmos_sensor(0x91 , 0x8d);
	SP0838_sub_write_cmos_sensor(0x92 , 0xa2);
	SP0838_sub_write_cmos_sensor(0x93 , 0xaf);
	SP0838_sub_write_cmos_sensor(0x94 , 0xba);
	SP0838_sub_write_cmos_sensor(0x95 , 0xc4);
	SP0838_sub_write_cmos_sensor(0x96 , 0xce);
	SP0838_sub_write_cmos_sensor(0x97 , 0xd6);
	SP0838_sub_write_cmos_sensor(0x98 , 0xdd);
	SP0838_sub_write_cmos_sensor(0x99 , 0xe4);
	SP0838_sub_write_cmos_sensor(0x9a , 0xea);
	SP0838_sub_write_cmos_sensor(0x9b , 0xf1);
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);  ///P1
	SP0838_sub_write_cmos_sensor(0x8d , 0xf8);
	SP0838_sub_write_cmos_sensor(0x8e , 0xff);  ///gamma end
#endif



	SP0838_sub_write_cmos_sensor(0xfd , 0x00);  ///P0
	SP0838_sub_write_cmos_sensor(0xca , 0xcf);
	SP0838_sub_write_cmos_sensor(0xd8 , 0x48);///0x65  ///UV outdoor
	SP0838_sub_write_cmos_sensor(0xd9 , 0x48);///0x65  ///UV indoor 
	SP0838_sub_write_cmos_sensor(0xda , 0x48);///0x65  ///UV dummy
	SP0838_sub_write_cmos_sensor(0xdb , 0x48);///0x50  ///UV lowlight
	SP0838_sub_write_cmos_sensor(0xb9 , 0x00);  ///Ygamma start
	SP0838_sub_write_cmos_sensor(0xba , 0x04);
	SP0838_sub_write_cmos_sensor(0xbb , 0x08);
	SP0838_sub_write_cmos_sensor(0xbc , 0x10);
	SP0838_sub_write_cmos_sensor(0xbd , 0x20);
	SP0838_sub_write_cmos_sensor(0xbe , 0x30);
	SP0838_sub_write_cmos_sensor(0xbf , 0x40);
	SP0838_sub_write_cmos_sensor(0xc0 , 0x50);
	SP0838_sub_write_cmos_sensor(0xc1 , 0x60);
	SP0838_sub_write_cmos_sensor(0xc2 , 0x70);
	SP0838_sub_write_cmos_sensor(0xc3 , 0x80);
	SP0838_sub_write_cmos_sensor(0xc4 , 0x90);
	SP0838_sub_write_cmos_sensor(0xc5 , 0xA0);
	SP0838_sub_write_cmos_sensor(0xc6 , 0xB0);
	SP0838_sub_write_cmos_sensor(0xc7 , 0xC0);
	SP0838_sub_write_cmos_sensor(0xc8 , 0xD0);
	SP0838_sub_write_cmos_sensor(0xc9 , 0xE0);
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);  ///P1
	SP0838_sub_write_cmos_sensor(0xc9 , 0xE0);
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);  ///P1
	SP0838_sub_write_cmos_sensor(0x89 , 0xf0);
	SP0838_sub_write_cmos_sensor(0x8a , 0xff);  ///Ygamma end
	SP0838_sub_write_cmos_sensor(0xfd , 0x00);  ///P0
	SP0838_sub_write_cmos_sensor(0xe8 , 0x30);  ///AEdebug start
	SP0838_sub_write_cmos_sensor(0xe9 , 0x30);
	SP0838_sub_write_cmos_sensor(0xea , 0x40);  ///Alc Window sel
	SP0838_sub_write_cmos_sensor(0xf4 , 0x1b);  ///outdoor mode sel
	SP0838_sub_write_cmos_sensor(0xf5 , 0x80);
	SP0838_sub_write_cmos_sensor(0xf7 , 0x78);  ///AE target 
	SP0838_sub_write_cmos_sensor(0xf8 , 0x63);
	SP0838_sub_write_cmos_sensor(0xf9 , 0x68);  ///AE target 
	SP0838_sub_write_cmos_sensor(0xfa , 0x53);
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);  ///P1
	SP0838_sub_write_cmos_sensor(0x09 , 0x31);  ///AE Step 3.0
	SP0838_sub_write_cmos_sensor(0x0a , 0x85);
	SP0838_sub_write_cmos_sensor(0x0b , 0x23);  ///AE Step 3.0
	SP0838_sub_write_cmos_sensor(0x14 , 0x20);
	SP0838_sub_write_cmos_sensor(0x15 , 0x0f);


#if 1//caprure preview daylight 24M 50hz 20-8FPS maxgain:0x70   
	SP0838_sub_write_cmos_sensor(0xfd , 0x00);
	SP0838_sub_write_cmos_sensor(0x05 , 0x0 );
	SP0838_sub_write_cmos_sensor(0x06 , 0x0 );
	SP0838_sub_write_cmos_sensor(0x09 , 0x1 );
	SP0838_sub_write_cmos_sensor(0x0a , 0x76);
	SP0838_sub_write_cmos_sensor(0xf0 , 0x62);
	SP0838_sub_write_cmos_sensor(0xf1 , 0x0 );
	SP0838_sub_write_cmos_sensor(0xf2 , 0x5f);
	SP0838_sub_write_cmos_sensor(0xf5 , 0x78);
	SP0838_sub_write_cmos_sensor(0xfd , 0x01);
	SP0838_sub_write_cmos_sensor(0x00 , 0xb2);
	SP0838_sub_write_cmos_sensor(0x0f , 0x60);
	SP0838_sub_write_cmos_sensor(0x16 , 0x60);
	SP0838_sub_write_cmos_sensor(0x17 , 0xa2);
	SP0838_sub_write_cmos_sensor(0x18 , 0xaa);
	SP0838_sub_write_cmos_sensor(0x1b , 0x60);
	SP0838_sub_write_cmos_sensor(0x1c , 0xaa);
	SP0838_sub_write_cmos_sensor(0xb4 , 0x20);
	SP0838_sub_write_cmos_sensor(0xb5 , 0x3a);
	SP0838_sub_write_cmos_sensor(0xb6 , 0x5e);
	SP0838_sub_write_cmos_sensor(0xb9 , 0x40);
	SP0838_sub_write_cmos_sensor(0xba , 0x4f);
	SP0838_sub_write_cmos_sensor(0xbb , 0x47);
	SP0838_sub_write_cmos_sensor(0xbc , 0x45);
	SP0838_sub_write_cmos_sensor(0xbd , 0x43);
	SP0838_sub_write_cmos_sensor(0xbe , 0x42);
	SP0838_sub_write_cmos_sensor(0xbf , 0x42);
	SP0838_sub_write_cmos_sensor(0xc0 , 0x42);
	SP0838_sub_write_cmos_sensor(0xc1 , 0x41);
	SP0838_sub_write_cmos_sensor(0xc2 , 0x41);
	SP0838_sub_write_cmos_sensor(0xc3 , 0x41);
	SP0838_sub_write_cmos_sensor(0xc4 , 0x41);
	SP0838_sub_write_cmos_sensor(0xc5 , 0x70);
	SP0838_sub_write_cmos_sensor(0xc6 , 0x41);

	SP0838_sub_write_cmos_sensor(0xc5 , 0x70);
	SP0838_sub_write_cmos_sensor(0xc6 , 0x41);
	SP0838_sub_write_cmos_sensor(0xca , 0x70);
	SP0838_sub_write_cmos_sensor(0xcb , 0xc );
	SP0838_sub_write_cmos_sensor(0xfd , 0x00);
#endif

	SP0838_sub_write_cmos_sensor(0x32 , 0x15);  ///Auto_mode set
	SP0838_sub_write_cmos_sensor(0x34 , 0x66);  ///Isp_mode set
	SP0838_sub_write_cmos_sensor(0x35 , 0x00);  ///out format

	///lens shading
	SP0838_sub_write_cmos_sensor(0x9c , 0x00);
	SP0838_sub_write_cmos_sensor(0x9d , 0x05);
	SP0838_sub_write_cmos_sensor(0x9e , 0x16);
	SP0838_sub_write_cmos_sensor(0x9f , 0x30);
	SP0838_sub_write_cmos_sensor(0xa0 , 0x59);
	SP0838_sub_write_cmos_sensor(0xa1 , 0x8d);///93
	SP0838_sub_write_cmos_sensor(0xa2 , 0xdf);
	SP0838_sub_write_cmos_sensor(0xa3 , 0xdf);
	SP0838_sub_write_cmos_sensor(0xa4 , 0xdf);
	SP0838_sub_write_cmos_sensor(0xa5 , 0x00);
	SP0838_sub_write_cmos_sensor(0xa6 , 0x07);
	SP0838_sub_write_cmos_sensor(0xa7 , 0x16);
	SP0838_sub_write_cmos_sensor(0xa8 , 0x30);
	SP0838_sub_write_cmos_sensor(0xa9 , 0x56);
	SP0838_sub_write_cmos_sensor(0xaa , 0x83);
	SP0838_sub_write_cmos_sensor(0xab , 0xc8);
	SP0838_sub_write_cmos_sensor(0xac , 0xdf);
	SP0838_sub_write_cmos_sensor(0xad , 0xdf);
	SP0838_sub_write_cmos_sensor(0xae , 0x00);
	SP0838_sub_write_cmos_sensor(0xaf , 0x0c);
	SP0838_sub_write_cmos_sensor(0xb0 , 0x1a);
	SP0838_sub_write_cmos_sensor(0xb1 , 0x32);///30
	SP0838_sub_write_cmos_sensor(0xb2 , 0x52);
	SP0838_sub_write_cmos_sensor(0xb3 , 0x7D);///88
	SP0838_sub_write_cmos_sensor(0xb4 , 0xd4);
	SP0838_sub_write_cmos_sensor(0xb5 , 0xdf);
	SP0838_sub_write_cmos_sensor(0xb6 , 0xdf);
	SP0838_sub_write_cmos_sensor(0xb7 , 0xa0);
	SP0838_sub_write_cmos_sensor(0xb8 , 0x78);
	
}


UINT32 SP0838_subGetSensorID(UINT32 *sensorID)
{
	kal_uint16 sensor_id=0;
	int retry=3;

	SENSORDB("SP0838_subGetSensorID \n");	

	// check if sensor ID correct
	do {

		SP0838_sub_write_cmos_sensor(0xfd,0x00);
		sensor_id=SP0838_sub_read_cmos_sensor(0x02);
		if (sensor_id == SP0838_SUB_SENSOR_ID) {
			break; 
		}
		SENSORDB("Read Sensor ID Fail = 0x%x\n", sensor_id); 

		retry--; 
	}while (retry > 0); 

	if (sensor_id != SP0838_SUB_SENSOR_ID) {
		return ERROR_SENSOR_CONNECT_FAIL;
	}

	*sensorID = sensor_id;
	RETAILMSG(1, (TEXT("Sensor Read ID OK \r\n")));

	return ERROR_NONE;
}

/*************************************************************************
 * FUNCTION
 *	SP0838_subOpen
 *
 * DESCRIPTION
 *	This function initialize the registers of CMOS sensor
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 SP0838_subOpen(void)
{
	kal_uint16 sensor_id=0;
	int retry = 3; 

	SENSORDB("SP0838_subOpen \n");

	do {

		SP0838_sub_write_cmos_sensor(0xfd,0x00);
		sensor_id=SP0838_sub_read_cmos_sensor(0x02);
		if (sensor_id == SP0838_SUB_SENSOR_ID) {
			break; 
		}
		SENSORDB("Read Sensor ID Fail = 0x%x\n", sensor_id); 

		retry--; 
	}while (retry > 0); 

	if (sensor_id != SP0838_SUB_SENSOR_ID) 
	{
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	SENSORDB("SP0838_sub Sensor id read OK, ID = %x\n", sensor_id);

	Sleep(10);
	//SP0838_sub_Sensor_Init();		
	SP0838_Sub_Sensor_Init();
	SENSORDB("SP0838_subOpen end \n");
	return ERROR_NONE;
} /* SP0838_subOpen */


/*************************************************************************
 * FUNCTION
 *	SP0838_subClose
 *
 * DESCRIPTION
 *	This function is to turn off sensor module power.
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 SP0838_subClose(void)
{
     SENSORDB("SP0838_subClose\n");
    return ERROR_NONE;
} /* SP0838_subClose */


/*************************************************************************
 * FUNCTION
 * SP0838_subPreview
 *
 * DESCRIPTION
 *	This function start the sensor preview.
 *
 * PARAMETERS
 *	*image_window : address pointer of pixel numbers in one period of HSYNC
 *  *sensor_config_data : address pointer of line numbers in one period of VSYNC
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 SP0838_subPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
        MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)

{
	kal_uint32 iTemp;
	kal_uint16 iStartX = 0, iStartY = 1;

	if(sensor_config_data->SensorOperationMode == MSDK_SENSOR_OPERATION_MODE_VIDEO)		// MPEG4 Encode Mode
	{
		RETAILMSG(1, (TEXT("Camera Video preview\r\n")));
		SP0838_sub_MPEG4_encode_mode = KAL_TRUE;
	}
	else
	{
		RETAILMSG(1, (TEXT("Camera preview\r\n")));
		SP0838_sub_MPEG4_encode_mode = KAL_FALSE;
	}

	image_window->GrabStartX= IMAGE_SENSOR_VGA_GRAB_PIXELS;
	image_window->GrabStartY= IMAGE_SENSOR_VGA_GRAB_LINES;
	image_window->ExposureWindowWidth = IMAGE_SENSOR_PV_WIDTH;
	image_window->ExposureWindowHeight =IMAGE_SENSOR_PV_HEIGHT;

	// copy sensor_config_data
	memcpy(&SP0838_subSensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
} /* SP0838_subPreview */


/*************************************************************************
 * FUNCTION
 *	SP0838_subCapture
 *
 * DESCRIPTION
 *	This function setup the CMOS sensor in capture MY_OUTPUT mode
 *
 * PARAMETERS
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 SP0838_subCapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
        MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)

{
	SP0838_sub_MODE_CAPTURE=KAL_TRUE;

	image_window->GrabStartX = IMAGE_SENSOR_VGA_GRAB_PIXELS;
	image_window->GrabStartY = IMAGE_SENSOR_VGA_GRAB_LINES;
	image_window->ExposureWindowWidth= IMAGE_SENSOR_FULL_WIDTH;
	image_window->ExposureWindowHeight = IMAGE_SENSOR_FULL_HEIGHT;

	// copy sensor_config_data
	memcpy(&SP0838_subSensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
} /* SP0838_sub_Capture() */



UINT32 SP0838_subGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
	pSensorResolution->SensorFullWidth=IMAGE_SENSOR_FULL_WIDTH;
	pSensorResolution->SensorFullHeight=IMAGE_SENSOR_FULL_HEIGHT;
	pSensorResolution->SensorPreviewWidth=IMAGE_SENSOR_PV_WIDTH;
	pSensorResolution->SensorPreviewHeight=IMAGE_SENSOR_PV_HEIGHT;
	return ERROR_NONE;
} /* SP0838_subGetResolution() */


UINT32 SP0838_subGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
        MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
        MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	pSensorInfo->SensorPreviewResolutionX=IMAGE_SENSOR_PV_WIDTH;
	pSensorInfo->SensorPreviewResolutionY=IMAGE_SENSOR_PV_HEIGHT;
	pSensorInfo->SensorFullResolutionX=IMAGE_SENSOR_FULL_WIDTH;
	pSensorInfo->SensorFullResolutionY=IMAGE_SENSOR_FULL_WIDTH;

	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=10;
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;
	pSensorInfo->SensorResetDelayCount=1;
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
	pSensorInfo->SensorInterruptDelayLines = 1;
	pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_PARALLEL;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxWidth=CAM_SIZE_1M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxHeight=CAM_SIZE_1M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxWidth=CAM_SIZE_1M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxHeight=CAM_SIZE_1M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].BinningEnable=FALSE;
	pSensorInfo->CaptureDelayFrame = 1;
	pSensorInfo->PreviewDelayFrame = 0;
	pSensorInfo->VideoDelayFrame = 4;
	pSensorInfo->SensorMasterClockSwitch = 0;
	pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;

	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_VGA_GRAB_PIXELS;
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_VGA_GRAB_LINES;
			break;

		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount= 3;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=2;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_VGA_GRAB_PIXELS;
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_VGA_GRAB_LINES;
			break;
		default:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount= 3;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=2;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_VGA_GRAB_PIXELS;
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_VGA_GRAB_LINES;
			break;
	}
	SP0838_subPixelClockDivider=pSensorInfo->SensorPixelClockCount;
	memcpy(pSensorConfigData, &SP0838_subSensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
} /* SP0838_subGetInfo() */


UINT32 SP0838_subControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
        MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			SP0838_subPreview(pImageWindow, pSensorConfigData);
			break;

		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			SP0838_subCapture(pImageWindow, pSensorConfigData);
			break;
	}


	return TRUE;
}	/* SP0838_subControl() */

BOOL SP0838_sub_set_param_wb(UINT16 para)
{
	switch (para)
	{
		case AWB_MODE_OFF:
			SP0838_sub_write_cmos_sensor(0xfd,0x00);				   
			SP0838_sub_write_cmos_sensor(0x32,0x05);	  
			break;
		
		case AWB_MODE_AUTO:
			
			SP0838_sub_write_cmos_sensor(0xfd,0x01);                                                          
			SP0838_sub_write_cmos_sensor(0x28,0x75);		                                                       
			SP0838_sub_write_cmos_sensor(0x29,0x4e);
			SP0838_sub_write_cmos_sensor(0xfd,0x00);  // AUTO 3000K~7000K                                     
			SP0838_sub_write_cmos_sensor(0x32,0x15); 	
			break;
		
		case AWB_MODE_CLOUDY_DAYLIGHT: //cloudy
			// sp0838_reg_WB_auto   
			SP0838_sub_write_cmos_sensor(0xfd,0x00);   //7000K                                     
			SP0838_sub_write_cmos_sensor(0x32,0x05);                                                          
			SP0838_sub_write_cmos_sensor(0xfd,0x01);                                                          
			SP0838_sub_write_cmos_sensor(0x28,0x71);		                                                       
			SP0838_sub_write_cmos_sensor(0x29,0x41);		                                                       
			SP0838_sub_write_cmos_sensor(0xfd,0x00);                                             
			break;
		
		case AWB_MODE_DAYLIGHT: //sunny
			// sp0838_reg_WB_auto 
			SP0838_sub_write_cmos_sensor(0xfd,0x00);  //6500K                                     
			SP0838_sub_write_cmos_sensor(0x32,0x05);                                                          
			SP0838_sub_write_cmos_sensor(0xfd,0x01);                                                          
			SP0838_sub_write_cmos_sensor(0x28,0x6b);		                                                       
			SP0838_sub_write_cmos_sensor(0x29,0x48);		                                                       
			SP0838_sub_write_cmos_sensor(0xfd,0x00);                                                         
			break;
		   
		case AWB_MODE_INCANDESCENT: //office
		
			// sp0838_reg_WB_auto 
			SP0838_sub_write_cmos_sensor(0xfd,0x00);  //2800K~3000K 									
			SP0838_sub_write_cmos_sensor(0x32,0x05);														  
			SP0838_sub_write_cmos_sensor(0xfd,0x01);														  
			SP0838_sub_write_cmos_sensor(0x28,0x41);															   
			SP0838_sub_write_cmos_sensor(0x29,0x71);															   
			SP0838_sub_write_cmos_sensor(0xfd,0x00);														  
			break;
		
		case AWB_MODE_TUNGSTEN: //home
			// sp0838_reg_WB_auto 
			SP0838_sub_write_cmos_sensor(0xfd,0x00);  //4000K                                   
			SP0838_sub_write_cmos_sensor(0x32,0x05);                                                          
			SP0838_sub_write_cmos_sensor(0xfd,0x01);                                                          
			SP0838_sub_write_cmos_sensor(0x28,0x57);		                                                       
			SP0838_sub_write_cmos_sensor(0x29,0x66);		                                                       
			SP0838_sub_write_cmos_sensor(0xfd,0x00);                                                         
			break;
		
		case AWB_MODE_FLUORESCENT:
			//sp0838_reg_WB_auto  
			SP0838_sub_write_cmos_sensor(0xfd,0x00);  //4200K~5000K                                     
			SP0838_sub_write_cmos_sensor(0x32,0x05);                                                          
			SP0838_sub_write_cmos_sensor(0xfd,0x01);                                                          
			SP0838_sub_write_cmos_sensor(0x28,0x5a);		                                                       
			SP0838_sub_write_cmos_sensor(0x29,0x62);		                                                       
			SP0838_sub_write_cmos_sensor(0xfd,0x00);                                                         
			break;
		
		default:
			return FALSE;
	}

	return TRUE;
} /* SP0838_sub_set_param_wb */


BOOL SP0838_sub_set_param_effect(UINT16 para)
{
	kal_uint32  ret = KAL_TRUE;

	switch (para)
	{
		case MEFFECT_OFF:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0x62, 0x00);
			SP0838_sub_write_cmos_sensor(0x63, 0x80);
			SP0838_sub_write_cmos_sensor(0x64, 0x80);
			break;
		
		case MEFFECT_SEPIA:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0x62, 0x20);
			SP0838_sub_write_cmos_sensor(0x63, 0xc0);
			SP0838_sub_write_cmos_sensor(0x64, 0x20);
			break;
		
		case MEFFECT_NEGATIVE:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0x62, 0x08);
			SP0838_sub_write_cmos_sensor(0x63, 0x80);
			SP0838_sub_write_cmos_sensor(0x64, 0x80);
			break;
		  
		case MEFFECT_SEPIAGREEN:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0x62, 0x20);
			SP0838_sub_write_cmos_sensor(0x63, 0x20);
			SP0838_sub_write_cmos_sensor(0x64, 0x20);
			break;
		
		case MEFFECT_SEPIABLUE:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0x62, 0x20);
			SP0838_sub_write_cmos_sensor(0x63, 0x20);
			SP0838_sub_write_cmos_sensor(0x64, 0xf0);
			break;

		case MEFFECT_MONO:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0x62, 0x40);
			SP0838_sub_write_cmos_sensor(0x63, 0x80);
			SP0838_sub_write_cmos_sensor(0x64, 0x80);
			break;

		default:
			ret = FALSE;
	}

	return ret;

} /* SP0838_sub_set_param_effect */


BOOL SP0838_sub_set_param_banding(UINT16 para)
{
	switch (para)
	{
		case AE_FLICKER_MODE_50HZ:
			sp0838_isBanding = 0;
			break;

		case AE_FLICKER_MODE_60HZ:
			sp0838_isBanding = 1;
			break;

		default:
			return FALSE;
	}

	return TRUE;
} /* SP0838_sub_set_param_banding */


BOOL SP0838_sub_set_param_exposure(UINT16 para)
{
	kal_uint8 value_luma, value_Y;
	value_luma = (SP0838_sub_NIGHT_MODE?0x2b:0x00);
	value_Y = (SP0838_sub_NIGHT_MODE?0x68:0x50);

	switch (para)
	{
		case AE_EV_COMP_n13:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0xc0);
			break;
		
		case AE_EV_COMP_n10:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0xd0);
			break;
		
		case AE_EV_COMP_n07:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0xe0);
			break;
		
		case AE_EV_COMP_n03:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0xf0);
			break;				
		
		case AE_EV_COMP_00:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0x00);//0xfa before
			break;

		case AE_EV_COMP_03:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0x10);
			break;
		
		case AE_EV_COMP_07:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0x20);
			break;
		
		case AE_EV_COMP_10:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0x20);
			break;
		
		case AE_EV_COMP_13:
			SP0838_sub_write_cmos_sensor(0xfd, 0x00);
			SP0838_sub_write_cmos_sensor(0xdc, 0x40);
			break;

		default:
			return FALSE;
	}

	return TRUE;
} /* SP0838_sub_set_param_exposure */


UINT32 SP0838_subYUVSensorSetting(FEATURE_ID iCmd, UINT16 iPara)
{
	switch (iCmd) {
		case FID_AWB_MODE:
			SP0838_sub_set_param_wb(iPara);
			break;

		case FID_COLOR_EFFECT:
			SP0838_sub_set_param_effect(iPara);
			break;

		case FID_AE_EV:
			SP0838_sub_set_param_exposure(iPara);
			break;

		case FID_AE_FLICKER:
			SP0838_sub_set_param_banding(iPara);
			break;

		case FID_SCENE_MODE:
			SP0838_subNightMode(iPara);
			break;

		default:
			break;  
	}
	return TRUE;
} /* SP0838_subYUVSensorSetting */


UINT32 SP0838_subFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
        UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
	UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	UINT32 SP0838_subSensorRegNumber;
	UINT32 i;
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

	RETAILMSG(1, (_T("gaiyang SP0838_subFeatureControl FeatureId=%d\r\n"), FeatureId));

	switch (FeatureId)
	{
		case SENSOR_FEATURE_GET_RESOLUTION:
			*pFeatureReturnPara16++=IMAGE_SENSOR_FULL_WIDTH;
			*pFeatureReturnPara16=IMAGE_SENSOR_FULL_HEIGHT;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_GET_PERIOD:
			*pFeatureReturnPara16++=(VGA_PERIOD_PIXEL_NUMS)+SP0838_sub_dummy_pixels;
			*pFeatureReturnPara16=(VGA_PERIOD_LINE_NUMS)+SP0838_sub_dummy_lines;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			*pFeatureReturnPara32 = SP0838_sub_g_fPV_PCLK;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_SET_ESHUTTER:
			break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
			// SP0838_subNightMode((BOOL) *pFeatureData16);
			break;
		case SENSOR_FEATURE_SET_GAIN:
		case SENSOR_FEATURE_SET_FLASHLIGHT:
			break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
			SP0838_sub_isp_master_clock=*pFeatureData32;
			break;
		case SENSOR_FEATURE_SET_REGISTER:
			SP0838_sub_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
			break;
		case SENSOR_FEATURE_GET_REGISTER:
			pSensorRegData->RegData = SP0838_sub_read_cmos_sensor(pSensorRegData->RegAddr);
			break;
		case SENSOR_FEATURE_GET_CONFIG_PARA:
			memcpy(pSensorConfigData, &SP0838_subSensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
			*pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
			break;
		case SENSOR_FEATURE_SET_CCT_REGISTER:
		case SENSOR_FEATURE_GET_CCT_REGISTER:
		case SENSOR_FEATURE_SET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
		case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
		case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
		case SENSOR_FEATURE_GET_GROUP_COUNT:
		case SENSOR_FEATURE_GET_GROUP_INFO:
		case SENSOR_FEATURE_GET_ITEM_INFO:
		case SENSOR_FEATURE_SET_ITEM_INFO:
		case SENSOR_FEATURE_GET_ENG_INFO:
			break;
		case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
			// get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
			// if EEPROM does not exist in camera module.
			*pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_SET_YUV_CMD:
			SP0838_subYUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
			break;
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
			SP0838_subGetSensorID(pFeatureData32);
			break;
		default:
			break;
	}
	return ERROR_NONE;
}	/* SP0838_subFeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncSP0838_subYUV=
{
	SP0838_subOpen,
	SP0838_subGetInfo,
	SP0838_subGetResolution,
	SP0838_subFeatureControl,
	SP0838_subControl,
	SP0838_subClose
};


UINT32 SP0838_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncSP0838_subYUV;

	return ERROR_NONE;
} /* SensorInit() */
