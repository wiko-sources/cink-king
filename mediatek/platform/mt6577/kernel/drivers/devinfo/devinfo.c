#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>



/***************************************************************************** 
* MODULE DEFINITION 
*****************************************************************************/

#define MODULE_NAME	    "[devinfo]"
#define DEV_NAME        "devmap"
#define MAJOR_DEV_NUM	196
 
/***************************************************************************** 
 * IOCTL DEFINITION 
 *****************************************************************************/
#define DEV_IOC_MAGIC       'd'
#define READ_DEV_DATA       _IOR(DEV_IOC_MAGIC,  1, unsigned int)

#define DEV_IOC_MAXNR       (10)

/***************************************************************************** 
* EXPORT DEFINITION 
*****************************************************************************/
extern u32 g_devinfo_data_size;
extern u32 get_devinfo_with_index(u32 index);



static struct cdev devinfo_cdev;
static dev_t devinfo_dev;
static int dev_open(struct inode *inode, struct file *filp);
static int dev_release(struct inode *inode, struct file *filp);
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int dev_mmap(struct file *filp, struct vm_area_struct *vma);

static struct file_operations mmap_fops = {        
    .open = dev_open,        
    .release = dev_release,       
    .mmap = dev_mmap,        	
    .unlocked_ioctl   = dev_ioctl,
    .owner = THIS_MODULE,
    };


static int dev_open(struct inode *inode, struct file *filp){
    return 0;
}

static int dev_release(struct inode *inode, struct file *filp){        
    return 0;
}


/************************************************************************** 
*  DEV DRIVER IOCTL 
**************************************************************************/ 
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u32 index = 0;
    int err   = 0;
    int ret   = 0;
    u32 data_size = g_devinfo_data_size;
    u32 data_read = 0;

    /* ---------------------------------- */
    /* IOCTL                              */
    /* ---------------------------------- */
    if (_IOC_TYPE(cmd) != DEV_IOC_MAGIC)
        return -ENOTTY;
    if (_IOC_NR(cmd) > DEV_IOC_MAXNR)
        return -ENOTTY;
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    if (err) return -EFAULT;
    
    switch (cmd) {

        /* ---------------------------------- */
        /* get dev info data                  */
        /* ---------------------------------- */
        case READ_DEV_DATA:
            //printk("%s CMD - READ_DEV_DATA\n",MODULE_NAME);
            if(copy_from_user((void *)&index, (void __user *)arg, sizeof(u32)))
            {              
                return -EFAULT;            
            }

            if (index < data_size){
                data_read = get_devinfo_with_index(index);
                ret = copy_to_user((void __user *)arg, (void *)&(data_read),sizeof(u32));
            }else{
                printk("%s Error! Data index larger than data size. index:%d, size:%d\n",MODULE_NAME, 
                    index, data_size);
                return -EFAULT;            
            }
        break;       
    }
    
    return 0;
}


/******************************************************************************
 * dev_mmap
 * 
 * DESCRIPTION: 
 *   mmap implementation for devinfo 
 * 
 * PARAMETERS: 
 *   filp: file pointer
 *   vma:virtual address range used to access the device
 * 
 * RETURNS: 
 *   0 for success.
 * 
 * NOTES: 
 *   None
 * 
 
******************************************************************************/
static int dev_mmap(struct file *filp, struct vm_area_struct *vma){
    int ret;
    long length = vma->vm_end - vma->vm_start;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    
    if ((ret = remap_pfn_range(vma,
                               vma->vm_start,
                               vma->vm_pgoff,
                               length,
                               vma->vm_page_prot)) < 0) {
        printk("[devinfo] map failed, ret:%d\n", ret);
        return ret;
    }
    return 0;
}




/******************************************************************************
 * devinfo_init
 * 
 * DESCRIPTION: 
 *   Init the device driver ! 
 * 
 * PARAMETERS: 
 *   None
 * 
 * RETURNS: 
 *   0 for success
 * 
 * NOTES: 
 *   None
 * 
 ******************************************************************************/
static int __init devinfo_init(void)
{
    int ret = 0;
    devinfo_dev = MKDEV(MAJOR_DEV_NUM, 0);

	printk("[devinfo] init\n");    

	ret = register_chrdev_region(devinfo_dev, 1, DEV_NAME );
	if (ret)    
	{        
	    printk("[%s] register device failed, ret:%d\n", MODULE_NAME, ret);
	    return ret;
    }

	/* initialize the device structure and register the device  */
    cdev_init(&devinfo_cdev, &mmap_fops);
    devinfo_cdev.owner = THIS_MODULE;
    if ((ret = cdev_add(&devinfo_cdev, devinfo_dev  , 1)) < 0) 
    {
        printk("[%s] could not allocate chrdev for the device, ret:%d\n", MODULE_NAME, ret);
        return ret;
    }
		
	return 0;
}

/******************************************************************************
 * devinfo_exit
 * 
 * DESCRIPTION: 
 *   Free the device driver ! 
 * 
 * PARAMETERS: 
 *   None
 * 
 * RETURNS: 
 *   None
 * 
 * NOTES: 
 *   None
 * 
 ******************************************************************************/
static void __exit devinfo_exit(void)
{
	cdev_del(&devinfo_cdev);        
	unregister_chrdev_region(devinfo_dev, 1);
}

module_init(devinfo_init);
module_exit(devinfo_exit);
