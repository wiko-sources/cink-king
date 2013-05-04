/*
 * MD218A voice coil motor driver
 *
 *
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include "FM52AF.h"
#include "../camera/kd_camera_hw.h"

#define LENS_I2C_BUSNUM 1
static struct i2c_board_info __initdata kd_lens_dev={ I2C_BOARD_INFO("FM52AF", 0x1A)};


#define FM52AF_DRVNAME "FM52AF"
#define FM52AF_VCM_WRITE_ID           0x18

#define FM52AF_DEBUG
#ifdef FM52AF_DEBUG
#define FM52AFDB printk
#else
#define FM52AFDB(x,...)
#endif

#ifdef CAMERA_SW_I2C_IMPL
#define TINNO_SW_I2C_IMPL
#endif
static spinlock_t g_FM52AF_SpinLock;

static struct i2c_client * g_pstFM52AF_I2Cclient = NULL;

static dev_t g_FM52AF_devno;
static struct cdev * g_pFM52AF_CharDrv = NULL;
static struct class *actuator_class = NULL;

static int  g_s4FM52AF_Opened = 0;
static long g_i4MotorStatus = 0;
static long g_i4Dir = 0;
static unsigned long g_u4FM52AF_INF = 0;
static unsigned long g_u4FM52AF_MACRO = 1023;
static unsigned long g_u4TargetPosition = 0;
static unsigned long g_u4CurrPosition   = 0;

static int g_sr = 3;

extern s32 mt_set_gpio_mode(u32 u4Pin, u32 u4Mode);
extern s32 mt_set_gpio_out(u32 u4Pin, u32 u4PinOut);
extern s32 mt_set_gpio_dir(u32 u4Pin, u32 u4Dir);

#ifdef TINNO_SW_I2C_IMPL
extern int SW_I2C_ReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 size);
extern int SW_I2C_WriteReg(u16 a_u2Addr , u8* a_u4Data , u32 a_u4Bytes);
#endif

static int s4FM52AF_ReadReg(unsigned short * a_pu2Result)
{
    int  i4RetValue = 0;
    char pBuff[2];

#ifdef TINNO_SW_I2C_IMPL
    i4RetValue = SW_I2C_ReadReg(FM52AF_VCM_WRITE_ID, pBuff , 2);
#else    
    i4RetValue = i2c_master_recv(g_pstFM52AF_I2Cclient, pBuff , 2);
#endif

    if (i4RetValue < 0) 
    {
        FM52AFDB("[FM52AF] I2C read failed!! \n");
        return -1;
    }

    *a_pu2Result = (((u16)pBuff[0]) << 4) + (pBuff[1] >> 4);

    return 0;
}

static int s4FM52AF_WriteReg(u16 a_u2Data)
{
    int  i4RetValue = 0;

    char puSendCmd[2] = {(char)(a_u2Data >> 4) , (char)(((a_u2Data & 0xF) << 4)+g_sr)};

#ifdef TINNO_SW_I2C_IMPL
    i4RetValue = SW_I2C_WriteReg(FM52AF_VCM_WRITE_ID, puSendCmd, 2);
#else        
    //FM52AFDB("[FM52AF] g_sr %d, write %d \n", g_sr, a_u2Data);
    i4RetValue = i2c_master_send(g_pstFM52AF_I2Cclient, puSendCmd, 2);
#endif    
	
    if (i4RetValue < 0) 
    {
        FM52AFDB("[FM52AF] I2C send failed!! \n");
        return -1;
    }

    return 0;
}

inline static int getFM52AFInfo(__user stFM52AF_MotorInfo * pstMotorInfo)
{
    stFM52AF_MotorInfo stMotorInfo;
    stMotorInfo.u4MacroPosition   = g_u4FM52AF_MACRO;
    stMotorInfo.u4InfPosition     = g_u4FM52AF_INF;
    stMotorInfo.u4CurrentPosition = g_u4CurrPosition;
    stMotorInfo.bIsSupportSR      = TRUE;

	if (g_i4MotorStatus == 1)	{stMotorInfo.bIsMotorMoving = 1;}
	else						{stMotorInfo.bIsMotorMoving = 0;}

	if (g_s4FM52AF_Opened >= 1)	{stMotorInfo.bIsMotorOpen = 1;}
	else						{stMotorInfo.bIsMotorOpen = 0;}

    if(copy_to_user(pstMotorInfo , &stMotorInfo , sizeof(stFM52AF_MotorInfo)))
    {
        FM52AFDB("[FM52AF] copy to user failed when getting motor information \n");
    }

    return 0;
}

inline static int moveFM52AF(unsigned long a_u4Position)
{
    int ret = 0;
    
    if((a_u4Position > g_u4FM52AF_MACRO) || (a_u4Position < g_u4FM52AF_INF))
    {
        FM52AFDB("[FM52AF] out of range \n");
        return -EINVAL;
    }

    if (g_s4FM52AF_Opened == 1)
    {
        unsigned short InitPos;
        ret = s4FM52AF_ReadReg(&InitPos);
	    
        spin_lock(&g_FM52AF_SpinLock);
        if(ret == 0)
        {
            FM52AFDB("[FM52AF] Init Pos %6d \n", InitPos);
            g_u4CurrPosition = (unsigned long)InitPos;
        }
        else
        {		
            g_u4CurrPosition = 0;
        }
        g_s4FM52AF_Opened = 2;
        spin_unlock(&g_FM52AF_SpinLock);
    }

    if (g_u4CurrPosition < a_u4Position)
    {
        spin_lock(&g_FM52AF_SpinLock);	
        g_i4Dir = 1;
        spin_unlock(&g_FM52AF_SpinLock);	
    }
    else if (g_u4CurrPosition > a_u4Position)
    {
        spin_lock(&g_FM52AF_SpinLock);	
        g_i4Dir = -1;
        spin_unlock(&g_FM52AF_SpinLock);			
    }
    else										{return 0;}

    spin_lock(&g_FM52AF_SpinLock);    
    g_u4TargetPosition = a_u4Position;
    spin_unlock(&g_FM52AF_SpinLock);	

    //FM52AFDB("[FM52AF] move [curr] %d [target] %d\n", g_u4CurrPosition, g_u4TargetPosition);

            spin_lock(&g_FM52AF_SpinLock);
            g_sr = 3;
            g_i4MotorStatus = 0;
            spin_unlock(&g_FM52AF_SpinLock);	
		
            if(s4FM52AF_WriteReg((unsigned short)g_u4TargetPosition) == 0)
            {
                spin_lock(&g_FM52AF_SpinLock);		
                g_u4CurrPosition = (unsigned long)g_u4TargetPosition;
                spin_unlock(&g_FM52AF_SpinLock);				
            }
            else
            {
                FM52AFDB("[FM52AF] set I2C failed when moving the motor \n");			
                spin_lock(&g_FM52AF_SpinLock);
                g_i4MotorStatus = -1;
                spin_unlock(&g_FM52AF_SpinLock);				
            }

    return 0;
}

inline static int setFM52AFInf(unsigned long a_u4Position)
{
    spin_lock(&g_FM52AF_SpinLock);
    g_u4FM52AF_INF = a_u4Position;
    spin_unlock(&g_FM52AF_SpinLock);	
    return 0;
}

inline static int setFM52AFMacro(unsigned long a_u4Position)
{
    spin_lock(&g_FM52AF_SpinLock);
    g_u4FM52AF_MACRO = a_u4Position;
    spin_unlock(&g_FM52AF_SpinLock);	
    return 0;	
}

////////////////////////////////////////////////////////////////
static long FM52AF_Ioctl(
struct file * a_pstFile,
unsigned int a_u4Command,
unsigned long a_u4Param)
{
    long i4RetValue = 0;

    switch(a_u4Command)
    {
        case FM52AFIOC_G_MOTORINFO :
            i4RetValue = getFM52AFInfo((__user stFM52AF_MotorInfo *)(a_u4Param));
        break;

        case FM52AFIOC_T_MOVETO :
            i4RetValue = moveFM52AF(a_u4Param);
        break;
 
        case FM52AFIOC_T_SETINFPOS :
            i4RetValue = setFM52AFInf(a_u4Param);
        break;

        case FM52AFIOC_T_SETMACROPOS :
            i4RetValue = setFM52AFMacro(a_u4Param);
        break;
		
        default :
      	    FM52AFDB("[FM52AF] No CMD \n");
            i4RetValue = -EPERM;
        break;
    }

    return i4RetValue;
}

//Main jobs:
// 1.check for device-specified errors, device not ready.
// 2.Initialize the device if it is opened for the first time.
// 3.Update f_op pointer.
// 4.Fill data structures into private_data
//CAM_RESET
static int FM52AF_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    spin_lock(&g_FM52AF_SpinLock);

    if(g_s4FM52AF_Opened)
    {
        spin_unlock(&g_FM52AF_SpinLock);
        FM52AFDB("[FM52AF] the device is opened \n");
        return -EBUSY;
    }

    g_s4FM52AF_Opened = 1;
		
    spin_unlock(&g_FM52AF_SpinLock);

    return 0;
}

//Main jobs:
// 1.Deallocate anything that "open" allocated in private_data.
// 2.Shut down the device on last close.
// 3.Only called once on last time.
// Q1 : Try release multiple times.
static int FM52AF_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
    if (g_s4FM52AF_Opened)
    {
        FM52AFDB("[FM52AF] feee \n");
        g_sr = 5;
	    s4FM52AF_WriteReg(200);
        msleep(10);
	    s4FM52AF_WriteReg(100);
        msleep(10);
            	            	    	    
        spin_lock(&g_FM52AF_SpinLock);
        g_s4FM52AF_Opened = 0;
        spin_unlock(&g_FM52AF_SpinLock);

    }

    return 0;
}

static const struct file_operations g_stFM52AF_fops = 
{
    .owner = THIS_MODULE,
    .open = FM52AF_Open,
    .release = FM52AF_Release,
    .unlocked_ioctl = FM52AF_Ioctl
};

inline static int Register_FM52AF_CharDrv(void)
{
    struct device* vcm_device = NULL;

    //Allocate char driver no.
    if( alloc_chrdev_region(&g_FM52AF_devno, 0, 1,FM52AF_DRVNAME) )
    {
        FM52AFDB("[FM52AF] Allocate device no failed\n");

        return -EAGAIN;
    }

    //Allocate driver
    g_pFM52AF_CharDrv = cdev_alloc();

    if(NULL == g_pFM52AF_CharDrv)
    {
        unregister_chrdev_region(g_FM52AF_devno, 1);

        FM52AFDB("[FM52AF] Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //Attatch file operation.
    cdev_init(g_pFM52AF_CharDrv, &g_stFM52AF_fops);

    g_pFM52AF_CharDrv->owner = THIS_MODULE;

    //Add to system
    if(cdev_add(g_pFM52AF_CharDrv, g_FM52AF_devno, 1))
    {
        FM52AFDB("[FM52AF] Attatch file operation failed\n");

        unregister_chrdev_region(g_FM52AF_devno, 1);

        return -EAGAIN;
    }

    actuator_class = class_create(THIS_MODULE, "actuatordrv5");
    if (IS_ERR(actuator_class)) {
        int ret = PTR_ERR(actuator_class);
        FM52AFDB("Unable to create class, err = %d\n", ret);
        return ret;            
    }

    vcm_device = device_create(actuator_class, NULL, g_FM52AF_devno, NULL, FM52AF_DRVNAME);

    if(NULL == vcm_device)
    {
        return -EIO;
    }
    
    return 0;
}

inline static void Unregister_FM52AF_CharDrv(void)
{
    //Release char driver
    cdev_del(g_pFM52AF_CharDrv);

    unregister_chrdev_region(g_FM52AF_devno, 1);
    
    device_destroy(actuator_class, g_FM52AF_devno);

    class_destroy(actuator_class);
}

//////////////////////////////////////////////////////////////////////

static int FM52AF_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int FM52AF_i2c_remove(struct i2c_client *client);
static const struct i2c_device_id FM52AF_i2c_id[] = {{FM52AF_DRVNAME,0},{}};   
struct i2c_driver FM52AF_i2c_driver = {                       
    .probe = FM52AF_i2c_probe,                                   
    .remove = FM52AF_i2c_remove,                           
    .driver.name = FM52AF_DRVNAME,                 
    .id_table = FM52AF_i2c_id,                             
};  

#if 0 
static int FM52AF_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) {         
    strcpy(info->type, FM52AF_DRVNAME);                                                         
    return 0;                                                                                       
}      
#endif 
static int FM52AF_i2c_remove(struct i2c_client *client) {
    return 0;
}

/* Kirby: add new-style driver {*/
static int FM52AF_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int i4RetValue = 0;

    FM52AFDB("[FM52AF] Attach I2C \n");

    /* Kirby: add new-style driver { */
    g_pstFM52AF_I2Cclient = client;
    
    //g_pstFM52AF_I2Cclient->addr = g_pstFM52AF_I2Cclient->addr >> 1;
    g_pstFM52AF_I2Cclient->addr = (FM52AF_VCM_WRITE_ID) >> 1;
    
    //Register char driver
    i4RetValue = Register_FM52AF_CharDrv();

    if(i4RetValue){

        FM52AFDB("[FM52AF] register char device failed!\n");

        return i4RetValue;
    }

    spin_lock_init(&g_FM52AF_SpinLock);

    FM52AFDB("[FM52AF] Attached!! \n");

    return 0;
}

static int FM52AF_probe(struct platform_device *pdev)
{
    return i2c_add_driver(&FM52AF_i2c_driver);
}

static int FM52AF_remove(struct platform_device *pdev)
{
    i2c_del_driver(&FM52AF_i2c_driver);
    return 0;
}

static int FM52AF_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

static int FM52AF_resume(struct platform_device *pdev)
{
    return 0;
}

// platform structure
static struct platform_driver g_stFM52AF_Driver = {
    .probe		= FM52AF_probe,
    .remove	= FM52AF_remove,
    .suspend	= FM52AF_suspend,
    .resume	= FM52AF_resume,
    .driver		= {
        .name	= "lens_actuator5",
        .owner	= THIS_MODULE,
    }
};

static struct platform_device actuator_dev5 = {
	.name		  = "lens_actuator5",
	.id		  = -1,
};

static int __init FM52AF_i2C_init(void)
{
    i2c_register_board_info(LENS_I2C_BUSNUM, &kd_lens_dev, 1);
	platform_device_register(&actuator_dev5);	
    if(platform_driver_register(&g_stFM52AF_Driver)){
        FM52AFDB("failed to register FM52AF driver\n");
        return -ENODEV;
    }

    return 0;
}

static void __exit FM52AF_i2C_exit(void)
{
	platform_driver_unregister(&g_stFM52AF_Driver);
}

module_init(FM52AF_i2C_init);
module_exit(FM52AF_i2C_exit);

MODULE_DESCRIPTION("FM52AF lens module driver");
MODULE_AUTHOR("KY Chen <ky.chen@Mediatek.com>");
MODULE_LICENSE("GPL");


