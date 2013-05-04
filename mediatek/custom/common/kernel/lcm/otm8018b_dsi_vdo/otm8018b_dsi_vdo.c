#ifdef BUILD_LK
#else
#include <linux/string.h>
#if defined(BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
#include <mach/mt6577_gpio.h>
#endif
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(800)

#define REGFLAG_DELAY             							0xFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER

#define LCM_ID_OTM8018B	0x8009

#define LCM_DSI_CMD_MODE									0

#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

    {0x00, 1 ,{0x00}}, 
    {0xff, 3 ,{0x80,0x09,0x01}}, // enable EXTC

    {0x00, 1 ,{0x80}}, 
    {0xff, 2 ,{0x80,0x09}},      // enable Orise mode

    {0x00, 1 ,{0x03}}, 
    {0xff, 1 ,{0x01}},     

    {0x00, 1 ,{0x00}}, 
    {0x21, 1 ,{0x00}},   

    {0x00, 1 ,{0x00}}, 
    {0xD8, 2 ,{0x77,0x77}}, 

    {0x00, 1 ,{0x82}}, 
    {0xC5, 1 ,{0xA3}}, 

    {0x00, 1 ,{0x81}}, 
    {0xC1, 1 ,{0x66}}, 

    {0x00, 1 ,{0xA1}}, 
    {0xC1, 1 ,{0x08}}, 

    {0x00, 1 ,{0xA3}}, 
    {0xC0, 1 ,{0x1B}}, 

    {0x00, 1 ,{0x81}}, 
    {0xC4, 1 ,{0x83}}, 

    {0x00, 1 ,{0x87}}, 
    {0xC4, 3 ,{0x00,0x80,0x00}}, 
    {REGFLAG_DELAY, 1, {}},

    {0x00, 1 ,{0xB2}}, 
    {0xF5, 4 ,{0x15,0x00,0x15,0x00}}, 
    {REGFLAG_DELAY, 1, {}},

    {0x00, 1 ,{0x90}}, 
    {0xC5, 4 ,{0x96,0x85,0x00,0x03}}, 
    {REGFLAG_DELAY, 1, {}},

    {0x00, 1 ,{0xc0}}, 
    {0xC5, 1 ,{0x00}}, 

    {0x00, 1 ,{0x8b}}, 
    {0xB0, 1 ,{0x40}}, 

    {0x00, 1 ,{0xB1}}, 
    {0xC5, 1 ,{0xA9}}, 

    {0x00, 1 ,{0x00}}, 
    {0xD9, 1 ,{0x2D}}, 


    {0x00,	1,	{0x00}}, 
	{0xe1,	16,	{0x06,0x0E,0x16,0x0F,0x0A,0x17,0x0D,0x0C,0x01,0x05,0x04,0x08,0x10,0x26,0x21,0x04}}, 
    {REGFLAG_DELAY, 10, {}},
         
    {0x00,	1,	{0x00}}, 
    {0xe2,	16,	{0x06,0x0E,0x16,0x0F,0x0A,0x17,0x0D,0x0C,0x01,0x05,0x04,0x08,0x10,0x26,0x21,0x04}}, 
    {REGFLAG_DELAY, 10, {}},


    {0x00, 1 ,{0xA1}}, 
    {0xB3, 1 ,{0x10}}, 

    {0x00, 1 ,{0xA6}}, 
    {0xB3, 2 ,{0x2B,0x11}}, 
    {REGFLAG_DELAY, 1, {}},

    {0x00, 1 ,{0x80}}, 
    {0xCE, 6 ,{0x83,0x01,0x00,0x82,0x01,0x00}}, 
    {REGFLAG_DELAY, 10, {}},


	{0x00,	1,	{0xA0}}, 
	{0xce,	14,	{0x18,0x01,0x03,0x22,0x00,0x00,0x00,0x18,0x00,0x03,0x23,0x00,0x00,0x00}}, 
    {REGFLAG_DELAY, 10, {}},

	{0x00,	1,	{0xb0}}, 
	{0xce,	14,	{0x10,0x00,0x03,0x22,0x00,0x00,0x00,0x10,0x01,0x03,0x23,0x00,0x00,0x00}}, 
    {REGFLAG_DELAY, 10, {}},

	
	{0x00,	1,	{0xc7}}, 
	{0xcf,	1,	{0x80}}, 
	
	{0x00,	1,	{0xc9}}, 
	{0xcf,	1,	{0x06}}, 
	
	{0x00,	1,	{0xc6}}, 
	{0xcb,	2,	{0x04,0x04}}, 
    {REGFLAG_DELAY, 1, {}},
	
	{0x00,	1,	{0xc9}}, 
	{0xcb,	1,	{0x04}}, 

	{0x00,	1,	{0xdb}}, 
	{0xcb,	2,	{0x04,0x04}}, 
    {REGFLAG_DELAY, 1, {}},
	
	{0x00,	1,	{0xde}}, 
	{0xcb,	1,	{0x04}}, 

	{0x00,	1,	{0x86}}, 
	{0xcc,	2,	{0x0c,0x0a}}, 
    {REGFLAG_DELAY, 1, {}},
	
	{0x00,	1,	{0x89}}, 
	{0xcc,	1,	{0x02}}, 

	{0x00,	1,	{0xa1}}, 
	{0xcc,	2,	{0x0b,0x09}}, 
    {REGFLAG_DELAY, 1, {}},

	{0x00,	1,	{0xa4}}, 
	{0xcc,	1,	{0x01}}, 

	{0x00,	1,	{0xb6}}, 
	{0xcc,	2,	{0x09,0x0b}}, 
    {REGFLAG_DELAY, 1, {}},

	{0x00,	1,	{0xb9}}, 
	{0xcc,	1,	{0x01}}, 

	{0x00,	1,	{0xd1}}, 
	{0xcc,	2,	{0x0a,0x0c}}, 
    {REGFLAG_DELAY, 1, {}},

	{0x00,	1,	{0xd4}}, 
	{0xcc,	1,	{0x02}}, 


    {0x00,	1,	{0x90}},
	{0xc0,	6,	{0x00,0x44,0x00,0x00,0x00,0x03}}, 
    {REGFLAG_DELAY, 5, {}},

    {0x00,	1,	{0xA6}},
	{0xc1,	3,	{0x01,0x00,0x00}}, 

	{REGFLAG_DELAY, 10, {}},
	{0x11,1,   {0x00}}, 
	{REGFLAG_DELAY, 150, {}},
	{0x29,1,	{0x00}}, 
	{REGFLAG_DELAY, 20, {}},

	{0x2C,	0,	{}},	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 50, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 4;
		params->dsi.vertical_backporch					= 8;
		params->dsi.vertical_frontporch					= 8;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 6;
		params->dsi.horizontal_backporch				= 37;
		params->dsi.horizontal_frontporch				= 37;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		// Bit rate calculation
		params->dsi.pll_div1=29;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1; 		// div2=0~15: fout=fvo/(2*div2)

		/* ESD or noise interference recovery For video mode LCM only. */ // Send TE packet to LCM in a period of n frames and check the response. 
		params->dsi.lcm_int_te_monitor = FALSE; 
		params->dsi.lcm_int_te_period = 1; // Unit : frames 
 
		// Need longer FP for more opportunity to do int. TE monitor applicably. 
		if(params->dsi.lcm_int_te_monitor) 
			params->dsi.vertical_frontporch *= 2; 
 
		// Monitor external TE (or named VSYNC) from LCM once per 2 sec. (LCM VSYNC must be wired to baseband TE pin.) 
		params->dsi.lcm_ext_te_monitor = TRUE; 
		// Non-continuous clock 
		params->dsi.noncont_clock = FALSE; 
		params->dsi.noncont_clock_period = 2; // Unit : frames
}


static void lcm_init(void)
{
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(100);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(20);
	
}


static void lcm_resume(void)
{
	lcm_init();
	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static struct LCM_setting_table lcm_compare_id_setting[] = {

	{0x00,	1,	{0x00}},
	{0xff,	3,	{0x80,0x09,0x01}}, 
	{REGFLAG_DELAY, 10, {}},

	{0x00,	1,	{0x80}},
	{0xff,	3,	{0x80,0x09,0x01}}, 
	{REGFLAG_DELAY, 10, {}},

	{0x00,	1,	{0x02}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}

};

static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[5];
	char id_high=0;
	char id_low=0;
	int id=0;

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(100);
/*
	array[0] = 0x00053700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xa1, buffer, 5);

	id_high = buffer[2];
	id_low = buffer[3];
	id = (id_high<<8) | id_low;
*/
	push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);

	array[0] = 0x00023700;// set return byte number
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xD2, &buffer, 2);

	id = buffer[0]<<8 |buffer[1]; 

	#if defined(BUILD_UBOOT)||defined(BUILD_LK)
		//printf("OTM8018B uboot %s \n", __func__);
		//printf("%s id = 0x%08x \n", __func__, id);
	#else
		//printk("OTM8018B kernel %s \n", __func__);
		//printk("%s id = 0x%08x \n", __func__, id);
		printk("OTM8018B 0x%x , 0x%x , 0x%x \n",buffer[0],buffer[1],id);
	#endif

	//return (id == LCM_ID_OTM8018B)?1:0;
	return 1;
}


LCM_DRIVER otm8018b_dsi_vdo_lcm_drv = 
{
    .name			= "otm8018b_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,	
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
};

