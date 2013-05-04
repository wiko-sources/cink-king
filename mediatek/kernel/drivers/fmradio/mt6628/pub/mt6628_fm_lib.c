#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "stp_exp.h"
#include "wmt_exp.h"

#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_interface.h"
#include "fm_stdlib.h"
#include "fm_patch.h"
#include "fm_utils.h"
#include "fm_config.h"

#include "mt6628_fm_reg.h"
#include "mt6628_fm.h"
#include "mt6628_drv_dsp.h"
#include "mt6628_fm_link.h"
#include "mt6628_fm_lib.h"
#include "mt6628_fm_cmd.h"

#define MT6628_FM_PATCH_PATH "/etc/firmware/mt6628_fm_patch.bin"
#define MT6628_FM_COEFF_PATH "/etc/firmware/mt6628_fm_coeff.bin"
#define MT6628_FM_HWCOEFF_PATH "/etc/firmware/mt6628_fm_hwcoeff.bin"
#define MT6628_FM_ROM_PATH "/etc/firmware/mt6628_fm_rom.bin"

static struct fm_patch_tbl mt6628_patch_tbl[5] = {
    {FM_ROM_V1, "/etc/firmware/mt6628_fm_v1_patch.bin", "/etc/firmware/mt6628_fm_v1_coeff.bin", NULL, NULL},
    {FM_ROM_V2, "/etc/firmware/mt6628_fm_v2_patch.bin", "/etc/firmware/mt6628_fm_v2_coeff.bin", NULL, NULL},
    {FM_ROM_V3, "/etc/firmware/mt6628_fm_v3_patch.bin", "/etc/firmware/mt6628_fm_v3_coeff.bin", NULL, NULL},
    {FM_ROM_V4, "/etc/firmware/mt6628_fm_v4_patch.bin", "/etc/firmware/mt6628_fm_v4_coeff.bin", NULL, NULL},
    {FM_ROM_V5, "/etc/firmware/mt6628_fm_v5_patch.bin", "/etc/firmware/mt6628_fm_v5_coeff.bin", NULL, NULL},
};

static struct fm_hw_info mt6628_hw_info = {
    .chip_id = 0x00006628,
    .eco_ver = 0x00000000,
    .rom_ver = 0x00000000,
    .patch_ver = 0x00000000,
    .reserve = 0x00000000,
};

static struct mt6628_fm_i2s_info mt6628_i2s_inf = {
    .status = 0,    //i2s off
    .mode = 0,      //slave mode
    .rate = 48000,  //48000 sample rate
};

#define PATCH_SEG_LEN 512

static fm_u8 *cmd_buf = NULL;
static struct fm_lock *cmd_buf_lock = NULL;
static struct fm_callback *fm_cb_op;
static struct fm_res_ctx *res = NULL;
static fm_s32 Chip_Version = mt6628_E1;

static fm_bool rssi_th_set = fm_false;

//#define MT6628_FM_USE_6626EVB
//#define MT6628_FM_FPGA
#ifdef MT6628_FM_FPGA
#undef MT6628_FM_USE_6626EVB
#endif

#ifdef MTK_FM_50KHZ_SUPPORT
static struct fm_fifo *cqi_fifo = NULL;
#endif

static fm_s32 mt6628_I2s_Setting(fm_s32 onoff, fm_s32 mode, fm_s32 sample);

static fm_s32 mt6628_pwron(fm_s32 data)
{
    /*//Turn on FM on 6628 chip by WMT driver
    if(MTK_WCN_BOOL_FALSE == mtk_wcn_wmt_func_on(WMTDRV_TYPE_LPBK)){
        WCN_DBG(FM_ALT|CHIP,"WMT turn on LPBK Fail!\n");
        return -FM_ELINK;
    }else{
        WCN_DBG(FM_ALT|CHIP,"WMT turn on LPBK OK!\n");
        //return 0;
    }*/
    if (MTK_WCN_BOOL_FALSE == mtk_wcn_wmt_func_on(WMTDRV_TYPE_FM)) {
        WCN_DBG(FM_ALT | CHIP, "WMT turn on FM Fail!\n");
        return -FM_ELINK;
    } else {
        WCN_DBG(FM_ALT | CHIP, "WMT turn on FM OK!\n");
        return 0;
    }
}


static fm_s32 mt6628_pwroff(fm_s32 data)
{
    if (MTK_WCN_BOOL_FALSE == mtk_wcn_wmt_func_off(WMTDRV_TYPE_FM)) {
        WCN_DBG(FM_ALT | CHIP, "WMT turn off FM Fail!\n");
        return -FM_ELINK;
    } else {
        WCN_DBG(FM_NTC | CHIP, "WMT turn off FM OK!\n");
        return 0;
    }
}

static fm_s32 Delayms(fm_u32 data)
{
    WCN_DBG(FM_DBG | CHIP, "delay %dms\n", data);
    msleep(data);
    return 0;
}

static fm_s32 Delayus(fm_u32 data)
{
    WCN_DBG(FM_DBG | CHIP, "delay %dus\n", data);
    udelay(data);
    return 0;
}

fm_s32 mt6628_get_read_result(struct fm_res_ctx* result)
{
    FMR_ASSERT(result);
    res = result;

    return 0;
}

static fm_s32 mt6628_read(fm_u8 addr, fm_u16 *val)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_get_reg(cmd_buf, TX_BUF_SIZE, addr);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_FSPI_RD, SW_RETRY_CNT, FSPI_RD_TIMEOUT, mt6628_get_read_result);

    if (!ret && res) {
        *val = res->fspi_rd;
    }

    FM_UNLOCK(cmd_buf_lock);

    return ret;
}

static fm_s32 mt6628_write(fm_u8 addr, fm_u16 val)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_set_reg(cmd_buf, TX_BUF_SIZE, addr, val);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_FSPI_WR, SW_RETRY_CNT, FSPI_WR_TIMEOUT, NULL);
    FM_UNLOCK(cmd_buf_lock);

    return ret;
}

static fm_s32 mt6628_set_bits(fm_u8 addr, fm_u16 bits, fm_u16 mask)
{
    fm_s32 ret = 0;
    fm_u16 val;

    ret = mt6628_read(addr, &val);

    if (ret)
        return ret;

    val = ((val & (mask)) | bits);
    ret = mt6628_write(addr, val);

    return ret;
}

static fm_u16 mt6628_get_chipid(void)
{
    return 0x6628;
}

/*  MT6628_SetAntennaType - set Antenna type
 *  @type - 1,Short Antenna;  0, Long Antenna
 */
static fm_s32 mt6628_SetAntennaType(fm_s32 type)
{
    fm_u16 dataRead;

    WCN_DBG(FM_DBG | CHIP, "set ana to %s\n", type ? "short" : "long");
    mt6628_read(FM_MAIN_CG2_CTRL, &dataRead);

    if (type) {
        dataRead |= ANTENNA_TYPE;
    } else {
        dataRead &= (~ANTENNA_TYPE);
    }

    mt6628_write(FM_MAIN_CG2_CTRL, dataRead);

    return 0;
}

static fm_s32 mt6628_GetAntennaType(void)
{
    fm_u16 dataRead;

    mt6628_read(FM_MAIN_CG2_CTRL, &dataRead);
    WCN_DBG(FM_DBG | CHIP, "get ana type: %s\n", (dataRead&ANTENNA_TYPE) ? "short" : "long");

    if (dataRead&ANTENNA_TYPE)
        return FM_SHORT_ANA; //short antenna
    else
        return FM_LONG_ANA; //long antenna
}


static fm_s32 mt6628_Mute(fm_bool mute)
{
    fm_s32 ret = 0;
    fm_u16 dataRead;

    WCN_DBG(FM_DBG | CHIP, "set %s\n", mute ? "mute" : "unmute");
    mt6628_read(FM_MAIN_CTRL, &dataRead);

    if (mute == 1) {
        ret = mt6628_write(FM_MAIN_CTRL, (dataRead&0xFFDF) | 0x0020);
    } else {
        ret = mt6628_write(FM_MAIN_CTRL, (dataRead&0xFFDF));
}

    return ret;
}


static fm_s32 mt6628_set_RSSITh(fm_u16 TH_long, fm_u16 TH_short)
{
    mt6628_write(0xE2, 0x3072);
    mt6628_write(0xE3, TH_long);
    mt6628_write(0xE1, 0x0002);
    Delayms(1);
    mt6628_write(0xE2, 0x307A);
    mt6628_write(0xE3, TH_short);
    mt6628_write(0xE1, 0x0002);

    WCN_DBG(FM_DBG | CHIP, "RSSI TH, long:0x%04x, short:0x%04x", TH_long, TH_short);
    return 0;
}

static fm_s32 mt6628_set_SMGTh(fm_s32 ver, fm_u16 TH_smg)
{
    if (mt6628_E1 == ver) {
        mt6628_write(0xE2, 0x321E);
        mt6628_write(0xE3, TH_smg);
        mt6628_write(0xE1, 0x0002);
    } else {
        mt6628_write(0xE2, 0x3218);
        mt6628_write(0xE3, TH_smg);
        mt6628_write(0xE1, 0x0002);
    }

    WCN_DBG(FM_DBG | CHIP, "Soft-mute gain TH %d\n", (int)TH_smg);
    return 0;
}

static fm_s32 mt6628_RampDown(void)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;
    fm_u16 tmp;

    WCN_DBG(FM_DBG | CHIP, "ramp down\n");

    mt6628_read(FM_MAIN_INTRMASK, &tmp);
    mt6628_write(FM_MAIN_INTRMASK, 0x0000);
    mt6628_write(0x6B, 0x0000);

    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_rampdown(cmd_buf, TX_BUF_SIZE);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_RAMPDOWN, SW_RETRY_CNT, RAMPDOWN_TIMEOUT, NULL);
    FM_UNLOCK(cmd_buf_lock);

    if (ret) {
        WCN_DBG(FM_ERR | CHIP, "ramp down failed\n");
    }

#ifdef MT6628_FM_FPGA
    Delayms(10);
    mt6628_write(0x54, 0x0001);
    mt6628_write(0x11, 0x37D4);
    mt6628_write(0x54, 0x8000);
#endif
    mt6628_write(0x6B, 0x2100);
    mt6628_write(FM_MAIN_INTRMASK, tmp);
    return ret;
}

static fm_s32 mt6628_get_rom_version(void)
{
    fm_u16 tmp;

    //DSP rom code version request enable --- set 0x61 b15=1
    mt6628_set_bits(0x61, 0x8000, 0x7FFF);

    //Release ASIP reset --- set 0x61 b1=1
    mt6628_set_bits(0x61, 0x0002, 0xFFFD);

    //Enable ASIP power --- set 0x61 b0=0
    mt6628_set_bits(0x61, 0x0000, 0xFFFE);

    //Wait DSP code version ready --- wait 1ms
    Delayus(1000);

    //Get FM DSP code version --- rd 0x83[15:8]
    mt6628_read(0x83, &tmp);
    tmp = (tmp >> 8);

    //DSP rom code version request disable --- set 0x61 b15=0
    mt6628_set_bits(0x61, 0x0000, 0x7FFF);

    //Reset ASIP --- set 0x61[1:0] = 1
    mt6628_set_bits(0x61, 0x0001, 0xFFFC);

    WCN_DBG(FM_NTC | CHIP, "ROM version: v%d\n", (fm_s32)tmp);
    return (fm_s32)tmp;
}

static fm_s32 mt6628_get_patch_path(fm_s32 ver, const fm_s8 **ppath)
{
    fm_s32 i;
    fm_s32 max = sizeof(mt6628_patch_tbl) / sizeof(mt6628_patch_tbl[0]);

    //check if the ROM version is defined or not
    for (i = 0; i < max; i++) {
        if ((mt6628_patch_tbl[i].idx == ver) && (fm_file_exist(mt6628_patch_tbl[i].patch) == 0)) {
            *ppath = mt6628_patch_tbl[i].patch;
            WCN_DBG(FM_NTC | CHIP, "Get ROM version OK\n");
            return 0;
        }
    }


    // the ROM version isn't defined, find a latest patch instead
    for (i = max; i > 0; i--) {
        if (fm_file_exist(mt6628_patch_tbl[i-1].patch) == 0) {
            *ppath = mt6628_patch_tbl[i-1].patch;
            WCN_DBG(FM_WAR | CHIP, "undefined ROM version\n");
            return 1;
        }
    }

    // get path failed
    WCN_DBG(FM_ERR | CHIP, "No valid patch file\n");
    return -FM_EPATCH;
}


static fm_s32 mt6628_get_coeff_path(fm_s32 ver, const fm_s8 **ppath)
{
    fm_s32 i;
    fm_s32 max = sizeof(mt6628_patch_tbl) / sizeof(mt6628_patch_tbl[0]);

    //check if the ROM version is defined or not
    for (i = 0; i < max; i++) {
        if ((mt6628_patch_tbl[i].idx == ver) && (fm_file_exist(mt6628_patch_tbl[i].coeff) == 0)) {
            *ppath = mt6628_patch_tbl[i].coeff;
            WCN_DBG(FM_NTC | CHIP, "Get ROM version OK\n");
            return 0;
        }
    }


    // the ROM version isn't defined, find a latest patch instead
    for (i = max; i > 0; i--) {
        if (fm_file_exist(mt6628_patch_tbl[i-1].coeff) == 0) {
            *ppath = mt6628_patch_tbl[i-1].coeff;
            WCN_DBG(FM_WAR | CHIP, "undefined ROM version\n");
            return 1;
        }
    }

    // get path failed
    WCN_DBG(FM_ERR | CHIP, "No valid coeff file\n");
    return -FM_EPATCH;
}


/*
*  mt6628_DspPatch - DSP download procedure
*  @img - source dsp bin code
*  @len - patch length in byte
*  @type - rom/patch/coefficient/hw_coefficient
*/
static fm_s32 mt6628_DspPatch(const fm_u8 *img, fm_s32 len, enum IMG_TYPE type)
{
    fm_u8 seg_num;
    fm_u8 seg_id = 0;
    fm_s32 seg_len;
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    FMR_ASSERT(img);

    if (len <= 0) {
        return -1;
    }

    seg_num = len / PATCH_SEG_LEN + 1;
    WCN_DBG(FM_NTC | CHIP, "binary len:%d, seg num:%d\n", len, seg_num);

    switch (type) {
    case IMG_ROM:

        for (seg_id = 0; seg_id < seg_num; seg_id++) {
            seg_len = ((seg_id + 1) < seg_num) ? PATCH_SEG_LEN : (len % PATCH_SEG_LEN);
            WCN_DBG(FM_NTC | CHIP, "rom,[seg_id:%d],  [seg_len:%d]\n", seg_id, seg_len);
            FM_LOCK(cmd_buf_lock);
            pkt_size = mt6628_rom_download(cmd_buf, TX_BUF_SIZE, seg_num, seg_id, &img[seg_id*PATCH_SEG_LEN], seg_len);
            WCN_DBG(FM_NTC | CHIP, "pkt_size:%d\n", (fm_s32)pkt_size);
            ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_ROM, SW_RETRY_CNT, ROM_TIMEOUT, NULL);
            FM_UNLOCK(cmd_buf_lock);

            if (ret) {
                WCN_DBG(FM_ALT | CHIP, "mt6628_rom_download failed\n");
                return ret;
            }
        }

        break;
    case IMG_PATCH:

        for (seg_id = 0; seg_id < seg_num; seg_id++) {
            seg_len = ((seg_id + 1) < seg_num) ? PATCH_SEG_LEN : (len % PATCH_SEG_LEN);
            WCN_DBG(FM_NTC | CHIP, "patch,[seg_id:%d],  [seg_len:%d]\n", seg_id, seg_len);
            FM_LOCK(cmd_buf_lock);
            pkt_size = mt6628_patch_download(cmd_buf, TX_BUF_SIZE, seg_num, seg_id, &img[seg_id*PATCH_SEG_LEN], seg_len);
            WCN_DBG(FM_NTC | CHIP, "pkt_size:%d\n", (fm_s32)pkt_size);
            ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_PATCH, SW_RETRY_CNT, PATCH_TIMEOUT, NULL);
            FM_UNLOCK(cmd_buf_lock);

            if (ret) {
                WCN_DBG(FM_ALT | CHIP, "mt6628_patch_download failed\n");
                return ret;
            }
        }

        break;
    case IMG_HW_COEFFICIENT:

        for (seg_id = 0; seg_id < seg_num; seg_id++) {
            seg_len = ((seg_id + 1) < seg_num) ? PATCH_SEG_LEN : (len % PATCH_SEG_LEN);
            WCN_DBG(FM_NTC | CHIP, "hwcoeff,[seg_id:%d],  [seg_len:%d]\n", seg_id, seg_len);
            FM_LOCK(cmd_buf_lock);
            pkt_size = mt6628_hwcoeff_download(cmd_buf, TX_BUF_SIZE, seg_num, seg_id, &img[seg_id*PATCH_SEG_LEN], seg_len);
            WCN_DBG(FM_NTC | CHIP, "pkt_size:%d\n", (fm_s32)pkt_size);
            ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_HWCOEFF, SW_RETRY_CNT, HWCOEFF_TIMEOUT, NULL);
            FM_UNLOCK(cmd_buf_lock);

            if (ret) {
                WCN_DBG(FM_ALT | CHIP, "mt6628_hwcoeff_download failed\n");
                return ret;
            }
        }

        break;
    case IMG_COEFFICIENT:

        for (seg_id = 0; seg_id < seg_num; seg_id++) {
            seg_len = ((seg_id + 1) < seg_num) ? PATCH_SEG_LEN : (len % PATCH_SEG_LEN);
            WCN_DBG(FM_NTC | CHIP, "coeff,[seg_id:%d],  [seg_len:%d]\n", seg_id, seg_len);
            FM_LOCK(cmd_buf_lock);
            pkt_size = mt6628_coeff_download(cmd_buf, TX_BUF_SIZE, seg_num, seg_id, &img[seg_id*PATCH_SEG_LEN], seg_len);
            WCN_DBG(FM_NTC | CHIP, "pkt_size:%d\n", (fm_s32)pkt_size);
            ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_COEFF, SW_RETRY_CNT, COEFF_TIMEOUT, NULL);
            FM_UNLOCK(cmd_buf_lock);

            if (ret) {
                WCN_DBG(FM_ALT | CHIP, "mt6628_coeff_download failed\n");
                return ret;
            }
        }

        break;
    default:
        break;
    }

    return 0;
}


static fm_s32 mt6628_PowerUp(fm_u16 *chip_id, fm_u16 *device_id)
{
#define PATCH_BUF_SIZE 4096*6
    fm_s32 ret = 0;
    fm_u16 pkt_size;
    fm_u16 tmp_reg = 0;

    const fm_s8 *path_patch = NULL;
    const fm_s8 *path_coeff = NULL;
    //const fm_s8 *path_hwcoeff = NULL;
    //fm_s32 coeff_len = 0;
    fm_s32 patch_len = 0;
    fm_u8 *dsp_buf = NULL;

    FMR_ASSERT(chip_id);
    FMR_ASSERT(device_id);

    WCN_DBG(FM_DBG | CHIP, "pwr on seq......\n");

    //Wholechip FM Power Up: step 1, FM Digital Clock enable
    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_pwrup_clock_on(cmd_buf, TX_BUF_SIZE);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    FM_UNLOCK(cmd_buf_lock);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6628_pwrup_clock_on failed\n");
        return ret;
    }

#ifdef FM_DIGITAL_INPUT
    mt6628_I2s_Setting(MT6628_I2S_ON, MT6628_I2S_SLAVE, MT6628_I2S_48K);
    mt_combo_audio_ctrl(COMBO_AUDIO_STATE_2);
#endif

    //Wholechip FM Power Up: step 2, read HW version
    mt6628_read(0x62, &tmp_reg);
    mt6628_read(0x62, &tmp_reg);
    *chip_id = tmp_reg;
    *device_id = tmp_reg;
    mt6628_hw_info.chip_id = (fm_s32)tmp_reg;
    WCN_DBG(FM_NTC | CHIP, "chip_id:0x%04x\n", tmp_reg);

    if (mt6628_hw_info.chip_id != 0x6628) {
        WCN_DBG(FM_NTC | CHIP, "fm sys error, reset hw\n");
        return (-FM_EFW);
    }
    
    mt6628_hw_info.eco_ver = (fm_s32)mtk_wcn_wmt_hwver_get();
    WCN_DBG(FM_NTC | CHIP, "ECO version:0x%08x\n", mt6628_hw_info.eco_ver);
    mt6628_hw_info.eco_ver += 1;
    
    //get mt6628 DSP rom version
    if ((ret = mt6628_get_rom_version()) >= 0) {
        mt6628_hw_info.rom_ver = ret;
        WCN_DBG(FM_NTC | CHIP, "ROM version: v%d\n", mt6628_hw_info.rom_ver);
    } else {
        WCN_DBG(FM_ERR | CHIP, "get ROM version failed\n");
    }


    //Wholechip FM Power Up: step 3, download patch
    if (!(dsp_buf = fm_zalloc(PATCH_BUF_SIZE))) {
        WCN_DBG(FM_ALT | CHIP, "-ENOMEM\n");
        return -ENOMEM;
    }

    ret = mt6628_get_patch_path(mt6628_hw_info.rom_ver, &path_patch);
    patch_len = fm_file_read(path_patch, dsp_buf, PATCH_BUF_SIZE, 0);
    mt6628_DspPatch((const fm_u8*)dsp_buf, patch_len, IMG_PATCH);

    ret = mt6628_get_coeff_path(mt6628_hw_info.rom_ver, &path_coeff);
    patch_len = fm_file_read(path_coeff, dsp_buf, PATCH_BUF_SIZE, 0);

    mt6628_hw_info.rom_ver += 1;
    
    tmp_reg = dsp_buf[38] | (dsp_buf[39] << 8);
    mt6628_hw_info.patch_ver = (fm_s32)tmp_reg;
    WCN_DBG(FM_NTC | CHIP, "Patch version: 0x%08x\n", mt6628_hw_info.patch_ver);
    
    if (ret == 1) {
        dsp_buf[4] = 0x00;  //if we found rom version undefined, we should disable patch
        dsp_buf[5] = 0x00;
    }
    
    mt6628_DspPatch((const fm_u8*)dsp_buf, patch_len, IMG_COEFFICIENT);
    mt6628_write(0x90, 0x0040);
    mt6628_write(0x90, 0x0000);

    if (dsp_buf) {
        fm_free(dsp_buf);
        dsp_buf = NULL;
    }

    //mt6628_check_dsp();
    
    //Wholechip FM Power Up: step 4, FM Digital Init: fm_rgf_maincon
    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_pwrup_digital_init(cmd_buf, TX_BUF_SIZE);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    FM_UNLOCK(cmd_buf_lock);
    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6628_pwrup_digital_init failed\n");
        return ret;;
    }

    WCN_DBG(FM_NTC | CHIP, "pwr on seq ok\n");

    //mt6628_check_dsp();

    rssi_th_set = fm_false;
    return ret;
}

static fm_s32 mt6628_PowerDown(void)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;
    fm_u16 dataRead;

    WCN_DBG(FM_DBG | CHIP, "pwr down seq\n");
    /*SW work around for MCUFA issue.
     *if interrupt happen before doing rampdown, DSP can't switch MCUFA back well.
     * In case read interrupt, and clean if interrupt found before rampdown.
     */
    mt6628_read(FM_MAIN_INTR, &dataRead);

    if (dataRead & 0x1) {
        mt6628_write(FM_MAIN_INTR, dataRead);//clear status flag
    }

    mt6628_RampDown();

#ifdef FM_DIGITAL_INPUT
    mt6628_I2s_Setting(MT6628_I2S_OFF, MT6628_I2S_SLAVE, MT6628_I2S_48K);
#endif

    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_pwrdown(cmd_buf, TX_BUF_SIZE);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    FM_UNLOCK(cmd_buf_lock);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6628_pwrdown failed\n");
        return ret;
    }

    //FIX_ME, disable ext interrupt
    mt6628_write(FM_MAIN_EXTINTRMASK, 0x00);

    rssi_th_set = fm_false;
    return ret;
}

static fm_bool mt6628_SetFreq(fm_u16 freq)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;
    fm_u16 chan_para = 0;

    fm_cb_op->cur_freq_set(freq);

    //start tune
    FM_LOCK(cmd_buf_lock);

    if (fm_cb_op->chan_para_get) {
        chan_para = fm_cb_op->chan_para_get(freq);
        WCN_DBG(FM_DBG | CHIP, "%d chan para = %d\n", (fm_s32)freq, (fm_s32)chan_para);
    }

    pkt_size = mt6628_tune(cmd_buf, TX_BUF_SIZE, freq, chan_para);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_TUNE | FLAG_TUNE_DONE, SW_RETRY_CNT, TUNE_TIMEOUT, NULL);
    FM_UNLOCK(cmd_buf_lock);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6628_tune failed\n");
        return ret;
    }

    WCN_DBG(FM_DBG | CHIP, "set freq to %d ok\n", freq);

    if (rssi_th_set == fm_false) {
        mt6628_set_RSSITh(fm_cust_config_fetch(FM_CFG_RX_RSSI_TH_LONG), fm_cust_config_fetch(FM_CFG_RX_RSSI_TH_SHORT));
        mt6628_set_SMGTh(Chip_Version, fm_cust_config_fetch(FM_CFG_RX_SMG_TH));
        rssi_th_set = fm_true;
    }

    return fm_true;
}


/*
* mt6628_Seek
* @pFreq - IN/OUT parm, IN start freq/OUT seek valid freq
* @seekdir - 0:up, 1:down
* @space - 1:50KHz, 2:100KHz, 4:200KHz
* return fm_true:seek success; fm_false:seek failed
*/
static fm_bool mt6628_Seek(fm_u16 min_freq, fm_u16 max_freq, fm_u16 *pFreq, fm_u16 seekdir, fm_u16 space)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    mt6628_Mute(fm_true);

    mt6628_RampDown();

    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_seek(cmd_buf, TX_BUF_SIZE, seekdir, space, max_freq, min_freq);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_SEEK | FLAG_SEEK_DONE, SW_RETRY_CNT, SEEK_TIMEOUT, mt6628_get_read_result);
    FM_UNLOCK(cmd_buf_lock);

    if (!ret && res) {
        *pFreq = res->seek_result;
        //fm_cb_op->cur_freq_set(*pFreq);
    } else {
        WCN_DBG(FM_ALT | CHIP, "mt6628_seek failed\n");
        return ret;
    }

    //get the result freq
    WCN_DBG(FM_NTC | CHIP, "seek, result freq:%d\n", *pFreq);
    mt6628_RampDown();
    mt6628_Mute(fm_false);

    return fm_true;
}

static fm_bool mt6628_Scan(fm_u16 min_freq, fm_u16 max_freq, fm_u16 *pFreq, fm_u16 *pScanTBL,
                           fm_u16 *ScanTBLsize, fm_u16 scandir, fm_u16 space)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;
    fm_u16 offset = 0;
    fm_u16 tmp_scanTBLsize = *ScanTBLsize;

    if ((!pScanTBL) || (tmp_scanTBLsize == 0)) {
        WCN_DBG(FM_ALT | CHIP, "scan, failed:invalid scan table\n");
        return fm_false;
    }

    WCN_DBG(FM_DBG | CHIP, "start freq: %d, max_freq:%d, min_freq:%d, scan BTL size:%d, scandir:%d, space:%d\n", *pFreq, max_freq, min_freq, *ScanTBLsize, scandir, space);

    mt6628_Mute(fm_true);

    mt6628_RampDown();

    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_scan(cmd_buf, TX_BUF_SIZE, scandir, space, max_freq, min_freq);
    ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_SCAN | FLAG_SCAN_DONE, SW_RETRY_CNT, SCAN_TIMEOUT, mt6628_get_read_result);
    FM_UNLOCK(cmd_buf_lock);

    if (!ret && res) {
        fm_memcpy(pScanTBL, res->scan_result, sizeof(fm_u16)*FM_SCANTBL_SIZE);
        WCN_DBG(FM_NTC | CHIP, "Rx scan result:\n");

        for (offset = 0; offset < tmp_scanTBLsize; offset++) {
            WCN_DBG(FM_NTC | CHIP, "%d: %04x\n", (fm_s32)offset, *(pScanTBL + offset));
        }

        *ScanTBLsize = tmp_scanTBLsize;
    } else {
        WCN_DBG(FM_ALT | CHIP, "mt6628_scan failed\n");
        return ret;
    }

    mt6628_set_bits(FM_MAIN_CTRL, 0x0000, 0xFFF0); //make sure tune/seek/scan/cqi bits = 0
        
    mt6628_Mute(fm_false);

    return fm_true;
}

//add for scan cancel case
static fm_bool cqi_abort = fm_false;

static fm_s32 mt6628_CQI_Get(fm_s8 *buf, fm_s32 buf_len)
{
    fm_s32 ret = 0;
    fm_s32 i;
    fm_u16 pkt_size;
    struct mt6628_fm_cqi *pmt6628_cqi;
    struct adapt_fm_cqi  *pcqi;

    if (!buf || buf_len < FM_CQI_BUF_SIZE) {
        return -FM_EBUF;
    }

    FM_LOCK(cmd_buf_lock);
    pkt_size = mt6628_cqi_get(cmd_buf, TX_BUF_SIZE);
    if (cqi_abort == fm_true) {
        cqi_abort = fm_false;
        ret = -1;
    } else {
        ret = fm_cmd_tx(cmd_buf, pkt_size, FLAG_SCAN | FLAG_CQI_DONE, SW_RETRY_CNT, SCAN_TIMEOUT, mt6628_get_read_result);
    }
    FM_UNLOCK(cmd_buf_lock);

    if (!ret && res) {
        //FIXEDME
        pmt6628_cqi = (struct mt6628_fm_cqi*)res->cqi;
        pcqi = (struct adapt_fm_cqi*)buf;

        for (i = 0; i < (sizeof(res->cqi) / sizeof(struct mt6628_fm_cqi)); i++) {
            pcqi[i].ch = (pmt6628_cqi[i].ch * 10 / 2) + 6400;
            pcqi[i].rssi = (fm_s32)pmt6628_cqi[i].rssi;

            if (pcqi[i].rssi >= 32768) {
                pcqi[i].rssi = pcqi[i].rssi - 65536;
            }

            pcqi[i].rssi = ((pcqi[i].rssi * 6) >> 4);
            WCN_DBG(FM_NTC | CHIP, "%d --> %d(dbm)\n", pcqi[i].ch, pcqi[i].rssi);
        }
    } else {
        WCN_DBG(FM_ALT | CHIP, "mt6628 get CQI failed\n");
    }

    mt6628_set_bits(FM_MAIN_CTRL, 0x0000, 0xFFF0); //make sure tune/seek/scan/cqi bits = 0

    return ret;
}

static fm_bool scan_abort = fm_false;

#ifdef MTK_FM_50KHZ_SUPPORT
#define SCAN_SEG_LEN 250
static fm_s8 raw_buf[16 * sizeof(struct adapt_fm_cqi)] = {0};

static fm_bool mt6628_Scan_50KHz(fm_u16 min_freq, fm_u16 max_freq, fm_u16 *pFreq, fm_u16 *pScanTBL,
                           fm_u16 *ScanTBLsize, fm_u16 scandir, fm_u16 space)
{
    fm_s32 ret = 0;
    fm_s32 num;
    fm_s32 seg;
    fm_s32 i, j;
    fm_u16 scan_tbl[FM_SCANTBL_SIZE]; //need no less than the chip
    fm_s32 start_freq, end_freq;
    fm_s32 ch_offset, step, tmp_val;
    fm_s32 chl_cnt = 0;
    fm_s32 word_offset, bit_offset;
    fm_s32 space_val = 5;
    struct adapt_fm_cqi *pCQI = (struct adapt_fm_cqi *)raw_buf;

    if (space == 0x0001) {
        space_val = 5; // 50Khz
    } else if (space == 0x0002) {
        space_val = 10; // 100Khz
    } else if (space == 0x0004) {
        space_val = 20; // 200Khz
    }
    
    // calculate segment number
    num = (max_freq - min_freq) / space_val; //Eg, (10800 - 8750) / 5 = 410
    seg = (num / SCAN_SEG_LEN) + ((num % SCAN_SEG_LEN) ? 1 : 0); //Eg, (410 / 200) + ((410 % 200) ? 1 : 0) = 2 + 1 = 3

    FM_FIFO_RESET(cqi_fifo);
    fm_memset(pScanTBL, 0, sizeof(fm_u16) * (*ScanTBLsize));
    
    // do scan
    scan_abort = fm_false; // reset scan cancel flag
    for (i = 0; i < seg; i++) {
        start_freq = min_freq + SCAN_SEG_LEN * space_val * i;
        end_freq = min_freq + SCAN_SEG_LEN * space_val * (i + 1) - space_val;
        end_freq = (end_freq > max_freq) ? max_freq : end_freq;
        chl_cnt = 0;

        if (fm_true == scan_abort) {
            scan_abort = fm_false;
            return fm_false;
        }
        
        if (fm_false == mt6628_Scan(start_freq, end_freq, pFreq, scan_tbl, ScanTBLsize, scandir, space)) {
            return fm_false;
        }

        // get channel count
        for (ch_offset = 0; ch_offset < FM_SCANTBL_SIZE; ch_offset++) {
		    if (scan_tbl[ch_offset] == 0)
			    continue;
		    for (step = 0; step < 16; step++) {
			    if (scan_tbl[ch_offset] & (1 << step)) {
                    tmp_val =  start_freq + (ch_offset * 16 + step) * space_val;
                    WCN_DBG(FM_NTC | CHIP, "freq %d, end freq %d\n", tmp_val, end_freq);
                    if (tmp_val <= end_freq) {
                        chl_cnt++; 
                        // set reult bitmap
                        word_offset = (tmp_val - min_freq) / space_val / 16;
                        bit_offset = (tmp_val - min_freq) / space_val % 16;
                        if ((word_offset < 26) && (word_offset >= 0)) {
                            pScanTBL[word_offset] |= (1 << bit_offset);
                        }
                        WCN_DBG(FM_NTC | CHIP, "cnt %d, word %d, bit %d\n", chl_cnt, word_offset, bit_offset);
                    }
			    }
		    }
	    }

        // get cqi info
        while (chl_cnt > 0) {
            ret = mt6628_CQI_Get(raw_buf, 16 * sizeof(struct adapt_fm_cqi));
            if (ret) {
                return ret;
            }

        // add valid channel to cqi_fifo
        for (j = 0; j < sizeof(raw_buf) / sizeof(struct adapt_fm_cqi); j++) {
            if ((pCQI[j].ch >= start_freq) && (pCQI[j].ch <= end_freq)) {
                FM_FIFO_INPUT(cqi_fifo, pCQI + j);
                WCN_DBG(FM_NTC | CHIP, "%d %d(dbm) add to fifo\n", pCQI[j].ch, pCQI[j].rssi);
            }
        }
            
            chl_cnt -= 16;
        }
    }
    
    return fm_true;
}


static fm_s32 mt6628_CQI_Get_50KHz(fm_s8 *buf, fm_s32 buf_len)
{
    fm_s32 ret = 0;
    fm_s32 i;
    struct adapt_fm_cqi tmp = {
        .ch = 0,
        .rssi = 0,
    };
    struct adapt_fm_cqi *pcqi = (struct adapt_fm_cqi *)buf;

    
    if (!buf || buf_len < FM_CQI_BUF_SIZE) {
        return -FM_EBUF;
    }

    for (i = 0; ((i < (buf_len / sizeof(struct adapt_fm_cqi))) && \
        (fm_false == FM_FIFO_IS_EMPTY(cqi_fifo))); i++) {
        FM_FIFO_OUTPUT(cqi_fifo, &tmp);
        pcqi[i].ch = tmp.ch;
        pcqi[i].rssi = tmp.rssi;
        WCN_DBG(FM_NTC | CHIP, "%d %d(dbm) get from fifo\n", pcqi[i].ch, pcqi[i].rssi);
    }
    
    return ret;
}

#endif //MTK_FM_50KHZ_SUPPORT


/*
 * mt6628_GetCurRSSI - get current freq's RSSI value
 * RS=RSSI
 * If RS>511, then RSSI(dBm)= (RS-1024)/16*6
 *				   else RSSI(dBm)= RS/16*6
 */
static fm_s32 mt6628_GetCurRSSI(fm_s32 *pRSSI)
{
    fm_u16 tmp_reg;

    mt6628_read(FM_RSSI_IND, &tmp_reg);
    tmp_reg = tmp_reg & 0x03ff;

    if (pRSSI) {
        *pRSSI = (tmp_reg > 511) ? (((tmp_reg - 1024) * 6) >> 4) : ((tmp_reg * 6) >> 4);
        WCN_DBG(FM_DBG | CHIP, "rssi:%d, dBm:%d\n", tmp_reg, *pRSSI);
    } else {
        WCN_DBG(FM_ERR | CHIP, "get rssi para error\n");
        return -FM_EPARA;
    }

    return 0;
}

static fm_u16 mt6628_vol_tbl[16] = { \
    0x0000, 0x0519, 0x066A, 0x0814, \
    0x0A2B, 0x0CCD, 0x101D, 0x1449, \
    0x198A, 0x2027, 0x287A, 0x32F5, \
    0x4027, 0x50C3, 0x65AD, 0x7FFF
};
static fm_s32 mt6628_SetVol(fm_u8 vol)
{
    fm_s32 ret = 0;
    
    vol = (vol > 15) ? 15 : vol;
    ret = mt6628_write(0x7D, mt6628_vol_tbl[vol]); 
    if (ret) {
        WCN_DBG(FM_ERR | CHIP, "Set vol=%d Failed\n", vol);
        return ret;
    } else {
        WCN_DBG(FM_DBG | CHIP, "Set vol=%d OK\n", vol);
    }

    if (vol == 10) {
        fm_print_cmd_fifo();    //just for debug
        fm_print_evt_fifo();
    }
    return 0;
}

static fm_s32 mt6628_GetVol(fm_u8 *pVol)
{
    int ret = 0;
    fm_u16 tmp;
    fm_s32 i;

    FMR_ASSERT(pVol);

    ret = mt6628_read(0x7D, &tmp);
    if (ret) {
        *pVol = 0;
        WCN_DBG(FM_ERR | CHIP, "Get vol Failed\n");
        return ret;
    } 

    for (i = 0; i < 16; i++) {
        if (mt6628_vol_tbl[i] == tmp) {
            *pVol = i;
            break;
        }
    }

    WCN_DBG(FM_DBG | CHIP, "Get vol=%d OK\n", *pVol);
    return 0;
}

static fm_s32 mt6628_dump_reg(void)
{
    return 0;
}

static fm_bool mt6628_GetMonoStereo(fm_u16 *pMonoStereo)
{
#define FM_BF_STEREO 0x1000
    fm_u16 TmpReg;

    if (pMonoStereo) {
        mt6628_read(FM_RSSI_IND, &TmpReg);
        *pMonoStereo = (TmpReg & FM_BF_STEREO) >> 12;
    } else {
        WCN_DBG(FM_ERR | CHIP, "MonoStero: para err\n");
        return fm_false;
    }

    WCN_DBG(FM_DBG | CHIP, "MonoStero:0x%04x\n", *pMonoStereo);
    return fm_true;
}

static fm_s32 mt6628_SetMonoStereo(fm_s32 MonoStereo)
{
    fm_s32 ret = 0;
#define FM_FORCE_MS 0x0008

    WCN_DBG(FM_DBG | CHIP, "set to %s\n", MonoStereo ? "mono" : "auto");

    mt6628_write(0x60, 0x3007);

    if (MonoStereo) {
        ret = mt6628_set_bits(0x75, FM_FORCE_MS, ~FM_FORCE_MS);
    } else {
        ret = mt6628_set_bits(0x75, 0x0000, ~FM_FORCE_MS);
    }

    return ret;
}

static fm_s32 mt6628_GetCapArray(fm_s32 *ca)
{
    fm_u16 dataRead;
    fm_u16 tmp = 0;

    FMR_ASSERT(ca);
    mt6628_read(0x60, &tmp);
    mt6628_write(0x60, tmp&0xFFF7); //0x60 D3=0

    mt6628_read(0x26, &dataRead);
    *ca = dataRead;

    mt6628_write(0x60, tmp); //0x60 D3=1
    return 0;
}

/*
 * mt6628_GetCurPamd - get current freq's PAMD value
 * PA=PAMD
 * If PA>511 then PAMD(dB)=  (PA-1024)/16*6,
 *				else PAMD(dB)=PA/16*6
 */
static fm_bool mt6628_GetCurPamd(fm_u16 *pPamdLevl)
{
    fm_u16 tmp_reg;
    fm_u16 dBvalue;

    if (mt6628_read(FM_ADDR_PAMD, &tmp_reg))
        return fm_false;

    tmp_reg &= 0x03FF;
    dBvalue = (tmp_reg > 511) ? ((1024 - tmp_reg) * 6 / 16) : 0;

    *pPamdLevl = dBvalue;
    return fm_true;
}

static fm_s32 mt6628_ScanStop(void)
{
    cqi_abort = fm_true;
    scan_abort = fm_true;
    fm_force_active_event(FLAG_SCAN_DONE | FLAG_CQI_DONE);
    
    return 0;
}

static fm_s32 mt6628_SeekStop(void)
{
    return fm_force_active_event(FLAG_SEEK_DONE);
}


/*
 * mt6628_I2s_Setting - set the I2S state on MT6628
 * @onoff - I2S on/off
 * @mode - I2S mode: Master or Slave
 *
 * Return:0, if success; error code, if failed
 */
static fm_s32 mt6628_I2s_Setting(fm_s32 onoff, fm_s32 mode, fm_s32 sample)
{
    fm_u16 tmp_state = 0;
    fm_u16 tmp_mode = 0;
    fm_u16 tmp_sample = 0;
    fm_s32 ret = 0;

    if (onoff == MT6628_I2S_ON) {
        tmp_state = 0x0080; //I2S Frequency tracking on, 0x61 D7=1
        mt6628_i2s_inf.status = 1;
    } else if (onoff == MT6628_I2S_OFF) {
        tmp_state = 0x0000; //I2S Frequency tracking off, 0x61 D7=0
        mt6628_i2s_inf.status = 0;
    } else {
        WCN_DBG(FM_ERR | CHIP, "%s():[onoff=%d]\n", __func__, onoff);
        ret = -FM_EPARA;
        goto out;
    }

    if (mode == MT6628_I2S_MASTER) {
        tmp_mode = 0x0000; //6620 as I2S master, set 0x9B D3=0
        mt6628_i2s_inf.mode = 1;
    } else if (mode == MT6628_I2S_SLAVE) {
        tmp_mode = 0x0008; //6620 as I2S slave, set 0x9B D3=1
        mt6628_i2s_inf.mode = 0;
    } else {
        WCN_DBG(FM_ERR | CHIP, "%s():[mode=%d]\n", __func__, mode);
        ret = -FM_EPARA;
        goto out;
    }

    if (sample == MT6628_I2S_32K) {
        tmp_sample = 0x0000; //6620 I2S 32KHz sample rate, 0x5F D11~12
        mt6628_i2s_inf.rate = 32000;
    } else if (sample == MT6628_I2S_44K) {
        tmp_sample = 0x0800; //6620 I2S 44.1KHz sample rate
        mt6628_i2s_inf.rate = 44100;
    } else if (sample == MT6628_I2S_48K) {
        tmp_sample = 0x1000; //6620 I2S 48KHz sample rate
        mt6628_i2s_inf.rate = 48000;
    } else {
        WCN_DBG(FM_ERR | CHIP, "%s():[sample=%d]\n", __func__, sample);
        ret = -FM_EPARA;
        goto out;
    }

    if ((ret = mt6628_set_bits(0x5F, tmp_sample, 0xE7FF)))
        goto out;

    if ((ret = mt6628_set_bits(0x9B, tmp_mode, 0xFFF7)))
        goto out;

    if ((ret = mt6628_set_bits(0x61, tmp_state, 0xFF7F)))
        goto out;

    WCN_DBG(FM_NTC | CHIP, "[onoff=%s][mode=%s][sample=%d](0)33KHz,(1)44.1KHz,(2)48KHz\n",
            (onoff == MT6628_I2S_ON) ? "On" : "Off",
            (mode == MT6628_I2S_MASTER) ? "Master" : "Slave",
            sample);
out:
    return ret;
}



static fm_s32 mt6628_i2s_info_get(fm_s32 *ponoff, fm_s32 *pmode, fm_s32 *psample)
{
    FMR_ASSERT(ponoff);
    FMR_ASSERT(pmode);
    FMR_ASSERT(psample);

    *ponoff = mt6628_i2s_inf.status;
    *pmode = mt6628_i2s_inf.mode;
    *psample = mt6628_i2s_inf.rate;

    return 0;
}


static fm_s32 mt6628_hw_info_get(struct fm_hw_info *req)
{
    FMR_ASSERT(req);

    req->chip_id = mt6628_hw_info.chip_id;
    req->eco_ver = mt6628_hw_info.eco_ver;
    req->patch_ver = mt6628_hw_info.patch_ver;
    req->rom_ver = mt6628_hw_info.rom_ver;

    return 0;
}


static fm_bool mt6628_em_test(fm_u16 group_idx, fm_u16 item_idx, fm_u32 item_value)
{
    return fm_true;
}

static fm_s32 fm_low_power_wa_default(fm_s32 fmon)
{
    return 0;
}

fm_s32 fm_low_ops_register(struct fm_lowlevel_ops *ops)
{
    fm_s32 ret = 0;
    //Basic functions.

    FMR_ASSERT(ops);
    FMR_ASSERT(ops->cb.cur_freq_get);
    FMR_ASSERT(ops->cb.cur_freq_set);
    fm_cb_op = &ops->cb;

    //ops->bi.low_pwr_wa = mt6628_low_pwr_wa;
    ops->bi.low_pwr_wa = fm_low_power_wa_default;
    ops->bi.pwron = mt6628_pwron;
    ops->bi.pwroff = mt6628_pwroff;
    ops->bi.msdelay = Delayms;
    ops->bi.usdelay = Delayus;
    ops->bi.read = mt6628_read;
    ops->bi.write = mt6628_write;
    ops->bi.setbits = mt6628_set_bits;
    ops->bi.chipid_get = mt6628_get_chipid;
    ops->bi.mute = mt6628_Mute;
    ops->bi.rampdown = mt6628_RampDown;
    ops->bi.pwrupseq = mt6628_PowerUp;
    ops->bi.pwrdownseq = mt6628_PowerDown;
    ops->bi.setfreq = mt6628_SetFreq;
    ops->bi.seek = mt6628_Seek;
    ops->bi.seekstop = mt6628_SeekStop;
    ops->bi.scan = mt6628_Scan;
    ops->bi.cqi_get = mt6628_CQI_Get;
    #ifdef MTK_FM_50KHZ_SUPPORT
    ops->bi.scan = mt6628_Scan_50KHz;
    ops->bi.cqi_get = mt6628_CQI_Get_50KHz;
    #endif
    ops->bi.scanstop = mt6628_ScanStop;
    ops->bi.rssiget = mt6628_GetCurRSSI;
    ops->bi.volset = mt6628_SetVol;
    ops->bi.volget = mt6628_GetVol;
    ops->bi.dumpreg = mt6628_dump_reg;
    ops->bi.msget = mt6628_GetMonoStereo;
    ops->bi.msset = mt6628_SetMonoStereo;
    ops->bi.pamdget = mt6628_GetCurPamd;
    ops->bi.em = mt6628_em_test;
    ops->bi.anaswitch = mt6628_SetAntennaType;
    ops->bi.anaget = mt6628_GetAntennaType;
    ops->bi.caparray_get = mt6628_GetCapArray;
    ops->bi.i2s_set = mt6628_I2s_Setting;
    ops->bi.hwinfo_get = mt6628_hw_info_get;
    ops->bi.i2s_get = mt6628_i2s_info_get;

    cmd_buf_lock = fm_lock_create("fm_lib_buf");
    ret = fm_lock_get(cmd_buf_lock);

    cmd_buf = fm_zalloc(TX_BUF_SIZE + 1);

    if (!cmd_buf) {
        WCN_DBG(FM_ALT | CHIP, "6628 fm lib alloc tx buf failed\n");
        ret = -1;
    }

#ifdef MTK_FM_50KHZ_SUPPORT
    cqi_fifo = fm_fifo_create("6628_cqi_fifo", sizeof(struct adapt_fm_cqi), 640);
    if (!cqi_fifo) {
        WCN_DBG(FM_ALT | CHIP, "6628 fm lib create cqi fifo failed\n");
        ret = -1;
    }
#endif

    return ret;
}

fm_s32 fm_low_ops_unregister(struct fm_lowlevel_ops *ops)
{
    fm_s32 ret = 0;
    //Basic functions.
    FMR_ASSERT(ops);

#ifdef MTK_FM_50KHZ_SUPPORT
    fm_fifo_release(cqi_fifo);
#endif

    if (cmd_buf) {
        fm_free(cmd_buf);
        cmd_buf = NULL;
    }

    ret = fm_lock_put(cmd_buf_lock);
    fm_memset(&ops->bi, 0, sizeof(struct fm_basic_interface));
    return ret;
}

