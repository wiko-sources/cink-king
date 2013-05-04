#include <linux/thermal.h>
#include <linux/xlog.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include "wmt_tm.h"

#include <mach/mtk_thermal_monitor.h>

#if  defined(CONFIG_THERMAL) &&  defined(CONFIG_THERMAL_OPEN)

static int wmt_tm_debug_log = 0;
#define wmt_tm_dprintk(fmt, args...)   \
do {                                    \
    if (wmt_tm_debug_log) {                \
        xlog_printk(ANDROID_LOG_INFO, "Power/WMT_Thermal", fmt, ##args); \
    }                                   \
} while(0)

struct linux_thermal_ctrl_if { 
    int kernel_mode;
    int interval;
    struct thermal_zone_device *thz_dev;
    struct thermal_cooling_device *cl_dev;
};

struct wmt_thermal_ctrl_if {
    struct wmt_thermal_ctrl_ops ops;
}; 

typedef struct wmt_tm {
   struct linux_thermal_ctrl_if linux_if;
   struct wmt_thermal_ctrl_if   wmt_if; 
}wmt_tm_t;    

static unsigned int cl_dev_state =0;
static unsigned int trip_0_temp = 85000;
wmt_tm_t g_wmt_tm; 
wmt_tm_t *pg_wmt_tm = &g_wmt_tm;

static int wmt_thz_bind(struct thermal_zone_device *,
		     struct thermal_cooling_device *);
static int wmt_thz_unbind(struct thermal_zone_device *,
		     struct thermal_cooling_device *);
static int wmt_thz_get_temp(struct thermal_zone_device *, 
             unsigned long *);
static int wmt_thz_get_mode(struct thermal_zone_device *,
			 enum thermal_device_mode *);
static int wmt_thz_set_mode(struct thermal_zone_device *,
		     enum thermal_device_mode);
static int wmt_thz_get_trip_type(struct thermal_zone_device *, int,
		     enum thermal_trip_type *);
static int wmt_thz_get_trip_temp(struct thermal_zone_device *, int,
			 unsigned long *);
static int wmt_thz_get_crit_temp(struct thermal_zone_device *, 
             unsigned long *);
static int wmt_cl_get_max_state(struct thermal_cooling_device *, 
             unsigned long *);
static int wmt_cl_get_cur_state(struct thermal_cooling_device *, 
             unsigned long *);
static int wmt_cl_set_cur_state(struct thermal_cooling_device *, 
             unsigned long);

static struct thermal_zone_device_ops wmt_thz_dev_ops = {
	.bind = wmt_thz_bind,
	.unbind = wmt_thz_unbind,
	.get_temp = wmt_thz_get_temp,
	.get_mode = wmt_thz_get_mode,
	.set_mode = wmt_thz_set_mode,
	.get_trip_type = wmt_thz_get_trip_type,
	.get_trip_temp = wmt_thz_get_trip_temp,
	.get_crit_temp = wmt_thz_get_crit_temp,
};

static struct thermal_cooling_device_ops mtktspa_cooling_sysrst_ops = {
	.get_max_state = wmt_cl_get_max_state,
	.get_cur_state = wmt_cl_get_cur_state,
	.set_cur_state = wmt_cl_set_cur_state,
};

static int wmt_thz_bind(struct thermal_zone_device *thz_dev,
		     struct thermal_cooling_device *cool_dev)
{
    struct linux_thermal_ctrl_if *p_linux_if = 0;
//    int    table_val = 0;
    
    wmt_tm_dprintk("[wmt_thz_bind] \n");

    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else 
    {
       return -EINVAL;
    }
  
    /* cooling devices */
    if (cool_dev != p_linux_if->cl_dev)
    {
        return 0;
    }
 
    if (mtk_thermal_zone_bind_cooling_device(thz_dev, 0/*table_val*/, cool_dev)) {
        wmt_tm_dprintk("[wmt_thz_bind] wmt_thz_bind\n");
        return -EINVAL;
    } else {
        wmt_tm_dprintk("[wmt_thz_bind] binding OK\n");
    }

    return 0;

}
static int wmt_thz_unbind(struct thermal_zone_device *thz_dev,
		     struct thermal_cooling_device *cool_dev)
{
    struct linux_thermal_ctrl_if *p_linux_if = 0;
//    int    table_val = 0;

    wmt_tm_dprintk("[wmt_thz_unbind] \n");

    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else 
    {
       return -EINVAL;
    }
#if 0
    /* cooling devices */
    if (cool_dev == p_linux_if->cl_dev)
    {
        table_val= 0;
    }
    else
    {
        wmt_tm_dprintk("[wmt_thz_unbind] unbind device fail..!\n");
        return -EINVAL;
    }
#endif

    if (thermal_zone_unbind_cooling_device(thz_dev, 0 /*table_val*/, cool_dev)) {
	    wmt_tm_dprintk("[wmt_thz_unbind] error unbinding cooling dev\n");
		return -EINVAL;
	} else {
	    wmt_tm_dprintk("[wmt_thz_unbind] unbinding OK\n");
    }

	return 0;
}

static int wmt_thz_get_temp(struct thermal_zone_device *thz_dev, 
             unsigned long *pv)
{
    struct wmt_thermal_ctrl_ops *p_des;
    int temp = 0;

    *pv = 0;
    if(pg_wmt_tm)
    {
        p_des = &pg_wmt_tm->wmt_if.ops;
        temp = p_des->query_temp();

        //temp = ((temp & 0x80) == 0x0)?temp:(-1)*temp ;
        temp = ((temp & 0x80) == 0x0)?temp:(-1)*(temp & 0x7f);
        *pv =  temp*1000;

        wmt_tm_dprintk("[wmt_thz_get_temp] ... temp = %d\n", temp);
    }
    
    return 0;
}

static int wmt_thz_get_mode(struct thermal_zone_device *thz_dev,
			 enum thermal_device_mode *mode)
{
    struct linux_thermal_ctrl_if *p_linux_if = 0;
//    int    kernel_mode = 0;

    wmt_tm_dprintk("[wmt_thz_unbind] \n");

    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else
    {
       wmt_tm_dprintk("[wmt_thz_unbind] fail! \n"); 
       return -EINVAL; 
    }
    
    wmt_tm_dprintk("[wmt_thz_get_mode] %d\n", p_linux_if->kernel_mode);

    *mode = (p_linux_if->kernel_mode) ? THERMAL_DEVICE_ENABLED
			     : THERMAL_DEVICE_DISABLED;

	return 0;
}

static int wmt_thz_set_mode(struct thermal_zone_device *thz_dev,
		     enum thermal_device_mode mode)
{
    struct linux_thermal_ctrl_if *p_linux_if = 0;
//    int    kernel_mode = 0;

    wmt_tm_dprintk("[wmt_thz_unbind] \n");

    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else
    {
       wmt_tm_dprintk("[wmt_thz_unbind] fail! \n"); 
       return -EINVAL; 
    }

    wmt_tm_dprintk("[mtktspa_set_mode] %d\n", mode);

    p_linux_if->kernel_mode = mode;
    
	return 0;

}

static int wmt_thz_get_trip_type(struct thermal_zone_device *thz_dev, int trip,
		     enum thermal_trip_type *type)
{
    wmt_tm_dprintk("[mtktspa_get_trip_type] %d\n", trip);

    if (trip == 0)
    	*type = THERMAL_TRIP_ACTIVE;
    
	return 0;
}

static int wmt_thz_get_trip_temp(struct thermal_zone_device *thz_dev, int trip,
			 unsigned long *pv)
{
    wmt_tm_dprintk("[mtktspa_get_trip_temp] %d\n", trip);

    if (trip == 0)
    	*pv = trip_0_temp;
    
	return 0;
}

static int wmt_thz_get_crit_temp(struct thermal_zone_device *thz_dev, 
             unsigned long *pv)
{
    wmt_tm_dprintk("[mtktspa_get_crit_temp] \n");
#define WMT_TM_TEMP_CRIT 85000 /* 85.000 degree Celsius */
    *pv = WMT_TM_TEMP_CRIT;

    return 0;
}

static int wmt_cl_get_max_state(struct thermal_cooling_device *cool_dev, 
             unsigned long *pv)
{
    *pv = 1;    
    wmt_tm_dprintk("[wmt_cl_get_max_state] %d\n", *pv);
	return 0;
}

static int wmt_cl_get_cur_state(struct thermal_cooling_device *cool_dev, 
             unsigned long *pv)
{
    *pv = cl_dev_state;
    wmt_tm_dprintk("[wmt_cl_get_cur_state] %d\n", *pv);
	return 0;
}

static int wmt_cl_set_cur_state(struct thermal_cooling_device *cool_dev, 
             unsigned long v)
{
    wmt_tm_dprintk("[wmt_cl_set_cur_state] %d\n", v);    
    cl_dev_state = v;

    if(cl_dev_state == 1)
    {
        //the temperature is over than the critical, system reboot.
	BUG();
    }
    return 0;
}

static int wmt_tm_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *p = buf;
    struct linux_thermal_ctrl_if *p_linux_if = 0;

    wmt_tm_dprintk("[wmt_thz_unbind] \n");

    //sanity
    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else
    {
       wmt_tm_dprintk("[wmt_thz_unbind] fail! \n"); 
       return -EINVAL; 
    }
    
    p += sprintf(p, "[wmt_tm_read] trip_0_temp=%d,time_ms=%d\n", 
        trip_0_temp, p_linux_if->interval*1000);
    
    *start = buf + off;
    
    len = p - buf;
    if (len > off)
        len -= off;
    else
        len = 0;
    
    return len < count ? len  : count;
}

static ssize_t wmt_tm_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len=0,trip_0=0,time_msec=0;
    char desc[32];

    struct linux_thermal_ctrl_if *p_linux_if = 0;

    wmt_tm_dprintk("[wmt_thz_write] \n");

    //sanity
    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else
    {
       wmt_tm_dprintk("[wmt_thz_unbind] fail! \n"); 
       return -EINVAL; 
    }

    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%d %d", &trip_0, &time_msec) == 2)
    {
        // unregister
        if (p_linux_if->thz_dev) {
    		mtk_thermal_zone_device_unregister(p_linux_if->thz_dev);
    		p_linux_if->thz_dev = NULL;
    	}

        trip_0_temp = trip_0;        
        
        p_linux_if->interval = time_msec / 1000;
        //p_linux_if->thz_dev->polling_delay = p_linux_if->interval*1000;
        
	    //thermal_zone_device_update(p_linux_if->thz_dev);

	    // register
	    p_linux_if->thz_dev = mtk_thermal_zone_device_register("mtktswmt", 1, NULL,
                                &wmt_thz_dev_ops, 0, 0, 0, p_linux_if->interval*1000);
        
        wmt_tm_dprintk("[wmt_tm_write] time_ms=%d\n", p_linux_if->interval*1000);
        
        return count;
    }
    else
    {
        wmt_tm_dprintk("[wmt_tm_write] bad argument\n");
    }
    
    return -EINVAL;
}

static int wmt_tm_proc_register(void)
{
    struct proc_dir_entry *entry = NULL;
    struct proc_dir_entry *wmt_tm_proc_dir = NULL;

    wmt_tm_dprintk("[wmt_tm_proc_register] \n");

    wmt_tm_proc_dir = proc_mkdir("wmt_tm", NULL);
    if (!wmt_tm_proc_dir)
    {
        wmt_tm_dprintk("[wmt_tm_proc_register]: mkdir /proc/wmt_tm failed\n");
    }
    else
    {
        entry = create_proc_entry("wmt_tm", S_IRUGO | S_IWUSR, wmt_tm_proc_dir);
        if (entry)
        {
            entry->read_proc  =  wmt_tm_read;
            entry->write_proc =  wmt_tm_write;
        }
    }    
    return 0;
}

static int wmt_tm_proc_unregister(void)
{
    wmt_tm_dprintk("[wmt_tm_proc_unregister] \n");
	return 0;
}

static int wmt_tm_thz_cl_register(void)
{
    #define DEFAULT_POLL_TIME 0 //1000
    
    struct linux_thermal_ctrl_if *p_linux_if = 0;

    wmt_tm_dprintk("[wmt_tm_thz_cl_register] \n");

    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else 
    {
       return -1;
    }
    
    /* cooling devices */
    p_linux_if->cl_dev = mtk_thermal_cooling_device_register("mtktswmt-sysrst", NULL,
    &mtktspa_cooling_sysrst_ops);

    /* trips */
    p_linux_if->thz_dev = mtk_thermal_zone_device_register("mtktswmt", 1, NULL,
    &wmt_thz_dev_ops, 0, 0, 0, DEFAULT_POLL_TIME);

    return 0;
}

static int wmt_tm_thz_cl_unregister(void)
{
    struct linux_thermal_ctrl_if *p_linux_if = 0;

    wmt_tm_dprintk("[wmt_tm_thz_cl_unregister] \n");

    if(pg_wmt_tm)
    {
       p_linux_if = &pg_wmt_tm->linux_if;
    }
    else 
    {
       return -1;
    }
    
    if (p_linux_if->cl_dev) {
		mtk_thermal_cooling_device_unregister(p_linux_if->cl_dev);
		p_linux_if->cl_dev = NULL;
	}

	if (p_linux_if->thz_dev) {
		mtk_thermal_zone_device_unregister(p_linux_if->thz_dev);
		p_linux_if->thz_dev = NULL;
	}

    return 0;
}

static int wmt_tm_ops_register(struct wmt_thermal_ctrl_ops *ops)
{
    struct wmt_thermal_ctrl_ops *p_des;

    wmt_tm_dprintk("[wmt_tm_ops_register] \n");
    
    if(pg_wmt_tm)
    {
#if 1    
        p_des = &pg_wmt_tm->wmt_if.ops;
        if(ops!=NULL){
            wmt_tm_dprintk("[wmt_tm_ops_register] reg start ...\n");
            p_des->query_temp = ops->query_temp;
            p_des->set_temp = ops->set_temp;
            wmt_tm_dprintk("[wmt_tm_ops_register] reg end ...\n");
        }else {
            p_des->query_temp =  0;
            p_des->set_temp = 0;
        }
#endif        
        return 0;
    }
    else 
    {
        return -1;
    }
}

static int wmt_tm_ops_unregister(void)
{
    struct wmt_thermal_ctrl_ops *p_des;

    wmt_tm_dprintk("[wmt_tm_ops_register] \n");
    
    if(pg_wmt_tm)
    {
        p_des = &pg_wmt_tm->wmt_if.ops;
        p_des->query_temp = 0;
        p_des->set_temp = 0;
        
        return 0;
    }
    else 
    {
        return -1;
    }

    return 0;
}

int wmt_tm_init(struct wmt_thermal_ctrl_ops *ops)
{
    int err = 0;

    wmt_tm_dprintk("[wmt_tm_init] start -->\n");

    err = wmt_tm_ops_register(ops);
    if(err)
        return err;

    err = wmt_tm_proc_register();
    if(err)
        return err;
#if 0    
    err = wmt_tm_thz_cl_register();
    if(err)
        return err;
#endif
    wmt_tm_dprintk("[wmt_tm_init] end <--\n");

    return 0;
}

int wmt_tm_init_rt()
{
    int err = 0;

    wmt_tm_dprintk("[wmt_tm_init_rt] start -->\n");
    
    err = wmt_tm_thz_cl_register();
    if(err)
        return err;

    wmt_tm_dprintk("[wmt_tm_init_rt] end <--\n");

    return 0;
}

int wmt_tm_deinit_rt()
{
    int err = 0;

    wmt_tm_dprintk("[wmt_tm_deinit_rt] start -->\n");
    
    err = wmt_tm_thz_cl_unregister();
    if(err)
        return err;

    wmt_tm_dprintk("[wmt_tm_deinit_rt] end <--\n");

    return 0;
}

int wmt_tm_deinit()
{
    int err = 0;

    wmt_tm_dprintk("[wmt_tm_deinit] \n");
#if 0
    err = wmt_tm_thz_cl_unregister();
    if(err)
        return err;
#endif
    err = wmt_tm_proc_unregister();
    if(err)
        return err;

    err = wmt_tm_ops_unregister();
    if(err)
        return err;
   
    return 0;
}
#endif

