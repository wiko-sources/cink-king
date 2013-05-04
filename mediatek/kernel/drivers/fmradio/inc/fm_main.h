#ifndef __FM_MAIN_H__
#define __FM_MAIN_H__
#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_rds.h"
#include "fm_eint.h"
#include "fm_link.h"
#include "fm_interface.h"
#include "fm_stdlib.h"
#include "fm_private.h"

#define FM_NAME             "fm"
#define FM_DEVICE_NAME      "/dev/fm"

#define FM_VOL_MAX           0x2B	// 43 volume(0-15)

#define FM_RDS_ENABLE		0x01 // 1: enable RDS, 0:disable RDS
#define FM_RDS_DATA_READY   (1 << 0)

// errno
#define FM_SUCCESS      0
#define FM_FAILED       1
#define FM_EPARM        2
#define FM_BADSTATUS    3
#define FM_TUNE_FAILED  4
#define FM_SEEK_FAILED  5
#define FM_BUSY         6
#define FM_SCAN_FAILED  7

struct fm_tune_parm {
    uint8_t err;
    uint8_t band;
    uint8_t space;
    uint8_t hilo;
    uint16_t freq; // IN/OUT parameter
};

struct fm_seek_parm {
    uint8_t err;
    uint8_t band;
    uint8_t space;
    uint8_t hilo;
    uint8_t seekdir;
    uint8_t seekth;
    uint16_t freq; // IN/OUT parameter
};

#ifdef MTK_FM_50KHZ_SUPPORT
struct fm_scan_parm {
    uint8_t  err;
    uint8_t  band;
    uint8_t  space;
    uint8_t  hilo;
    uint16_t freq; // OUT parameter
    uint16_t ScanTBL[26]; //need no less than the chip
    uint16_t ScanTBLSize; //IN/OUT parameter
};
#else
struct fm_scan_parm {
    uint8_t  err;
    uint8_t  band;
    uint8_t  space;
    uint8_t  hilo;
    uint16_t freq; // OUT parameter
    uint16_t ScanTBL[16]; //need no less than the chip
    uint16_t ScanTBLSize; //IN/OUT parameter
};
#endif

struct fm_cqi {
    fm_s32 ch;
    fm_s32 rssi;
    fm_s32 reserve;
};

struct fm_cqi_req {
    fm_u16 ch_num;
    fm_s32 buf_size;
    fm_s8 *cqi_buf;
};

struct fm_ch_rssi {
    uint16_t freq;
    int rssi;
};

enum fm_scan_cmd_t {
    FM_SCAN_CMD_INIT = 0,
    FM_SCAN_CMD_START,
    FM_SCAN_CMD_GET_NUM,
    FM_SCAN_CMD_GET_CH,
    FM_SCAN_CMD_GET_RSSI,
    FM_SCAN_CMD_GET_CH_RSSI,
    FM_SCAN_CMD_MAX
};

struct fm_scan_t {
    enum fm_scan_cmd_t cmd;    
    int ret;    // 0, success; else error code
    uint16_t lower;             // lower band, Eg, 7600 -> 76.0Mhz
    uint16_t upper;             // upper band, Eg, 10800 -> 108.0Mhz
    int space;                  // 5: 50KHz, 10: 100Khz, 20: 200Khz
    int num;                    // valid channel number 
    void *priv;
    int sr_size;                // scan result buffer size in bytes
    union {
        uint16_t *ch_buf;       // channel buffer
        int *rssi_buf;          // rssi buffer
        struct fm_ch_rssi *ch_rssi_buf;  //channel and RSSI buffer 
    } sr;    
};

struct fm_seek_t {  
    int ret;                    // 0, success; else error code
    uint16_t freq;
    uint16_t lower;             // lower band, Eg, 7600 -> 76.0Mhz
    uint16_t upper;             // upper band, Eg, 10800 -> 108.0Mhz
    int space;                  // 5: 50KHz, 10: 100Khz, 20: 200Khz
    int dir;                    // 0: up; 1: down
    int th;                     // seek threshold in dbm(Eg, -95dbm)
    void *priv;
};

struct fm_tune_t {  
    int ret;                    // 0, success; else error code
    uint16_t freq;
    uint16_t lower;             // lower band, Eg, 7600 -> 76.0Mhz
    uint16_t upper;             // upper band, Eg, 10800 -> 108.0Mhz
    int space;                  // 5: 50KHz, 10: 100Khz, 20: 200Khz
    void *priv;
};


#ifdef MTK_FM_50KHZ_SUPPORT
struct fm_rssi_req {
    uint16_t num;
    uint16_t read_cnt;
    struct fm_ch_rssi cr[26*16];
};
#else
struct fm_rssi_req {
    uint16_t num;
    uint16_t read_cnt;
    struct fm_ch_rssi cr[16*16];
};
#endif

struct fm_rds_tx_parm {
    uint8_t err;
    uint16_t pi;
    uint16_t ps[12]; // 4 ps
    uint16_t other_rds[87];  // 0~29 other groups
    uint8_t other_rds_cnt; // # of other group
};

typedef struct fm_rds_tx_req {
    unsigned char pty;         // 0~31 integer
    unsigned char rds_rbds;    // 0:RDS, 1:RBDS
    unsigned char dyn_pty;     // 0:static, 1:dynamic
    unsigned short pi_code;    // 2-byte hex
    unsigned char ps_buf[8];     // hex buf of PS
    unsigned char ps_len;      // length of PS, must be 0 / 8"
    unsigned char af;          // 0~204, 0:not used, 1~204:(87.5+0.1*af)MHz
    unsigned char ah;          // Artificial head, 0:no, 1:yes
    unsigned char stereo;      // 0:mono, 1:stereo
    unsigned char compress;    // Audio compress, 0:no, 1:yes
    unsigned char tp;          // traffic program, 0:no, 1:yes
    unsigned char ta;          // traffic announcement, 0:no, 1:yes
    unsigned char speech;      // 0:music, 1:speech
} fm_rds_tx_req;

#define TX_SCAN_MAX 10
#define TX_SCAN_MIN 1
struct fm_tx_scan_parm {
    uint8_t  err;
    uint8_t  band;	//87.6~108MHz
    uint8_t  space;
    uint8_t  hilo;
    uint16_t freq; 	// start freq, if less than band min freq, then will use band min freq
    uint8_t	 scandir;
    uint16_t ScanTBL[TX_SCAN_MAX]; 	//need no less than the chip
    uint16_t ScanTBLSize; //IN: desired size, OUT: scan result size
};

struct fm_gps_rtc_info {
    int             err;            //error number, 0: success, other: err code
    int             retryCnt;       //GPS mnl can decide retry times
    int             ageThd;         //GPS 3D fix time diff threshold
    int             driftThd;       //GPS RTC drift threshold
    struct timeval  tvThd;          //time value diff threshold
    int             age;            //GPS 3D fix time diff
    int             drift;          //GPS RTC drift
    union {
        unsigned long stamp;        //time stamp in jiffies
        struct timeval  tv;         //time stamp value in RTC
    };
    int             flag;           //rw flag
};

typedef enum {
    FM_I2S_ON = 0,
    FM_I2S_OFF
} fm_i2s_state;

typedef enum {
    FM_I2S_MASTER = 0,
    FM_I2S_SLAVE
} fm_i2s_mode;

typedef enum {
    FM_I2S_32K = 0,
    FM_I2S_44K,
    FM_I2S_48K
} fm_i2s_sample;

struct fm_i2s_setting {
    int onoff;
    int mode;
    int sample;
};

typedef enum {
    FM_RX = 0,
    FM_TX = 1
} FM_PWR_T;

enum group_idx {
    mono = 0,
    stereo,
    RSSI_threshold,
    HCC_Enable,
    PAMD_threshold,
    Softmute_Enable,
    De_emphasis,
    HL_Side,
    Demod_BW,
    Dynamic_Limiter,
    Softmute_Rate,
    AFC_Enable,
    Softmute_Level,
    Analog_Volume,
    GROUP_TOTAL_NUMS
};

enum item_idx {
    Sblend_OFF = 0,
    Sblend_ON,
    ITEM_TOTAL_NUMS
};

struct fm_ctl_parm {
    uint8_t err;
    uint8_t addr;
    uint16_t val;
    uint16_t rw_flag;//0:write, 1:read
};

struct fm_em_parm {
    uint16_t group_idx;
    uint16_t item_idx;
    uint32_t item_value;
};


enum {
    FM_SUBSYS_RST_OFF,
    FM_SUBSYS_RST_START,
    FM_SUBSYS_RST_END,
    FM_SUBSYS_RST_MAX
};

//init and deinit APIs
extern fm_s32 fm_env_setup(void);
extern fm_s32 fm_env_destroy(void);
extern struct fm* fm_dev_init(fm_u32 arg);
extern fm_s32 fm_dev_destroy(struct fm *fm);


//fm main basic APIs
extern enum fm_pwr_state fm_pwr_state_get(struct fm *fmp);
extern enum fm_pwr_state fm_pwr_state_set(struct fm *fmp, enum fm_pwr_state sta);
extern fm_s32 fm_open(struct fm *fmp);
extern fm_s32 fm_close(struct fm *fmp);
extern fm_s32 fm_rds_read(struct fm *fmp, fm_s8 *dst, fm_s32 len);
extern fm_s32 fm_powerup(struct fm *fm, struct fm_tune_parm *parm);
extern fm_s32 fm_powerdown(struct fm *fm);
extern fm_s32 fm_seek(struct fm *fm, struct fm_seek_parm *parm);
extern fm_s32 fm_scan(struct fm *fm, struct fm_scan_parm *parm);
extern fm_s32 fm_cqi_get(struct fm *fm, fm_s32 ch_num, fm_s8 *buf, fm_s32 buf_size);
extern fm_s32 fm_get_hw_info(struct fm *pfm, struct fm_hw_info *req);
extern fm_s32 fm_hwscan_stop(struct fm *fm);
extern fm_s32 fm_ana_switch(struct fm *fm, fm_s32 antenna);
extern fm_s32 fm_setvol(struct fm *fm, fm_u32 vol);
extern fm_s32 fm_getvol(struct fm *fm, fm_u32 *vol);
extern fm_s32 fm_mute(struct fm *fm, fm_u32 bmute);
extern fm_s32 fm_getrssi(struct fm *fm, fm_s32 *rssi);
extern fm_s32 fm_reg_read(struct fm *fm, fm_u8 addr, fm_u16 *val);
extern fm_s32 fm_reg_write(struct fm *fm, fm_u8 addr, fm_u16 val);
extern fm_s32 fm_chipid_get(struct fm *fm, fm_u16 *chipid);
extern fm_s32 fm_monostereo_get(struct fm *fm, fm_u16 *ms);
extern fm_s32 fm_monostereo_set(struct fm *fm, fm_s32 ms);
extern fm_s32 fm_pamd_get(struct fm *fm, fm_u16 *pamd);
extern fm_s32 fm_caparray_get(struct fm *fm, fm_s32 *ca);
extern fm_s32 fm_em_test(struct fm *fm, fm_u16 group, fm_u16 item, fm_u32 val);
extern fm_s32 fm_rds_onoff(struct fm *fm, fm_u16 rdson_off);
extern fm_s32 fm_rds_good_bc_get(struct fm *fm, fm_u16 *gbc);
extern fm_s32 fm_rds_bad_bc_get(struct fm *fm, fm_u16 *bbc);
extern fm_s32 fm_rds_bler_ratio_get(struct fm *fm, fm_u16 *bbr);
extern fm_s32 fm_rds_group_cnt_get(struct fm *fm, struct rds_group_cnt_t *dst);
extern fm_s32 fm_rds_group_cnt_reset(struct fm *fm);
extern fm_s32 fm_rds_log_get(struct fm *fm, struct rds_rx_t *dst, fm_s32 *dst_len);
extern fm_s32 fm_rds_block_cnt_reset(struct fm *fm);
extern fm_s32 fm_i2s_set(struct fm *fm, fm_s32 onoff, fm_s32 mode, fm_s32 sample);
extern fm_s32 fm_get_i2s_info(struct fm *pfm, struct fm_i2s_info *req);
extern fm_s32 fm_tune(struct fm *fm, struct fm_tune_parm *parm);
extern fm_s32 fm_is_dese_chan(struct fm *pfm, fm_u16 freq);
extern fm_s32 fm_sys_state_get(struct fm *fmp);
extern fm_s32 fm_sys_state_set(struct fm *fmp, fm_s32 sta);
extern fm_s32 fm_subsys_reset(struct fm *fm);

extern fm_s32 fm_scan_new(struct fm *fm, struct fm_scan_t *parm);
extern fm_s32 fm_seek_new(struct fm *fm, struct fm_seek_t *parm);
extern fm_s32 fm_tune_new(struct fm *fm, struct fm_tune_t *parm);

#endif //__FM_MAIN_H__

