

#include <linux/types.h>
#include <cust_mag.h>

#ifdef MT6575
#include <mach/mt6575_pm_ldo.h>
#endif

#ifdef MT6577
#include <mach/mt6577_pm_ldo.h>
#endif

static struct mag_hw cust_mag_hw = {
    .i2c_num = 0,
    .direction = 5,			//5
    .power_id = MT65XX_POWER_NONE,  /*!< LDO is not used */
    .power_vol= VOL_DEFAULT,        /*!< LDO is not used */
};
struct mag_hw* get_cust_mag_hw(void) 
{
    return &cust_mag_hw;
}
