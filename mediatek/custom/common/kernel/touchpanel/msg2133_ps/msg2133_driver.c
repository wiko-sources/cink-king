/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
* applicable license agreements with MediaTek Inc.
*/


/* edit by Magnum 2012-6-1 
   some advices below, if you can not understand this driver code easily .
   1. refer to some other drivers code ,such as cypress.
   2. read the code separately according to the macro(hong) such as  TP_FIRMWARE_UPDATE and 
      CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP
   3. if you want a ctp working ,you can delete the macros which do other things.
      you should just power on and  enable the i2c device ,the most important it that 
      set the right slave address.

   4. you can upgrade tp firmware both apk and mstart_tpupgrade .
      CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP make that mstar_tpupgrade useful,
      TP_FIRMWARE_UPDATE enable that apk upgrade firmware.
      CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP depends on TP_FIRMWARE_UPDATE.
*/

/*  edit by Magnum 2012-8-11
    Mstar CTP can simulate Promxity Sensor. when you in a call ,the code will send a special register to msg2133 ic,
    and ic will go into another state that can change state when your face near the screen.
    if you want to add the func,you must do two things below:
    1: you shoule make sure the mobile load the msg2133 tp driver.
    2: just value CUSTOM_KERNEL_ALSPS = msg2133_alsps in ProjectConfig.
    you can  achieve it. 
*/

#include <linux/i2c.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include "tpd.h"
#include <cust_eint.h>
#include <linux/rtpm_prio.h>
#include <linux/wakelock.h>
#include <asm/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include "tpd_custom_msg2133.h"
#include "cust_gpio_usage.h"

#ifdef MT6575
#include <mach/mt_boot.h>
#include <mach/mt_pm_ldo.h>
#endif

#ifdef MT6577
#include <mach/mt_pm_ldo.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif 

//#define CTP_SIMULATE_PS
#ifdef CTP_SIMULATE_PS  //edit by Magnum 2012-7-27
#include <linux/sensors_io.h>
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/hwmsen_helper.h>
char ps_mode = -1;   // 1: turn on , 0: turn off , -1:default
char ps_data = 1;   // 1: far,      0: close    , 1 :default
char ps_data_pre = 1;   // 1: far,      0: close    , 1 :default
static int tpd_load_status_ok= -1 ; // 1: probe ok , other: probe other ctp driver.
extern void msg2133_ps_event_handler(void);
extern bool power_pressed;

enum
{
	DISABLE_CTP_PS,
	ENABLE_CTP_PS,
};
#endif
 
extern struct tpd_device *tpd;
 
static struct i2c_client *i2c_client = NULL;
static struct task_struct *mythread = NULL;
 
static DECLARE_WAIT_QUEUE_HEAD(waiter);
 
static struct early_suspend early_suspend;
extern struct tpd_device *tpd;

#ifdef CONFIG_HAS_EARLYSUSPEND
static void tpd_early_suspend(struct early_suspend *handler);
static void tpd_late_resume(struct early_suspend *handler);
#endif 
 
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
									  kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
									  kal_bool auto_umask);
void msg2133_tpd_eint_interrupt_handler(void);
static int __devinit tpd_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int tpd_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
static int __devexit tpd_remove(struct i2c_client *client);
static int touch_event_handler(void *unused);
static int tpd_initialize(struct i2c_client * client);
static void tpd_reset(void);
static void powerkey_call_reset(void);

static int ctp_suspend = 1;  //edit by Magnum 2012-10-23 resume depend on suspend .please attention it. 
static int tpd_flag = 0;
#define TP_FIRMWARE_UPDATE
#define TPD_OK 0
#define MSG_TPIO_RESET      GPIO_CTP_RST_PIN
//#define MSG_TPIO_WAKEUP   GPIO_CTP_WAKE_PIN		//Power Down
//#define MSG_TPIO_TPEN       GPIO152		//Power enable
#define MSG_TPIO_EINT     GPIO_CTP_EINT_PIN

static const int TPD_KEYSFACTORY[TPD_KEY_COUNT] =  TPD_FACTORYKEYS;
static int msg2133_ts_get_fw_version(void);
#ifdef TPD_HAVE_BUTTON 
static int tpd_keys_local[TPD_KEY_COUNT] = TPD_KEYS;
static int tpd_keys_dim_local[TPD_KEY_COUNT][4] = TPD_KEYS_DIM;
extern void tpd_button(unsigned int x, unsigned int y, unsigned int down);

#endif
				 
#define U8 unsigned char
#define S8 signed char
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned int
#define S32 signed int
#define TOUCH_ADDR_MSG21XX   0x26//0x4C

//firmware
#define FW_ADDR_MSG21XX      0x62  	//0xC4
static char vendor_version = 0;
static char panel_version = 0;
//static  char *fw_version;

#define CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP

//static int tpd_flag = 0;
static int point_num = 0;
static int p_point_num = 0;
static int key_index = -1;

struct touch_info
{
	unsigned short y[3];
	unsigned short x[3];
	unsigned short p[3];
	unsigned short count;
};

typedef struct
{
	unsigned short pos_x;
	unsigned short pos_y;
	unsigned short pos_x2;
	unsigned short pos_y2;
	unsigned short temp2;
	unsigned short temp;
	short dst_x;
	short dst_y;
	unsigned char checksum;
} SHORT_TOUCH_STATE;

 static const struct i2c_device_id msg2133_i2c_id[] = {{TPD_DEVICE,0},{}};
 static struct i2c_board_info __initdata msg2133_i2c_tpd={ I2C_BOARD_INFO(TPD_DEVICE, TOUCH_ADDR_MSG21XX )};

 //static const struct i2c_device_id tpd_i2c_id[] = {{TPD_DEVICE,0},{}};
 //static unsigned short myforce[] = {1,TOUCH_ADDR_MSG21XX,I2C_CLIENT_END,I2C_CLIENT_END};
 //static const unsigned short * const forces[] = { myforce, NULL };
 //static struct i2c_client_address_data addr_data = { .forces = forces, };
 static int boot_mode = 0;
 static int key_event_flag=0;
//  int boot_mode = 0;
//  int key_event_flag=0;


static void powerkey_call_reset()
{
 TPD_DEBUG("===============Magnum_TPD RESET.....\n");
    mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EINT_PIN, GPIO_OUT_ZERO);
    hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
    mt_set_gpio_mode(MSG_TPIO_RESET, 0x00);		//GPIO mode
    mt_set_gpio_pull_enable(MSG_TPIO_RESET, 1);	//external pull up???
    mt_set_gpio_pull_select(MSG_TPIO_RESET, GPIO_PULL_DOWN);
	mt_set_gpio_dir(MSG_TPIO_RESET,1);
    mt_set_gpio_out(MSG_TPIO_RESET,0);  //
    msleep(10);
	hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
     msleep(10);
   mt_set_gpio_mode(MSG_TPIO_RESET, 0x00);		//GPIO mode
   mt_set_gpio_pull_enable(MSG_TPIO_RESET, 1);	//external pull up???
   mt_set_gpio_pull_select(MSG_TPIO_RESET, GPIO_PULL_UP);
   mt_set_gpio_dir(MSG_TPIO_RESET,1);
   mt_set_gpio_out(MSG_TPIO_RESET,1);
   msleep(100);
   mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
   mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
   mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

}

static void tpd_reset(void)
{

 TPD_DEBUG("===============Magnum_TPD RESET.....\n");
    mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EINT_PIN, GPIO_OUT_ZERO);
    hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
    mt_set_gpio_mode(MSG_TPIO_RESET, 0x00);		//GPIO mode
    mt_set_gpio_pull_enable(MSG_TPIO_RESET, 1);	//external pull up???
    mt_set_gpio_pull_select(MSG_TPIO_RESET, GPIO_PULL_DOWN);
	mt_set_gpio_dir(MSG_TPIO_RESET,1);
    mt_set_gpio_out(MSG_TPIO_RESET,0);  //
    msleep(10);
	hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
     msleep(10);
   mt_set_gpio_mode(MSG_TPIO_RESET, 0x00);		//GPIO mode
   mt_set_gpio_pull_enable(MSG_TPIO_RESET, 1);	//external pull up???
   mt_set_gpio_pull_select(MSG_TPIO_RESET, GPIO_PULL_UP);
   mt_set_gpio_dir(MSG_TPIO_RESET,1);
   mt_set_gpio_out(MSG_TPIO_RESET,1);
   msleep(300);
   mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
   mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
   mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

}
#ifdef TP_FIRMWARE_UPDATE
/*FW UPDATE*/
//#define TPD_DEBUG 		//printk
#define FW_ADDR_MSG2133   0x62//device address of msg2133
#define FW_UPDATE_ADDR_MSG2133   0x49
#define   DOWNLOAD_FIRMWARE_BUF_SIZE   59*1024
static U8 *download_firmware_buf = NULL;
static int FwDataCnt;
struct class *firmware_class;
struct device *firmware_cmd_dev;
static  char *fw_version;
static int update_switch = 0;

#define MAX_CMD_LEN 255
static void MSG2133_En_Pin_Out(bool out)
{
	if(out)
		hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP"); //MSG2133_En_Pin_Out(1);
		
	else
		hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");  //MSG2133_En_Pin_Out(0);
		
}
static int i2c_master_send_ext(struct i2c_client *client,const char *buf ,int count)
{
	u8 *buf_dma = NULL;
	u32 old_addr = 0;
	int ret = 0;
	int retry = 3;

	if (count > MAX_CMD_LEN) {
		printk("[i2c_master_send_ext] exceed the max write length \n");
		return -1;
	}

	buf_dma= kmalloc(count,GFP_KERNEL);

	old_addr = client->addr;
	client->addr |= I2C_ENEXT_FLAG ;

	memcpy(buf_dma, buf, count);

	do {
		ret = i2c_master_send(client, (u8*)buf_dma, count);
		retry --;
		if (ret != count) {
			printk("[i2c_master_send_ext] Error sent I2C ret = %d\n", ret);
		}
	}while ((ret != count) && (retry > 0));

	client->addr = old_addr;
	kfree(buf_dma);

	return ret;

}


static int i2c_master_recv_ext(struct i2c_client *client, char *buf ,int count)
{
	u32 phyAddr = 0;
	u8  buf_dma[8] = {0};
	u32 old_addr = 0;
	int ret = 0;
	int retry = 3;
	int i = 0;
	u8  *buf_test ;
	buf_test = &buf_dma[0];

	old_addr = client->addr;
	client->addr |= I2C_ENEXT_FLAG ;

	TPD_DEBUG("[i2c_master_recv_ext] client->addr = %x\n", client->addr);

	do {
		ret = i2c_master_recv(client, buf_dma, count);
		retry --;
		if (ret != count) {
			TPD_DEBUG("[i2c_master_recv_ext] Error sent I2C ret = %d\n", ret);
		}
	}while ((ret != count) && (retry > 0));

	memcpy(buf, buf_dma, count);

	client->addr = old_addr;

	return ret;
}
void HalTscrCDevWriteI2CSeq(u8 addr, u8* data, u16 size)
{
	u8 addr_bak;

	addr_bak = i2c_client->addr;
	i2c_client->addr = addr;
	i2c_client->addr |= I2C_ENEXT_FLAG;
	i2c_master_send_ext(i2c_client,data,size);
	i2c_client->addr = addr_bak;
}


void HalTscrCReadI2CSeq(u8 addr, u8* read_data, u8 size)
{
	u8 addr_bak;

	addr_bak = i2c_client->addr;
	i2c_client->addr = addr;
	i2c_client->addr |= I2C_ENEXT_FLAG;
	i2c_master_recv_ext(i2c_client,read_data,size);
	i2c_client->addr = addr_bak;
}


void Get_Chip_Version(void)
{

	unsigned char dbbus_tx_data[3];
	unsigned char dbbus_rx_data[2];

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0xCE;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, &dbbus_tx_data[0], 3);
	HalTscrCReadI2CSeq(FW_ADDR_MSG2133, &dbbus_rx_data[0], 2);
	if (dbbus_rx_data[1] == 0)
	{
		// it is Catch2
		TPD_DEBUG("*** Catch2 ***\n");
		//FwVersion  = 2;// 2 means Catch2
	}
	else
	{
		// it is catch1
		TPD_DEBUG("*** Catch1 ***\n");
		//FwVersion  = 1;// 1 means Catch1
	}

}

void dbbusDWIICEnterSerialDebugMode()
{
	U8 data[5];

	// Enter the Serial Debug Mode
	data[0] = 0x53;
	data[1] = 0x45;
	data[2] = 0x52;
	data[3] = 0x44;
	data[4] = 0x42;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, data, 5);
}

void dbbusDWIICStopMCU()
{
	U8 data[1];

	// Stop the MCU
	data[0] = 0x37;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, data, 1);
}

void dbbusDWIICIICUseBus()
{
	U8 data[1];

	// IIC Use Bus
	data[0] = 0x35;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, data, 1);
}

void dbbusDWIICIICReshape()
{
	U8 data[1];

	// IIC Re-shape
	data[0] = 0x71;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, data, 1);
}

void dbbusDWIICIICNotUseBus()
{
	U8 data[1];

	// IIC Not Use Bus
	data[0] = 0x34;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, data, 1);
}

void dbbusDWIICNotStopMCU()
{
	U8 data[1];

	// Not Stop the MCU
	data[0] = 0x36;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, data, 1);
}

void dbbusDWIICExitSerialDebugMode()
{
	U8 data[1];

	// Exit the Serial Debug Mode
	data[0] = 0x45;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, data, 1);

	// Delay some interval to guard the next transaction
	//udelay ( 200 );        // delay about 0.2ms
}

void drvISP_EntryIspMode(void)
{
	U8 bWriteData[5] =
	{
		0x4D, 0x53, 0x54, 0x41, 0x52
	};

	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, bWriteData, 5);

	//udelay ( 1000 );        // delay about 1ms
}

U8 drvISP_Read(U8 n, U8* pDataToRead)    //First it needs send 0x11 to notify we want to get flash data back.
{
	U8 Read_cmd = 0x11;
	unsigned char dbbus_rx_data[2] = {0};
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &Read_cmd, 1);
	udelay(100);//10 zzf
	if (n == 1)
	{
		HalTscrCReadI2CSeq(FW_UPDATE_ADDR_MSG2133, &dbbus_rx_data[0], 2);
		*pDataToRead = dbbus_rx_data[0];
	}
	else
		HalTscrCReadI2CSeq(FW_UPDATE_ADDR_MSG2133, pDataToRead, n);

}

void drvISP_WriteEnable(void)
{
	U8 bWriteData[2] =
	{
		0x10, 0x06
	};
	U8 bWriteData1 = 0x12;
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, bWriteData, 2);
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData1, 1);
}


void drvISP_ExitIspMode(void)
{
	U8 bWriteData = 0x24;
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData, 1);
}

U8 drvISP_ReadStatus()
{
	U8 bReadData = 0;
	U8 bWriteData[2] =
	{
		0x10, 0x05
	};
	U8 bWriteData1 = 0x12;

	udelay(100);
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, bWriteData, 2);
	udelay(100);
	drvISP_Read(1, &bReadData);
	mdelay(10);//3->10 zzf
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData1, 1);
	return bReadData;
}


void drvISP_BlockErase(U32 addr)
{
	U8 bWriteData[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
	U8 bWriteData1 = 0x12;
	u32 timeOutCount=0;
	drvISP_WriteEnable();

	//Enable write status register
	bWriteData[0] = 0x10;
	bWriteData[1] = 0x50;
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, bWriteData, 2);
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData1, 1);

	//Write Status
	bWriteData[0] = 0x10;
	bWriteData[1] = 0x01;
	bWriteData[2] = 0x00;
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, bWriteData, 3);
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData1, 1);

	//Write disable
	bWriteData[0] = 0x10;
	bWriteData[1] = 0x04;
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, bWriteData, 2);
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData1, 1);

	udelay(100);
	timeOutCount=0;
	while ( ( drvISP_ReadStatus() & 0x01 ) == 0x01 )
	{
		timeOutCount++;
		if ( timeOutCount >= 10000 )
			break; /* around 1 sec timeout */
	}
	drvISP_WriteEnable();

	drvISP_ReadStatus();//zzf

	bWriteData[0] = 0x10;
	bWriteData[1] = 0xc7;//0xD8;        //Block Erase
	//bWriteData[2] = ((addr >> 16) & 0xFF) ;
	//bWriteData[3] = ((addr >> 8) & 0xFF) ;
	//bWriteData[4] = (addr & 0xFF) ;
	//HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData, 5);
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, bWriteData, 2);
	HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData1, 1);

	udelay(100);
	timeOutCount=0;
	while ( ( drvISP_ReadStatus() & 0x01 ) == 0x01 )
	{
		timeOutCount++;
		if ( timeOutCount >= 50000 )
			break; /* around 5 sec timeout */
	}
}

void drvISP_Program(U16 k, U8* pDataToWrite)
{
	U16 i = 0;
	U16 j = 0;
	//U16 n = 0;
	U8 TX_data[133];
	U8 bWriteData1 = 0x12;
	U32 addr = k * 1024;
	U32 timeOutCount = 0;

	for (j = 0; j < 8; j++)   //256*4 cycle
	{
		TX_data[0] = 0x10;
		TX_data[1] = 0x02;// Page Program CMD
		TX_data[2] = (addr + 128 * j) >> 16;
		TX_data[3] = (addr + 128 * j) >> 8;
		TX_data[4] = (addr + 128 * j);
		for (i = 0; i < 128; i++)
		{
			TX_data[5 + i] = pDataToWrite[j * 128 + i];
		}

		udelay(100);
		timeOutCount=0;
		while ( ( drvISP_ReadStatus() & 0x01 ) == 0x01 )
		{
			timeOutCount++;
			if ( timeOutCount >= 10000 )
				break; /* around 1 sec timeout */
		}

		drvISP_WriteEnable();
		HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, TX_data, 133);   //write 256 byte per cycle
		HalTscrCDevWriteI2CSeq(FW_UPDATE_ADDR_MSG2133, &bWriteData1, 1);
	}
}

static ssize_t firmware_update_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", fw_version);
}


static void tpd_hw_enable(unsigned int on);
static void tpd_hw_power(unsigned int on);
static ssize_t firmware_update_store(struct device *dev,
struct device_attribute *attr, const char *buf, size_t size)
{
	U8 i;
	U8 dbbus_tx_data[4];
	unsigned char dbbus_rx_data[2] = {0};
	int fd;
	//add mask interrupt
	update_switch = 1;
	mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_TOUCH_PANEL_POLARITY, 			NULL, 1);

	MSG2133_En_Pin_Out(0);
	mdelay(10);
	MSG2133_En_Pin_Out(1);
	mdelay(300);
	drvISP_EntryIspMode();
	drvISP_BlockErase(0x00000);
	mdelay(300);
	MSG2133_En_Pin_Out(0);
	mdelay(10);
	MSG2133_En_Pin_Out(1);

	mdelay(300);

#if 1
	// Enable slave's ISP ECO mode
	dbbusDWIICEnterSerialDebugMode();
	dbbusDWIICStopMCU();
	dbbusDWIICIICUseBus();
	dbbusDWIICIICReshape();

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x08;
	dbbus_tx_data[2] = 0x0c;
	dbbus_tx_data[3] = 0x08;

	// Disable the Watchdog
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);


	//Get_Chip_Version();

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x11;
	dbbus_tx_data[2] = 0xE2;
	dbbus_tx_data[3] = 0x00;

	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x3C;
	dbbus_tx_data[2] = 0x60;
	dbbus_tx_data[3] = 0x55;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x3C;
	dbbus_tx_data[2] = 0x61;
	dbbus_tx_data[3] = 0xAA;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//Stop MCU
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x0F;
	dbbus_tx_data[2] = 0xE6;
	dbbus_tx_data[3] = 0x01;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//Enable SPI Pad
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x02;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 3);
	HalTscrCReadI2CSeq(FW_ADDR_MSG2133, &dbbus_rx_data[0], 2);
	TPD_DEBUG("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
	dbbus_tx_data[3] = (dbbus_rx_data[0] | 0x20);  //Set Bit 5
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x25;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 3);
	dbbus_rx_data[0] = 0;
	dbbus_rx_data[1] = 0;
	HalTscrCReadI2CSeq(FW_ADDR_MSG2133, &dbbus_rx_data[0], 2);
	TPD_DEBUG("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
	dbbus_tx_data[3] = dbbus_rx_data[0] & 0xFC;  //Clear Bit 1,0
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//WP overwrite
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x0E;
	dbbus_tx_data[3] = 0x02;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//set pin high
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x10;
	dbbus_tx_data[3] = 0x08;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//set FRO to 50M
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x11;
	dbbus_tx_data[2] = 0xE2;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 3);
	dbbus_rx_data[0] = 0;
	dbbus_rx_data[1] = 0;
	HalTscrCReadI2CSeq(FW_ADDR_MSG2133, &dbbus_rx_data[0], 2);
	//printk("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
	dbbus_tx_data[3] = dbbus_rx_data[0] & 0xF7;  //Clear Bit 1,0
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	dbbusDWIICIICNotUseBus();
	dbbusDWIICNotStopMCU();
	dbbusDWIICExitSerialDebugMode();

	///////////////////////////////////////
	// Start to load firmware
	///////////////////////////////////////
	drvISP_EntryIspMode();
	//drvISP_BlockErase(0x00000);//////zzf
	//TPD_DEBUG("FwVersion=2");
	for (i = 0; i < 59; i++)   // total  94 KB : 1 byte per R/W
	{
		if (download_firmware_buf == NULL)
			return 0;
		drvISP_Program(i,&download_firmware_buf[i*1024]);
		mdelay(1);
	}
#endif
	drvISP_ExitIspMode();
	FwDataCnt = 0;
	if (download_firmware_buf != NULL)
	{
		kfree(download_firmware_buf);
		download_firmware_buf = NULL;
	}

	MSG2133_En_Pin_Out(0);
	msleep(100);
	MSG2133_En_Pin_Out(1);
	msleep(500);
	update_switch = 0;
	mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_POLARITY_HIGH, msg2133_tpd_eint_interrupt_handler,1);
	mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);


	return size;
}

static DEVICE_ATTR(update, S_IWUSR | S_IWGRP | S_IRUGO, firmware_update_show, firmware_update_store);

static ssize_t firmware_version_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", fw_version);
}

static ssize_t firmware_version_store(struct device *dev,
struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned char dbbus_tx_data[3];
	unsigned char dbbus_rx_data[4] ;
	unsigned short major=0, minor=0;
	fw_version = kzalloc(sizeof(char), GFP_KERNEL);

	//Get_Chip_Version();
	dbbus_tx_data[0] = 0x53;
	dbbus_tx_data[1] = 0x00;
	dbbus_tx_data[2] = 0x74;
	HalTscrCDevWriteI2CSeq(TOUCH_ADDR_MSG21XX, &dbbus_tx_data[0], 3);
	HalTscrCReadI2CSeq(TOUCH_ADDR_MSG21XX, &dbbus_rx_data[0], 4);
	major = (dbbus_rx_data[1]<<8)+dbbus_rx_data[0];
	minor = (dbbus_rx_data[3]<<8)+dbbus_rx_data[2];
	TPD_DEBUG("***major = %d ***\n", major);
	TPD_DEBUG("***minor = %d ***\n", minor);
	sprintf(fw_version,"%03d%03d", major, minor);
	TPD_DEBUG("***fw_version = %s ***\n", fw_version);

	return size;

}
static DEVICE_ATTR(version, S_IWUSR | S_IWGRP | S_IRUGO, firmware_version_show, firmware_version_store);


static ssize_t firmware_data_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
	return FwDataCnt;
}

static ssize_t firmware_data_store(struct device *dev,
struct device_attribute *attr, const char *buf, size_t size)
{
	TPD_DEBUG("***FwDataCnt = %d ***\n", FwDataCnt);
	int i;
	if (download_firmware_buf == NULL) {
		download_firmware_buf = kzalloc(DOWNLOAD_FIRMWARE_BUF_SIZE, GFP_KERNEL);
		if (download_firmware_buf == NULL)
			return NULL;
	}
	if(FwDataCnt<59)
	{
		memcpy(&download_firmware_buf[FwDataCnt*1024], buf, 1024);
	}
	FwDataCnt++;
	return size;
}
static DEVICE_ATTR(data, S_IWUSR | S_IWGRP | S_IRUGO, firmware_data_show, firmware_data_store);

#endif

 static void tpd_down(int x, int y, int p) {
	 input_report_abs(tpd->dev, ABS_PRESSURE, p);
	 input_report_key(tpd->dev, BTN_TOUCH, 1);
	 input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 128);
	 input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
	 input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
	 //TPD_DEBUG("D[%4d %4d %4d] ", x, y, p);
	 input_mt_sync(tpd->dev);
	 TPD_DOWN_DEBUG_TRACK(x,y);
 }

 static int tpd_up(int x, int y,int *count) {
	 if(*count>0) {
		 input_report_abs(tpd->dev, ABS_PRESSURE, 0);
		 input_report_key(tpd->dev, BTN_TOUCH, 0);
		 input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 0);
		 input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
		 input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
		 //TPD_DEBUG("U[%4d %4d %4d] ", x, y, 0);
		 input_mt_sync(tpd->dev);
		 TPD_UP_DEBUG_TRACK(x,y);
		 (*count)--;
		 return 1;
	 } return 0;
 }


#ifdef CTP_SIMULATE_PS

u8 msg2133_get_ps(void)
{
      return  ps_data;
}
void ctp_convert(char mode,bool force)
{
    U8 ps_store_data[4];
    TPD_DEBUG("current_CTP_PS mode=%d\n", mode);
	if(force==false)
	{
		if((!tpd_load_status_ok)||(mode==ps_mode)){
			return;
		}
	}
	if(DISABLE_CTP_PS == mode)
	{
		ps_mode = 0;
		TPD_DEBUG("DISABLE_CTP_PS mode=%d\n", mode);
		ps_store_data[0] = 0x52;
		ps_store_data[1] = 0x00;
		ps_store_data[2] = 0x62;
		ps_store_data[3] = 0xa1;
		HalTscrCDevWriteI2CSeq(TOUCH_ADDR_MSG21XX, &ps_store_data[0], 4);
	}
	else if(ENABLE_CTP_PS == mode)
	{
		TPD_DEBUG("ENABLE_CTP_PS mode=%d\n", mode);
		ps_mode = 1;
		ps_store_data[0] = 0x52;
		ps_store_data[1] = 0x00;
		ps_store_data[2] = 0x62;
		ps_store_data[3] = 0xa0;
		HalTscrCDevWriteI2CSeq(TOUCH_ADDR_MSG21XX, &ps_store_data[0], 4);
	}
}

void tpd_resume_process_in_ps_mode(struct i2c_client *client)
{
	TPD_DEBUG("tpd_resume_process_in_ps_mode\n");
	if(power_pressed ){
		U8 ps_store_data[4];
		TPD_DEBUG("tpd_resume_process_in_ps_mode power_pressed=%d\n", power_pressed);
		ps_data_pre=ps_data=1;
	    msg2133_ps_event_handler();  //do not work...

		//mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
		powerkey_call_reset();
		TPD_DEBUG("ENABLE_CTP_PS1 \n");
		/*
		ps_store_data[0] = 0x52;
		ps_store_data[1] = 0x00;
		ps_store_data[2] = 0x62;
		ps_store_data[3] = 0xa0;
		i2c_write(TOUCH_ADDR_MSG21XX, &ps_store_data[0], 4);  */

		//mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
		//msleep(200);

	}
	//tpd_eint_interrupt_handler();
}

#if 0
static ssize_t show_proximity_sensor(struct device *dev, struct device_attribute *attr, char *buf)
{
	TPD_DEBUG("*** show_proximity_sensor= %d***\n", ps_data);
	return sprintf(buf, "%d\n", ps_data);
}

static ssize_t store_proximity_sensor(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	char mode;
	if(buf != NULL && size != 0)
	{
		mode = *buf;
		ctp_convert(mode,false);
	}

	return size;
}
static DEVICE_ATTR(proximity_sensor, 0664, show_proximity_sensor, store_proximity_sensor);
#endif
#endif

#ifdef CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP
struct tinno_ts_data {
	struct wake_lock wake_lock;
	atomic_t isp_opened;
	uint8_t *isp_pBuffer;
	struct i2c_client *client;
};
static struct tinno_ts_data *g_pts = NULL;

static struct mutex tp_mutex;

#define MSG2133_TOUCH_IO_MAGIC ('G')
#define MSG2133_IOCTL_UPDATE_FIRMWARE		_IO(MSG2133_TOUCH_IO_MAGIC , 0x00)
#define MSG2133_IOCTL_GET_VENDORID		_IOR(MSG2133_TOUCH_IO_MAGIC, 0x01, int)
#define MSG2133_IOCTL_RESET			    _IO(MSG2133_TOUCH_IO_MAGIC, 0x02)
#define MSG2133_IOC_MAXNR				(0x03)

static u8 tpd_down_state=0;
static int down_x=0;
static int down_y=0;
static void msg2133_fts_isp_register(struct i2c_client *client);
#endif

#ifdef CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP

static int msg2133_do_start_upgrade()
{
    TPD_DEBUG("Magnum msg2133_do_start_upgrade\n");
	U8 i;
	U8 dbbus_tx_data[4];
	unsigned char dbbus_rx_data[2] = {0};
	int fd;
	//add mask interrupt
	update_switch = 1;
	mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_TOUCH_PANEL_POLARITY, 			NULL, 1);

	MSG2133_En_Pin_Out(0);
	mdelay(10);
	MSG2133_En_Pin_Out(1);
	mdelay(300);
	drvISP_EntryIspMode();
	drvISP_BlockErase(0x00000);
	mdelay(300);
	MSG2133_En_Pin_Out(0);
	mdelay(10);
	MSG2133_En_Pin_Out(1);

	mdelay(300);

#if 1
	// Enable slave's ISP ECO mode
	dbbusDWIICEnterSerialDebugMode();
	dbbusDWIICStopMCU();
	dbbusDWIICIICUseBus();
	dbbusDWIICIICReshape();

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x08;
	dbbus_tx_data[2] = 0x0c;
	dbbus_tx_data[3] = 0x08;

	// Disable the Watchdog
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);


	//Get_Chip_Version();

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x11;
	dbbus_tx_data[2] = 0xE2;
	dbbus_tx_data[3] = 0x00;

	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x3C;
	dbbus_tx_data[2] = 0x60;
	dbbus_tx_data[3] = 0x55;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x3C;
	dbbus_tx_data[2] = 0x61;
	dbbus_tx_data[3] = 0xAA;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//Stop MCU
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x0F;
	dbbus_tx_data[2] = 0xE6;
	dbbus_tx_data[3] = 0x01;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//Enable SPI Pad
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x02;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 3);
	HalTscrCReadI2CSeq(FW_ADDR_MSG2133, &dbbus_rx_data[0], 2);
	TPD_DEBUG("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
	dbbus_tx_data[3] = (dbbus_rx_data[0] | 0x20);  //Set Bit 5
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x25;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 3);
	dbbus_rx_data[0] = 0;
	dbbus_rx_data[1] = 0;
	HalTscrCReadI2CSeq(FW_ADDR_MSG2133, &dbbus_rx_data[0], 2);
	TPD_DEBUG("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
	dbbus_tx_data[3] = dbbus_rx_data[0] & 0xFC;  //Clear Bit 1,0
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//WP overwrite
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x0E;
	dbbus_tx_data[3] = 0x02;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//set pin high
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x1E;
	dbbus_tx_data[2] = 0x10;
	dbbus_tx_data[3] = 0x08;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	//set FRO to 50M
	dbbus_tx_data[0] = 0x10;
	dbbus_tx_data[1] = 0x11;
	dbbus_tx_data[2] = 0xE2;
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 3);
	dbbus_rx_data[0] = 0;
	dbbus_rx_data[1] = 0;
	HalTscrCReadI2CSeq(FW_ADDR_MSG2133, &dbbus_rx_data[0], 2);
	//printk("dbbus_rx_data[0]=0x%x", dbbus_rx_data[0]);
	dbbus_tx_data[3] = dbbus_rx_data[0] & 0xF7;  //Clear Bit 1,0
	HalTscrCDevWriteI2CSeq(FW_ADDR_MSG2133, dbbus_tx_data, 4);

	dbbusDWIICIICNotUseBus();
	dbbusDWIICNotStopMCU();
	dbbusDWIICExitSerialDebugMode();

	///////////////////////////////////////
	// Start to load firmware
	///////////////////////////////////////
	drvISP_EntryIspMode();
	//drvISP_BlockErase(0x00000);//////zzf
	//TPD_DEBUG("FwVersion=2");
	for (i = 0; i < 59; i++)   // total  94 KB : 1 byte per R/W
	{
		if (download_firmware_buf == NULL)
			return 0;
		drvISP_Program(i,&download_firmware_buf[i*1024]);
		mdelay(1);
	}
#endif
	drvISP_ExitIspMode();
	FwDataCnt = 0;
	if (download_firmware_buf != NULL)
	{
		kfree(download_firmware_buf);
		download_firmware_buf = NULL;
	}

	MSG2133_En_Pin_Out(0);
	msleep(100);
	MSG2133_En_Pin_Out(1);
	msleep(500);
	update_switch = 0;
	mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_POLARITY_HIGH, msg2133_tpd_eint_interrupt_handler,1);
	mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	msg2133_ts_get_fw_version();
	if(panel_version>0)
		return 1;
	else
		return 0;
}

static inline int _lock(atomic_t *excl)
{
	if (atomic_inc_return(excl) == 1) {
		return 0;
	} else {
		atomic_dec(excl);
		return -1;
	}
}

static inline void _unlock(atomic_t *excl)
{
	atomic_dec(excl);
}

static ssize_t msg2133_fts_isp_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	//ret = copy_to_user(buf,&acc, sizeof(acc));
	TPD_DEBUG("");
	return -EIO;
}

static ssize_t msg2133_fts_isp_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    char * tmp = NULL;
	tmp = kmalloc(count, GFP_KERNEL);
	if (tmp == NULL)
        return -ENOMEM;

    if (copy_from_user(tmp, buf, count)) {
        return -EFAULT;
    }
   	int i;
	TPD_DEBUG("***FwDataCnt = %d ***\n", FwDataCnt);
	if (download_firmware_buf == NULL) {
		download_firmware_buf = kzalloc(DOWNLOAD_FIRMWARE_BUF_SIZE, GFP_KERNEL);
		if (download_firmware_buf == NULL)
			return NULL;
	}
	if(FwDataCnt<59)
	{
		memcpy(&download_firmware_buf[FwDataCnt*1024], tmp, 1024);
	}
	FwDataCnt++;
	kfree(tmp);
	return count;
   return 0;
}

static int msg2133_fts_isp_open(struct inode *inode, struct file *file)
{
	struct tinno_ts_data *ts = file->private_data;

	TPD_DEBUG("try to open isp.\n");

	if (_lock(&g_pts->isp_opened)){
		TPD_DEBUG("isp is already opened.");
		return -EBUSY;
	}

	mutex_lock(&tp_mutex);

	file->private_data = g_pts;

    mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	wake_lock(&g_pts->wake_lock);

	TPD_DEBUG("isp open success.");
	return 0;
}

static int msg2133_fts_isp_close(struct inode *inode, struct file *file)
{
	struct tinno_ts_data *ts = file->private_data;

	TPD_DEBUG("try to close isp.");

 	mutex_unlock(&tp_mutex);

    mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

	file->private_data = NULL;

	_unlock ( &ts->isp_opened );

	wake_unlock(&ts->wake_lock);

	TPD_DEBUG("close isp success!");
	return 0;
}

static int msg2133_fts_isp_get_buffer(void)
{

}
static int msg2133_fts_switch_to_update(struct tinno_ts_data *ts)
{
   return 0;
}

static int msg2133_fts_mode_switch(struct tinno_ts_data *ts, int iMode)
{
   return 0;
}


//edit by Magnum 2012-2-2
static int  msg2133_fts_ctpm_auto_clb(void)
{
   return 0;
}
static int msg2133_fts_isp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct tinno_ts_data *ts = file->private_data;
	int flag;
	int rc = 0;
	uint8_t  command = 0xFC;
	uint8_t  checksum,calib_buf;

	if ( !atomic_read( &g_pts->isp_opened ) ){
		TPD_DEBUG("no opened isp.\n");
		return -ENODEV;
	}

	/* check cmd */
	if(_IOC_TYPE(cmd) != MSG2133_TOUCH_IO_MAGIC)
	{
		TPD_DEBUG("cmd magic type error\n");
		return -EINVAL;
	}
	if(_IOC_NR(cmd) > MSG2133_IOC_MAXNR)
	{
		TPD_DEBUG("cmd number error\n");
		return -EINVAL;
	}

	if(_IOC_DIR(cmd) & _IOC_READ)
		rc = !access_ok(VERIFY_WRITE,(void __user*)arg, _IOC_SIZE(cmd));
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
		rc = !access_ok(VERIFY_READ, (void __user*)arg, _IOC_SIZE(cmd));
	if(rc)
	{
		TPD_DEBUG("cmd access_ok error\n");
		return -EINVAL;
	}

	switch (cmd) {
	case MSG2133_IOCTL_UPDATE_FIRMWARE:
		printk("Magnum update firmware .....");
		 if(msg2133_do_start_upgrade())
		 	printk("OK OK OK update firmware .....");
		 else
		 	printk("Fail Fail Fail update firmware .....");
		break;
	case MSG2133_IOCTL_RESET:
		{
		    	tpd_reset();
		        break;}
	case MSG2133_IOCTL_GET_VENDORID:
            {

                TPD_DEBUG("Magnum vendor_id=0x%x\n", vendor_version);

                if(copy_to_user(argp,&vendor_version,sizeof(char))!=0)
                {
                    TPD_DEBUG(KERN_INFO "copy_to_user error\n");
                    rc = -EFAULT;
                }
                break;
            }
	default:
		TPD_DEBUG("invalid command %d\n", _IOC_NR(cmd));
		rc = -EINVAL;
		break;
	}

	return rc;
}


static const struct file_operations msg2133_fts_isp_fops = {
	.owner = THIS_MODULE,
	.read = msg2133_fts_isp_read,
	.write = msg2133_fts_isp_write,
	.open = msg2133_fts_isp_open,
	.release = msg2133_fts_isp_close,
	.unlocked_ioctl = msg2133_fts_isp_ioctl,
};

static struct miscdevice msg2133_fts_isp_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "msg2133_fts_isp",
	.fops = &msg2133_fts_isp_fops,
};

static void msg2133_fts_isp_register(struct i2c_client *client)
{
	struct tinno_ts_data *ts;
	int err,ret;

	ts = kzalloc(sizeof(*ts), GFP_KERNEL);
	if (ts == NULL) {
		ret = -ENOMEM;
		goto err_detect_failed;
	}

	wake_lock_init(&ts->wake_lock, WAKE_LOCK_SUSPEND, "fts_tp_isp");
	ts->client = i2c_client;
	err = misc_register(&msg2133_fts_isp_device);
	if (err) {
		TPD_DEBUG(KERN_ERR "fts_isp_device device register failed\n");
		goto exit_misc_device_register_failed;
	}else
		g_pts = ts;
	return;

err_detect_failed:
	kfree(ts);
	return;

exit_misc_device_register_failed:
	wake_lock_destroy(&ts->wake_lock);
}

#endif



//get tp_firmware
static int msg2133_get_fw_version_stored()
{
//edit by Magnum 2012-3-1
//    panel_version = tinno_ts_get_fw_version();
    return panel_version;
}

static int msg2133_get_vendor_version_stored()
{
     return vendor_version;
}

static int msg2133_ts_get_fw_version()
{
    TPD_DEBUG("magnum msg2133_ts_get_fw_version \n");
    unsigned char dbbus_tx_data[3];
    unsigned char dbbus_rx_data[4] ;
    unsigned short major = 0, minor = 0;
	int ret;
    dbbusDWIICEnterSerialDebugMode();
    dbbusDWIICStopMCU();
    dbbusDWIICIICUseBus();
    dbbusDWIICIICReshape();
    fw_version = kzalloc(sizeof(char), GFP_KERNEL);
    dbbus_tx_data[0] = 0x53;
    dbbus_tx_data[1] = 0x00;
    dbbus_tx_data[2] = 0x74;
 //   i2c_write(TOUCH_ADDR_MSG21XX, &dbbus_tx_data[0], 3);
 //   i2c_read(TOUCH_ADDR_MSG21XX, &dbbus_rx_data[0], 4);

   ret = i2c_master_send(i2c_client,&dbbus_tx_data[0], 3);
    if(ret <= 0)
    {
        TPD_DEBUG("Magnum i2c_write_interface error line = %d, ret = %d\n", __LINE__, ret);
		return -1;
    }

	ret = i2c_master_recv(i2c_client,&dbbus_rx_data[0], 4);
	 if(ret <= 0)
    {
        TPD_DEBUG("Magnum i2c_read_interface error line = %d, ret = %d\n", __LINE__, ret);
		return -1;
    }
    major = (dbbus_rx_data[1] << 8) + dbbus_rx_data[0];
    minor = (dbbus_rx_data[3] << 8) + dbbus_rx_data[2];
    TPD_DEBUG("***major = %x ***\n", major);
    TPD_DEBUG("***minor = %x ***\n", minor);
	vendor_version = major;   //major is vendor_version ,eg: YD ,BYD ,Truly
	panel_version = minor;    //minor is panel_version ,eg BM-24, Truly-16.
	 TPD_DEBUG("*** vendor_version = %x ***\n", vendor_version);
	 TPD_DEBUG("*** panel_version = %x ***\n", panel_version);
    sprintf(fw_version, "%02d%02d", major, minor);
    TPD_DEBUG("***fw_version = %s ***\n", fw_version);
	TPD_DEBUG("tpd_local_init boot mode = %d \n", boot_mode);

	return 1;
}
//end get tp_firmware



unsigned char tpd_check_sum(unsigned char *pval)
{
	int i, sum = 0;

	for(i = 0; i < 7; i++)
	{
		sum += pval[i];
	}

	return (unsigned char)((-sum) & 0xFF);
}


static bool msg2133_i2c_read(char *pbt_buf, int dw_lenth)
{
	int ret;
	i2c_client->timing = 100;
	i2c_client->addr|=I2C_ENEXT_FLAG;
     // TPD_DEBUG("Magnum msg_i2c read  ..............\n");
	ret = i2c_master_recv(i2c_client, pbt_buf, dw_lenth);
	if(ret <= 0)
	{
        TPD_DEBUG("Magnum msg_i2c read error ..............\n");
		return false;
	}
      //  TPD_DEBUG("Magnum msg_i2c read OK ..............\n");
	return true;
}

BYTE touch_flag=0x00;
static int tpd_touchinfo(struct touch_info *cinfo)
{
	SHORT_TOUCH_STATE ShortTouchState;
	BYTE reg_val[8] = {0};
	unsigned int  temp = 0;

	if(update_switch)
		return false;
	if(!msg2133_i2c_read(reg_val, 8))
		return false;
       int ii = 0;
       /*while(++ii <8){
            TPD_DEBUG("Magnum original reg_val-%d ==%x \n",ii,reg_val[ii]);
       }*/

	ShortTouchState.pos_x = ((reg_val[1] & 0xF0) << 4) | reg_val[2];
	ShortTouchState.pos_y = ((reg_val[1] & 0x0F) << 8) | reg_val[3];
	ShortTouchState.dst_x = ((reg_val[4] & 0xF0) << 4) | reg_val[5];
	ShortTouchState.dst_y = ((reg_val[4] & 0x0F) << 8) | reg_val[6];

  //  TPD_DEBUG("ShortTouchState.pos_x  === %d  \n ShortTouchState.pos_y === %d \n ShortTouchState.dst_x=== %d \n  ShortTouchState.dst_y=== %d \n ",ShortTouchState.pos_x,ShortTouchState.pos_y,ShortTouchState.dst_x,ShortTouchState.dst_y);

	if((ShortTouchState.dst_x) & 0x0800)
	{
		ShortTouchState.dst_x |= 0xF000;
	}

	if((ShortTouchState.dst_y) & 0x0800)
	{
		ShortTouchState.dst_y |= 0xF000;
	}

	ShortTouchState.pos_x2 = ShortTouchState.pos_x + ShortTouchState.dst_x;
	ShortTouchState.pos_y2 = ShortTouchState.pos_y + ShortTouchState.dst_y;
	temp = tpd_check_sum(reg_val);
	if(temp == reg_val[7])
	{
		if(reg_val[0] == 0x52) //CTP  ID
		{
			if(reg_val[1] == 0xFF&& reg_val[4] == 0xFF)
			{
                                  //default ==== reg_val[6]
        #ifdef CTP_SIMULATE_PS
				if(reg_val[5] == 0x80) // close to
				{
				    TPD_DEBUG("slaver report i am close...\n");
					ps_data = 0;
				}
				else if(reg_val[5] == 0x40) // leave
				{
				    TPD_DEBUG("slaver report i am far away...\n");
					ps_data = 1;
				}
				// when ic reset himself,it will send c0 to host,and when host in call ,convert mode.
				else if(reg_val[5] == 0xc0)
				{
                    TPD_DEBUG("report call state %d to msg2133...\n",ps_mode);
					ctp_convert(ps_mode,true);
				}
				else
		#endif
				if(reg_val[5]==0xFF||reg_val[5]==0x00)
				{
			//		TPD_DEBUG("Magnum Num 000000 ======\n");
					if(touch_flag)
					{
						touch_flag=0;
						point_num = 0;
					}
					else
					{
						return  false;
					}
				}
				else if(reg_val[5] == 0x01)
				{
				//Key 1
               TPD_DEBUG("Magnum Key menu ======\n");
				key_index = 1;
				cinfo->x[0] =TPD_CUST_KEY_X3;
				cinfo->y[0] = TPD_CUST_KEY_Y;
				point_num = 1;
				touch_flag=1;
         //       TPD_DEBUG(" x === %d  y === %d \n",cinfo->x[0],cinfo->y[0]);
				}
				else if(reg_val[5] == 0x02)
				{
                TPD_DEBUG("Magnum Key home ======\n");
				key_index = 2;
				cinfo->x[0] = TPD_CUST_KEY_X2;
				cinfo->y[0] = TPD_CUST_KEY_Y;
				point_num = 1;
				touch_flag=1;
        //        TPD_DEBUG(" x === %d  y === %d \n",cinfo->x[0],cinfo->y[0]);
					//Key 2
				}
				else if(reg_val[5] == 0x04)
				{
				    TPD_DEBUG("Magnum Key return ======\n");
					key_index = 3;
					cinfo->x[0] = TPD_CUST_KEY_X1;//tpd_keys_dim_local[key_index-1][0];
					cinfo->y[0] = TPD_CUST_KEY_Y;//tpd_keys_dim_local[key_index-1][1];
					point_num = 1;
					touch_flag=1;
				}
				#ifdef TPD_CUST_KEY_X4
				else if(reg_val[5] == 0x08)
				{
					key_index = 0;
					cinfo->x[0] = TPD_CUST_KEY_X4;
					cinfo->y[0] = TPD_CUST_KEY_Y;
					point_num = 1;
				}
				#endif
				else
				{
					return  false;
				}
			}
			else if(ShortTouchState.pos_x > 2047 || ShortTouchState.pos_y > 2047)
			{
				return  false;
			}
			else if((ShortTouchState.dst_x == 0) && (ShortTouchState.dst_y == 0))
			{
				cinfo->x[0] = (ShortTouchState.pos_x * TPD_CUST_RES_X) / 2048;
				cinfo->y[0] = (ShortTouchState.pos_y * TPD_CUST_RES_Y) / 2048;
				point_num = 1;
				touch_flag=1;
			}
			else
			{
                TPD_DEBUG("Point NUm ==== 222222 \n");
				if(ShortTouchState.pos_x2 > 2047 || ShortTouchState.pos_y2 > 2047)
					return false;
				cinfo->x[0] = (ShortTouchState.pos_x *TPD_CUST_RES_X) / 2048;
				cinfo->y[0] = (ShortTouchState.pos_y *  TPD_CUST_RES_Y) / 2048;
				cinfo->x[1] = (ShortTouchState.pos_x2 * TPD_CUST_RES_X) / 2048;
				cinfo->y[1] = (ShortTouchState.pos_y2 * TPD_CUST_RES_Y) / 2048;
				point_num = 2;
				touch_flag=1;
			}
		}
		else{
              TPD_DEBUG("Magnum 0x52 err ....\n");
		}
		return true;
	}
	else
	{
	    TPD_DEBUG("Magnum checksum err ....\n");
		return  false;
	}

}

 static int touch_event_handler(void *unused)
 {
    struct touch_info cinfo;
	int touch_state = 3;
	unsigned long time_eclapse;
	struct sched_param param = { .sched_priority = RTPM_PRIO_TPD };
	sched_setscheduler(current, SCHED_RR, &param);
	do
	{
	  //edit by Magnum 2012-8-30: if tp go bad in call ,reset the TP
	  #ifdef CTP_SIMULATE_PS
	    TPD_DEBUG(" ps mode == %d && power_pressed == %d \n",ps_mode,power_pressed);
	    if(ps_mode == 1 && power_pressed )//&& ps_data == 0 ) // && !tpd_touchinfo(&cinfo))
	    {
			tpd_resume_process_in_ps_mode(i2c_client);
		}
	  #endif
		mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(waiter, tpd_flag != 0);
		tpd_flag = 0;
		set_current_state(TASK_RUNNING);
		if(tpd_touchinfo(&cinfo))
	          {
				if(point_num == 1)
				{
	     //           TPD_DEBUG("Magnum report to host ........\n");
	    			 TPD_DEBUG("MSG_X = %d,MSG_Y = %d\n", cinfo.x[0], cinfo.y[0]);
	                if(cinfo.y[0] < TPD_CUST_RES_Y){
						TPD_DEBUG("Magnum point is in Working area ........\n");
						if(cinfo.x[0] == 0) cinfo.x[0] = 1;
						if(cinfo.y[0] == 0) cinfo.y[0] = 1;
	                    tpd_down(cinfo.x[0], cinfo.y[0], 1);
	                    input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, 0);
	                }
	                else
	                {
	                   if (FACTORY_BOOT== boot_mode || RECOVERY_BOOT == boot_mode)
	                   {
	                      TPD_DEBUG("Magnum point is FACTORY_BOOT or RECOVERY_BOOT  KEY area ........\n");
	                      tpd_button(cinfo.x[0], cinfo.y[0], 1);

	                   	}
					   else
					   	{
					   	  TPD_DEBUG("Magnum point is NORMAL_BOOT KEY area ........\n");
	                       tpd_down(cinfo.x[0], cinfo.y[0], 1);
						   input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, 0);
	                    }
	               }
					input_sync(tpd->dev);
				}
				else if (point_num == 2){
					// ps_data = 0;
				//	 TPD_DEBUG("ps_data ==== %d........\n",ps_data);
					 TPD_DEBUG("Magnum point Number 22222222........\n");
					 TPD_DEBUG("First point MSG_X = %d,MSG_Y = %d\n", cinfo.x[0], cinfo.y[0]);
					 TPD_DEBUG("Second point MSG_X = %d,MSG_Y = %d\n", cinfo.x[1], cinfo.y[1]);
	                if((cinfo.y[0] < TPD_CUST_RES_Y)&&(cinfo.y[1] < TPD_CUST_RES_Y)){
	                tpd_down(cinfo.x[0], cinfo.y[0], 1);
	                input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, 0);
	                tpd_down(cinfo.x[1], cinfo.y[1], 1);
	                input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, 1);
		            }
		            else if((cinfo.y[0] > TPD_CUST_RES_Y)&&(cinfo.y[1] < TPD_CUST_RES_Y))
		            {
					 	tpd_down(cinfo.x[1], cinfo.y[1], 1);
		                input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, 0);
		            }
		            else if((cinfo.y[0] < TPD_CUST_RES_Y)&&(cinfo.y[1] > TPD_CUST_RES_Y))
		            {
				 	    tpd_down(cinfo.x[0], cinfo.y[0], 1);
		                input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, 0);
		            }
		            input_sync(tpd->dev);
	         	}

	            else if(point_num == 0){
		//		       TPD_DEBUG("Magnum  tp release/........\n");
	                	if(key_index > -1)
						{
		//				TPD_DEBUG("UP  key ====%d",tpd_keys_local[key_index]);
	                  if (NORMAL_BOOT != boot_mode)
	                      tpd_button(cinfo.x[0], cinfo.y[0], 0);
			           	  key_index = -1;
						}
					input_mt_sync(tpd->dev);
					input_sync(tpd->dev);
				}
	}
	else{
        TPD_DEBUG("Magnun get touch info err......\n");
	}

	//edit by Magnum 2012-7-27
#ifdef CTP_SIMULATE_PS
	  if(tpd_load_status_ok&&(ps_data_pre!=ps_data)){
	      	msg2133_ps_event_handler();
			ps_data_pre=ps_data;
	  }
#endif

 }while(!kthread_should_stop());

	 return 0;
 }

 static int tpd_i2c_detect (struct i2c_client *client, int kind, struct i2c_board_info *info)
 {
	int error;
    TPD_DEBUG("TPD detect \n");


	 strcpy(info->type, TPD_DEVICE);




	  return 0;
 }


 void msg2133_tpd_eint_interrupt_handler(void)
 {
	// mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM); //1009 mask eint
	 TPD_DEBUG("TPD interrupt has been triggered \n");
	 tpd_flag = 1;
	 wake_up_interruptible(&waiter);
 }
 static int __devinit tpd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
 {

	int error;
	int retval = TPD_OK;
    u8 host_reg;

        //Disable the real PS by ljs
	mt_set_gpio_mode(GPIO_ALS_EN_PIN, GPIO_MODE_GPIO);
	mt_set_gpio_dir(GPIO_ALS_EN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_pull_enable(GPIO_ALS_EN_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_ALS_EN_PIN, GPIO_PULL_UP);
	mt_set_gpio_out(GPIO_ALS_EN_PIN, 1);

	i2c_client = client;
    hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
	error = tpd_initialize(client);
    if (error)
	{
	   TPD_DEBUG("tpd_initialize error\n");
	}

    TPD_DEBUG("Power on\n");

     char data;
    if((i2c_smbus_read_i2c_block_data(i2c_client, 0x00, 1, &data)) < 0)
        {
            TPD_DEBUG("Msg2133 I2C transfer error, func: %s\n", __func__);
            hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
            return -1;
        }

    tpd_load_status = 1;

    msg2133_ts_get_fw_version();
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(GPIO_CTP_EINT_PIN,GPIO_PULL_UP);
	mt65xx_eint_set_sens(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_SENSITIVE);
	mt65xx_eint_set_hw_debounce(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_TOUCH_PANEL_POLARITY, msg2133_tpd_eint_interrupt_handler, 1);
    msleep(300);
	mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

	msleep(100);

	#ifdef CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP
	msg2133_fts_isp_register(i2c_client);
	mutex_init(&tp_mutex);
    #endif

#ifdef TP_FIRMWARE_UPDATE
	TPD_DEBUG("Magnum i2c client name ==== %s\n",client->name);
	firmware_class = class_create(THIS_MODULE,client->name);// "ms-touchscreen-msg20xx");

	if(IS_ERR(firmware_class))
	{
		TPD_DEBUG("Failed to create class(firmware)!\n");
	}

	firmware_cmd_dev = device_create(firmware_class,NULL, 0, NULL, "device");

	if(IS_ERR(firmware_cmd_dev))
	{
		TPD_DEBUG("Failed to create device(firmware_cmd_dev)!\n");
	}

	// version
	if(device_create_file(firmware_cmd_dev, &dev_attr_version) < 0)
	{
		TPD_DEBUG("Failed to create device file(%s)!\n", dev_attr_version.attr.name);
	}

	// update
	if(device_create_file(firmware_cmd_dev, &dev_attr_update) < 0)
	{
		TPD_DEBUG("Failed to create device file(%s)!\n", dev_attr_update.attr.name);
	}

	// data
	if(device_create_file(firmware_cmd_dev, &dev_attr_data) < 0)
	{
		TPD_DEBUG("Failed to create device file(%s)!\n", dev_attr_data.attr.name);
	}

	dev_set_drvdata(firmware_cmd_dev, NULL);
#endif

#if 0 //def CTP_SIMULATE_PS

	if(device_create_file(firmware_cmd_dev, &dev_attr_proximity_sensor) < 0) // /sys/class/mtk-tpd/device/proximity_sensor
	{
		TPD_DEBUG("Failed to create device file(%s)!\n", dev_attr_proximity_sensor.attr.name);
	}
#endif
  mythread = kthread_run(touch_event_handler, 0, TPD_DEVICE);
    if (IS_ERR(mythread))
	 {
	  error = PTR_ERR(mythread);
	  TPD_DMESG(TPD_DEVICE " failed to create kernel thread: %d\n", error);
	}
	TPD_DMESG("Touch Panel Device Probe %s\n", (retval < TPD_OK) ? "FAIL" : "PASS");
	#ifdef CTP_SIMULATE_PS
	tpd_load_status_ok=1;
	#endif
    //edit by Magnum 2012-8-8 try to soleve tp go bad in FTM
    tpd_initialize(client);
    return retval;

 }

 static int tpd_initialize(struct i2c_client * client)
 {
   int retval = TPD_OK;
    tpd_reset();
	return retval;
 }
 static int __devexit tpd_i2c_remove(struct i2c_client *client)

 {
   int error;
     i2c_unregister_device(client);
     mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
  #ifdef CONFIG_TOUCHSCREEN_MSG2133_SUPPORT_ISP
	misc_deregister(&msg2133_fts_isp_device);
	wake_lock_destroy(&g_pts->wake_lock);
	g_pts = NULL;
#endif
  #ifdef CONFIG_HAS_EARLYSUSPEND
	 unregister_early_suspend(&early_suspend);
  #endif /* CONFIG_HAS_EARLYSUSPEND */
     input_unregister_device(tpd->dev);
	 TPD_DEBUG("TPD removed\n");

   return 0;
 }

static  struct i2c_driver tpd_i2c_driver = {
    .probe = tpd_i2c_probe,
    .remove =  __devexit_p(tpd_i2c_remove),
    .detect = tpd_i2c_detect,
    .driver.name = "msg2133",//TPD_DEVICE,
    .id_table = msg2133_i2c_id,
//    .address_data = &addr_data,
};

static int tpd_local_init(void)
 {
   int retval ,i;
     boot_mode = get_boot_mode();
    // Software reset mode will be treated as normal boot
    if(boot_mode==3) boot_mode = NORMAL_BOOT;
//Ivan
  TPD_DEBUG("tpd_local_init boot mode = %d\n",boot_mode);
  TPD_DMESG("MSG2133 I2C Touchscreen Driver (Built %s @ %s)\n", __DATE__, __TIME__);


   retval = i2c_add_driver(&tpd_i2c_driver);

#ifdef TPD_HAVE_BUTTON
    if (FACTORY_BOOT == boot_mode)
    {
    TPD_DEBUG("Magnum i am in Factory mode\n");
	for (i = 0; i < TPD_KEY_COUNT ; i++)
	    tpd_keys_local[i] = TPD_KEYSFACTORY[i];
    }

    tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local, tpd_keys_dim_local);// initialize tpd button data
#endif

   return retval;

 }

static int tpd_resume(struct i2c_client *client)
 {
  int retval = TPD_OK;
//  TPD_DEBUG("================Magnum_TPD resume ps_mode=%d\n",ps_mode);
/*
#ifdef CTP_SIMULATE_PS
   if(ps_mode == 1){
		TPD_DEBUG("Magnum ps_mode ==%d, simulate PS,do not resume!\n",ps_mode);
		 return TPD_OK;
	}
	else
	    TPD_DEBUG("Magnum ps_mode ==%d, resume!!!!\n",ps_mode);
#endif  */
   TPD_DEBUG("Magnum  ==%d!\n",ctp_suspend);
   if(ctp_suspend){
		TPD_DEBUG("ctp not suspend, resume depends on suspend...!\n");
		 return TPD_OK;
	}
   #ifdef TP_FIRMWARE_UPDATE
	if(update_switch==1)
	{
		return 0;
	}
	#endif

   hwPowerOn(MT65XX_POWER_LDO_VGP2, VOL_2800, "TP");
   mt_set_gpio_mode(MSG_TPIO_RESET, 0x00);		//GPIO mode
   mt_set_gpio_pull_enable(MSG_TPIO_RESET, 1);	//external pull up???
   mt_set_gpio_pull_select(MSG_TPIO_RESET, GPIO_PULL_UP);
   mt_set_gpio_dir(MSG_TPIO_RESET,1);
   mt_set_gpio_out(MSG_TPIO_RESET,1);
   msleep(300);
   mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
   mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
//    //200
   TPD_DEBUG("==========close cust eint touch panel reset\n");
//   mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

//ensure ctp can work after resume. edit by Magnum 2012-9-5
    int i = 0; int err = -1;
	 for(i;i<5;i++){
        err = msg2133_ts_get_fw_version();
        if (err == 1)
            break;
		else
            tpd_reset();
	 msleep(200);
    }
	ctp_suspend = 1;   // reset;
    return retval;
 }

static int tpd_suspend(struct i2c_client *client, pm_message_t message)
 {
    int retval = TPD_OK;

    TPD_DEBUG("================Magnum_TPD enter sleep\n");
#ifdef CTP_SIMULATE_PS
	TPD_DEBUG("Magnum ps_mode ==%d\n",ps_mode);
	if(ps_mode == 1){
			TPD_DEBUG("Magnum ps_mode ==%d, simulate PS,do not suspend!\n",ps_mode);
			 return TPD_OK;
	}
	else{
		    TPD_DEBUG("Magnum ps_mode ==%d, suspend!!!!\n",ps_mode);
	}
#endif
#ifdef TP_FIRMWARE_UPDATE
	if(update_switch==1)
	{
		return 0;
	}
#endif

   // mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);  // attention:  do not work ..
   TPD_DEBUG("Magnum Power Down suspend!!!!\n");
    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_CTP_EINT_PIN, GPIO_OUT_ZERO);
/*#ifdef TP_FIRMWARE_UPDATE
	if(update_switch==0)
#endif*/
	{
	    msleep(10);
	    hwPowerDown(MT65XX_POWER_LDO_VGP2, "TP");
	    mt_set_gpio_mode(MSG_TPIO_RESET, 0x00);		//GPIO mode
	    mt_set_gpio_pull_enable(MSG_TPIO_RESET, 1);	//external pull up???
	    mt_set_gpio_pull_select(MSG_TPIO_RESET, GPIO_PULL_DOWN);
		mt_set_gpio_dir(MSG_TPIO_RESET,1);
	    mt_set_gpio_out(MSG_TPIO_RESET,1);  //
	    ctp_suspend = 0;
  		TPD_DEBUG("Magnum suspend END....\n");
	}
    return retval;
 }

#ifdef CONFIG_HAS_EARLYSUSPEND
 static void tpd_early_suspend(struct early_suspend *handler)
 {
	 tpd_suspend(i2c_client, PMSG_SUSPEND);
 }

 static void tpd_late_resume(struct early_suspend *handler)
 {
	 tpd_resume(i2c_client);
 }
#endif

static struct tpd_driver_t tpd_device_driver = {
		.tpd_device_name = "MSG2133",
		.tpd_local_init = tpd_local_init,
		.suspend = tpd_early_suspend,
		.resume = tpd_late_resume,
#ifdef TPD_HAVE_BUTTON
		.tpd_have_button = 1,
#else
		.tpd_have_button = 0,
#endif
		.tpd_x_res = TPD_CUST_RES_X,
		.tpd_y_res = TPD_CUST_RES_Y,	//including button area
		.tpd_get_fw_version = msg2133_get_fw_version_stored,
		.tpd_get_vendor_version = msg2133_get_vendor_version_stored,
};
/* called when loaded into kernel */

 static int __init tpd_driver_init(void)
{
	TPD_DEBUG("MediaTek MSG2133 touch panel driver init\n");
	i2c_register_board_info(1, &msg2133_i2c_tpd, 1);
	if(tpd_driver_add(&tpd_device_driver) < 0)
	{
		TPD_DEBUG("add MSG2133 driver failed\n");
	}
	return 0;
}

/* should never be called */
static void __exit tpd_driver_exit(void)
{
	TPD_DEBUG("MediaTek MSG2133 touch panel driver exit\n");
	//input_unregister_device(tpd->dev);
	tpd_driver_remove(&tpd_device_driver);
}

module_init(tpd_driver_init);
module_exit(tpd_driver_exit);
