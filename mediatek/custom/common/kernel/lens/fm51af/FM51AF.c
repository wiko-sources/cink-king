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
#include "FM51AF.h"
#include "../camera/kd_camera_hw.h"

#define LENS_I2C_BUSNUM 1
static struct i2c_board_info __initdata kd_lens_dev={ I2C_BOARD_INFO("FM51AF", 0x19)};


#define FM51AF_DRVNAME "FM51AF"
#define FM51AF_VCM_WRITE_ID           0x18

#define FM51AF_DEBUG
#ifdef FM51AF_DEBUG
#define FM51AFDB printk
#else
#define FM51AFDB(x,...)
#endif

#ifdef CAMERA_SW_I2C_IMPL
#define TINNO_SW_I2C_IMPL
#endif
static spinlock_t g_FM51AF_SpinLock;

static struct i2c_client * g_pstFM51AF_I2Cclient = NULL;

static dev_t g_FM51AF_devno;
static struct cdev * g_pFM51AF_CharDrv = NULL;
static struct class *actuator_class = NULL;

static int  g_s4FM51AF_Opened = 0;
static long g_i4MotorStatus = 0;
static long g_i4Dir = 0;
static unsigned long g_u4FM51AF_INF = 0;
static unsigned long g_u4FM51AF_MACRO = 1023;
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

static int s4FM51AF_ReadReg(unsigned short * a_pu2Result)
{
    int  i4RetValue = 0;
    char pBuff[2];

#ifdef TINNO_SW_I2C_IMPL
    i4RetValue = SW_I2C_ReadReg(FM51AF_VCM_WRITE_ID, pBuff , 2);
#else    
    i4RetValue = i2c_master_recv(g_pstFM51AF_I2Cclient, pBuff , 2);
#endif

    if (i4RetValue < 0) 
    {
        FM51AFDB("[FM51AF] I2C read failed!! \n");
        return -1;
    }

    *a_pu2Result = (((u16)pBuff[0]) << 4) + (pBuff[1] >> 4);

    return 0;
}

static int s4FM51AF_WriteReg(u16 a_u2Data)
{
    int  i4RetValue = 0;

    char puSendCmd[2] = {(char)(a_u2Data >> 4) , (char)(((a_u2Data & 0xF) << 4)+g_sr)};

#ifdef TINNO_SW_I2C_IMPL
    i4RetValue = SW_I2C_WriteReg(FM51AF_VCM_WRITE_ID, puSendCmd, 2);
#else        
    //FM51AFDB("[FM51AF] g_sr %d, write %d \n", g_sr, a_u2Data);
    i4RetValue = i2c_master_send(g_pstFM51AF_I2Cclient, puSendCmd, 2);
#endif    
	
    if (i4RetValue < 0) 
    {
        FM51AFDB("[FM51AF] I2C send failed!! \n");
        return -1;
    }

    return 0;
}

inline static int getFM51AFInfo(__user stFM51AF_MotorInfo * pstMotorInfo)
{
    stFM51AF_MotorInfo stMotorInfo;
    stMotorInfo.u4MacroPosition   = g_u4FM51AF_MACRO;
    stMotorInfo.u4InfPosition     = g_u4FM51AF_INF;
    stMotorInfo.u4CurrentPosition = g_u4CurrPosition;
    stMotorInfo.bIsSupportSR      = TRUE;

	if (g_i4MotorStatus == 1)	{stMotorInfo.bIsMotorMoving = 1;}
	else						{stMotorInfo.bIsMotorMoving = 0;}

	if (g_s4FM51AF_Opened >= 1)	{stMotorInfo.bIsMotorOpen = 1;}
	else						{stMotorInfo.bIsMotorOpen = 0;}

    if(copy_to_user(pstMotorInfo , &stMotorInfo , sizeof(stFM51AF_MotorInfo)))
    {
        FM51AFDB("[FM51AF] copy to user failed when getting motor information \n");
    }

    return 0;
}

inline static int moveFM51AF(unsigned long a_u4Position)
{
    int ret = 0;
    
    if((a_u4Position > g_u4FM51AF_MACRO) || (a_u4Position < g_u4FM51AF_INF))
    {
        FM51AFDB("[FM51AF] out of range \n");
        return -EINVAL;
    }

    if (g_s4FM51AF_Opened == 1)
    {
        unsigned short InitPos;
        ret = s4FM51AF_ReadReg(&InitPos);
	    
        spin_lock(&g_FM51AF_SpinLock);
        if(ret == 0)
        {
            FM51AFDB("[FM51AF] Init Pos %6d \n", InitPos);
            g_u4CurrPosition = (unsigned long)InitPos;
        }
        else
        {		
            g_u4CurrPosition = 0;
        }
        g_s4FM51AF_Opened = 2;
        spin_unlock(&g_FM51AF_SpinLock);
    }

    if (g_u4CurrPosition < a_u4Position)
    {
        spin_lock(&g_FM51AF_SpinLock);	
        g_i4Dir = 1;
        spin_unlock(&g_FM51AF_SpinLock);	
    }
    else if (g_u4CurrPosition > a_u4Position)
    {
        spin_lock(&g_FM51AF_SpinLock);	
        g_i4Dir = -1;
        spin_unlock(&g_FM51AF_SpinLock);			
    }
    else										{return 0;}

    spin_lock(&g_FM51AF_SpinLock);    
    g_u4TargetPosition = a_u4Position;
    spin_unlock(&g_FM51AF_SpinLock);	

    //FM51AFDB("[FM51AF] move [curr] %d [target] %d\n", g_u4CurrPosition, g_u4TargetPosition);

            spin_lock(&g_FM51AF_SpinLock);
            g_sr = 3;
            g_i4MotorStatus = 0;
            spin_unlock(&g_FM51AF_SpinLock);	
		
            if(s4FM51AF_WriteReg((unsigned short)g_u4TargetPosition) == 0)
            {
                spin_lock(&g_FM51AF_SpinLock);		
                g_u4CurrPosition = (unsigned long)g_u4TargetPosition;
                spin_unlock(&g_FM51AF_SpinLock);				
            }
            else
            {
                FM51AFDB("[FM51AF] set I2C failed when moving the motor \n");			
                spin_lock(&g_FM51AF_SpinLock);
                g_i4MotorStatus = -1;
                spin_unlock(&g_FM51AF_SpinLock);				
            }

    return 0;
}

inline static int setFM51AFInf(unsigned long a_u4Position)
{
    spin_lock(&g_FM51AF_SpinLock);
    g_u4FM51AF_INF = a_u4Position;
    spin_unlock(&g_FM51AF_SpinLock);	
    return 0;
}

inline static int setFM51AFMacro(unsigned long a_u4Position)
{
    spin_lock(&g_FM51AF_SpinLock);
    g_u4FM51AF_MACRO = a_u4Position;
    spin_unlock(&g_FM51AF_SpinLock);	
    return 0;	
}

////////////////////////////////////////////////////////////////
static long FM51AF_Ioctl(
struct file * a_pstFile,
unsigned int a_u4Command,
unsigned long a_u4Param)
{
    long i4RetValue = 0;

    switch(a_u4Command)
    {
        case FM51AFIOC_G_MOTORINFO :
            i4RetValue = getFM51AFInfo((__user stFM51AF_MotorInfo *)(a_u4Param));
        break;

        case FM51AFIOC_T_MOVETO :
            i4RetValue = moveFM51AF(a_u4Param);
        break;
 
        case FM51AFIOC_T_SETINFPOS :
            i4RetValue = setFM51AFInf(a_u4Param);
        break;

        case FM51AFIOC_T_SETMACROPOS :
            i4RetValue = setFM51AFMacro(a_u4Param);
        break;
		
        default :
      	    FM51AFDB("[FM51AF] No CMD \n");
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
static int FM51AF_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    spin_lock(&g_FM51AF_SpinLock);

    if(g_s4FM51AF_Opened)
    {
        spin_unlock(&g_FM51AF_SpinLock);
        FM51AFDB("[FM51AF] the device is opened \n");
        return -EBUSY;
    }

    g_s4FM51AF_Opened = 1;
		
    spin_unlock(&g_FM51AF_SpinLock);

    return 0;
}

//Main jobs:
// 1.Deallocate anything that "open" allocated in private_data.
// 2.Shut down the device on last close.
// 3.Only called once on last time.
// Q1 : Try release multiple times.
static int FM51AF_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
    if (g_s4FM51AF_Opened)
    {
        FM51AFDB("[FM51AF] feee \n");
        g_sr = 5;
	    s4FM51AF_WriteReg(200);
        msleep(10);
	    s4FM51AF_WriteReg(100);
        msleep(10);
            	            	    	    
        spin_lock(&g_FM51AF_SpinLock);
        g_s4FM51AF_Opened = 0;
        spin_unlock(&g_FM51AF_SpinLock);

    }

    return 0;
}

static const struct file_operations g_stFM51AF_fops = 
{
    .owner = THIS_MODULE,
    .open = FM51AF_Open,
    .release = FM51AF_Release,
    .unlocked_ioctl = FM51AF_Ioctl
};

inline static int Register_FM51AF_CharDrv(void)
{
    struct device* vcm_device = NULL;

    //Allocate char driver no.
    if( alloc_chrdev_region(&g_FM51AF_devno, 0, 1,FM51AF_DRVNAME) )
    {
        FM51AFDB("[FM51AF] Allocate device no failed\n");

        return -EAGAIN;
    }

    //Allocate driver
    g_pFM51AF_CharDrv = cdev_alloc();

    if(NULL == g_pFM51AF_CharDrv)
    {
        unregister_chrdev_region(g_FM51AF_devno, 1);

        FM51AFDB("[FM51AF] Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //Attatch file operation.
    cdev_init(g_pFM51AF_CharDrv, &g_stFM51AF_fops);

    g_pFM51AF_CharDrv->owner = THIS_MODULE;

    //Add to system
    if(cdev_add(g_pFM51AF_CharDrv, g_FM51AF_devno, 1))
    {
        FM51AFDB("[FM51AF] Attatch file operation failed\n");

        unregister_chrdev_region(g_FM51AF_devno, 1);

        return -EAGAIN;
    }

    actuator_class = class_create(THIS_MODULE, "actuatordrv4");
    if (IS_ERR(actuator_class)) {
        int ret = PTR_ERR(actuator_class);
        FM51AFDB("Unable to create class, err = %d\n", ret);
        return ret;            
    }

    vcm_device = device_create(actuator_class, NULL, g_FM51AF_devno, NULL, FM51AF_DRVNAME);

    if(NULL == vcm_device)
    {
        return -EIO;
    }
    
    return 0;
}

inline static void Unregister_FM51AF_CharDrv(void)
{
    //Release char driver
    cdev_del(g_pFM51AF_CharDrv);

    unregister_chrdev_region(g_FM51AF_devno, 1);
    
    device_destroy(actuator_class, g_FM51AF_devno);

    class_destroy(actuator_class);
}

//////////////////////////////////////////////////////////////////////

static int FM51AF_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int FM51AF_i2c_remove(struct i2c_client *client);
static const struct i2c_device_id FM51AF_i2c_id[] = {{FM51AF_DRVNAME,0},{}};   
struct i2c_driver FM51AF_i2c_driver = {                       
    .probe = FM51AF_i2c_probe,                                   
    .remove = FM51AF_i2c_remove,                           
    .driver.name = FM51AF_DRVNAME,                 
    .id_table = FM51AF_i2c_id,                             
};  

#if 0 
static int FM51AF_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) {         
    strcpy(info->type, FM51AF_DRVNAME);                                                         
    return 0;                                                                                       
}      
#endif 
static int FM51AF_i2c_remove(struct i2c_client *client) {
    return 0;
}

/* Kirby: add new-style driver {*/
static int FM51AF_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int i4RetValue = 0;

    FM51AFDB("[FM51AF] Attach I2C \n");

    /* Kirby: add new-style driver { */
    g_pstFM51AF_I2Cclient = client;
    
    g_pstFM51AF_I2Cclient->addr = g_pstFM51AF_I2Cclient->addr >> 1;
    
    //Register char driver
    i4RetValue = Register_FM51AF_CharDrv();

    if(i4RetValue){

        FM51AFDB("[FM51AF] register char device failed!\n");

        return i4RetValue;
    }

    spin_lock_init(&g_FM51AF_SpinLock);

    FM51AFDB("[FM51AF] Attached!! \n");

    return 0;
}

static int FM51AF_probe(struct platform_device *pdev)
{
    return i2c_add_driver(&FM51AF_i2c_driver);
}

static int FM51AF_remove(struct platform_device *pdev)
{
    i2c_del_driver(&FM51AF_i2c_driver);
    return 0;
}

static int FM51AF_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

static int FM51AF_resume(struct platform_device *pdev)
{
    return 0;
}

// platform structure
static struct platform_driver g_stFM51AF_Driver = {
    .probe		= FM51AF_probe,
    .remove	= FM51AF_remove,
    .suspend	= FM51AF_suspend,
    .resume	= FM51AF_resume,
    .driver		= {
        .name	= "lens_actuator4",
        .owner	= THIS_MODULE,
    }
};

static struct platform_device actuator_dev4 = {
	.name		  = "lens_actuator4",
	.id		  = -1,
};

static int __init FM51AF_i2C_init(void)
{
    i2c_register_board_info(LENS_I2C_BUSNUM, &kd_lens_dev, 1);
	platform_device_register(&actuator_dev4);	
    if(platform_driver_register(&g_stFM51AF_Driver)){
        FM51AFDB("failed to register FM51AF driver\n");
        return -ENODEV;
    }

    return 0;
}

static void __exit FM51AF_i2C_exit(void)
{
	platform_driver_unregister(&g_stFM51AF_Driver);
}

module_init(FM51AF_i2C_init);
module_exit(FM51AF_i2C_exit);

MODULE_DESCRIPTION("FM51AF lens module driver");
MODULE_AUTHOR("KY Chen <ky.chen@Mediatek.com>");
MODULE_LICENSE("GPL");


