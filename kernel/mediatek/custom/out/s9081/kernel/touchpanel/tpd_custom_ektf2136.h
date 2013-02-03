

#ifndef TOUCHPANEL_H__
#define TOUCHPANEL_H__

/* Pre-defined definition */
#define TPD_USE_VIRTUAL_KEY
#define TPD_TYPE_CAPACITIVE
#define TPD_TYPE_RESISTIVE
#define TPD_I2C_NUMBER           1
#define SUPORT_5_POINTS
#define tpd_driver_name 	"Ektf2136"
#define slaver_addr			 0x2a
#define SUPPORT_UPDATE_FW
#define SW_FIRMWARE_UPDATE
#define TPD_WAKEUP_TRIAL         60
#define TPD_WAKEUP_DELAY         100

#define TPD_DELAY                (2*HZ/100)
#define TPD_CUST_RES_X          	480
#define TPD_CUST_RES_Y           854

#define EKTF_WIDTH      704
#define EKTF_HEIGHT     1255//1344
#define TPD_CALIBRATION_MATRIX  {962,0,0,0,1600,0,0,0};

//#define TPD_HAVE_CALIBRATION
#define TPD_HAVE_BUTTON
//#define TPD_HAVE_TREMBLE_ELIMINATION

//#define TPD_HAVE_POWER_ON_OFF

#define PRESSURE_FACTOR	10
#define TPD_BUTTON_HEIGHT		854

#define VIRTUAL_KEY_DEB_TIME	3
#define TPD_Y_OFFSET		0

#define TPD_KEY_COUNT           3
#define TPD_KEYS                {KEY_MENU,KEY_HOMEPAGE,KEY_BACK}
static const int TPD_KEYSFACTORY[TPD_KEY_COUNT] =  {KEY_F1, KEY_F2, KEY_F3};

#define TPD_YMAX		915		//Truely, BYD = 516
#define TPD_BUTTON_SIZE_HEIGHT  (TPD_YMAX - TPD_BUTTON_HEIGHT - TPD_Y_OFFSET)
#define TPD_BUTTON_Y_CENTER   	(TPD_BUTTON_HEIGHT + (TPD_YMAX - TPD_BUTTON_HEIGHT)/2 + TPD_Y_OFFSET)

//TP virtual key customization

// |                                                                                                   |
// |                                                                                                   |  Touch Pad area ( H < 480)
//  ---------------------------------------------------------------------------------------------------
// |                                           TPD_Y_OFFSET                                            |  Virtual key area ( H > 480)
// |---------------------------------------------------------------------------------------------------
// |TPD_B1_FP | [TPD_B1_W] | TPD_B2_FP | [TPD_B2_W] | TPD_B3_FP | [TPD_B3_W] | TPD_B4_FP | [TPD_B4_W]  |  
// -----------------------------------------------------------------------------------------------------

#define TPD_B1_FP	0		//Button 1 pad space
#define TPD_B1_W	120		//Button 1 Width
#define TPD_B2_FP	80		//Button 2 pad space
#define TPD_B2_W	120		//Button 2 Width
#define TPD_B3_FP	80		//Button 3 pad space
#define TPD_B3_W	120		//Button 3 Width
#define TPD_B4_FP	0		//Button 4 pad space
#define TPD_B4_W	75		//Button 4 width
//#define TPD_KEYS_DIM            {{80,850,160,TPD_BUTTON_HEIGH},{240,850,160,TPD_BUTTON_HEIGH},{400,850,160,TPD_BUTTON_HEIGH}}


//-------------------------------------------------------------------------
#define TPD_BUTTON1_X_CENTER	TPD_B1_FP + TPD_B1_W/2
#define TPD_BUTTON2_X_CENTER	TPD_B1_FP + TPD_B1_W + TPD_B2_FP + TPD_B2_W/2
#define TPD_BUTTON3_X_CENTER	TPD_B1_FP + TPD_B1_W + TPD_B2_FP + TPD_B2_W + TPD_B3_FP + TPD_B3_W/2
#define TPD_BUTTON4_X_CENTER	TPD_B1_FP + TPD_B1_W + TPD_B2_FP + TPD_B2_W + TPD_B3_FP + TPD_B3_W + TPD_B4_FP + TPD_B4_W/2


#define TPD_KEYS_DIM            {{TPD_BUTTON1_X_CENTER, TPD_BUTTON_Y_CENTER, TPD_B1_W, TPD_BUTTON_SIZE_HEIGHT},	\
				 {TPD_BUTTON2_X_CENTER, TPD_BUTTON_Y_CENTER, TPD_B2_W, TPD_BUTTON_SIZE_HEIGHT},	\
				 {TPD_BUTTON3_X_CENTER, TPD_BUTTON_Y_CENTER, TPD_B3_W, TPD_BUTTON_SIZE_HEIGHT}}

#endif /* TOUCHPANEL_H__ */
