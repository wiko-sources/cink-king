#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/fm.h>

#include "mtkstp.h"
#include "stp_drv.h"
#include "wmt_exp.h"

#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_interface.h"
#include "fm_stdlib.h"
#include "fm_patch.h"

#include "mt6620_fm_reg.h"
#include "mt6620_fm.h"
#include "mt6620_drv_dsp.h"
#include "mt6620_fm_link.h"
#include "mt6620_fm_lib.h"
#include "mt6620_fm_cmd.h"

static DECLARE_MUTEX(fm_cmd_mutex); //protect FM HW access

struct fm_buf {
    fm_u8 *buf;
    fm_s32 size;
    fm_s32(*construct)(struct fm_buf *thiz);
    fm_s32(*lock)(struct fm_buf *thiz, void *data);
    fm_s32(*unlock)(struct fm_buf *thiz, void *data);
    fm_s32(*destruct)(struct fm_buf *thiz);
};

static fm_s32 mt6620_buf_lock(struct fm_buf *thiz, void *data)
{
    WCN_DBG(FM_DBG | CHIP, "...lock\n");

    if (down_interruptible(&fm_cmd_mutex)) {
        WCN_DBG(FM_CRT | CHIP, "get mutex failed\n");
        return -FM_ELOCK;
    }

    fm_memset(thiz->buf, 0, TX_BUF_SIZE); //clear the buffer before use

    return 0;
}

static fm_s32 mt6620_buf_unlock(struct fm_buf *thiz, void *data)
{
    up(&fm_cmd_mutex);
    WCN_DBG(FM_DBG | CHIP, "...unlock\n");

    return 0;
}

static fm_u8 buffer[TX_BUF_SIZE+1];

static struct fm_buf cmd_buf = {
    .buf = buffer,
    .size = TX_BUF_SIZE,
    .lock = mt6620_buf_lock,
    .unlock = mt6620_buf_unlock,
};

#define BUF_LOCK(a)         \
{                           \
    if((a)->lock){          \
        (a)->lock((a), NULL);    \
    }                       \
}

#define BUF_UNLOCK(a)         \
{                             \
    if((a)->unlock){          \
        (a)->unlock((a), NULL);    \
    }                       \
}

static struct fm_callback *fm_cb_op;

static fm_s32 mt6620_pwron(fm_s32 data)
{
    if (MTK_WCN_BOOL_FALSE == mtk_wcn_wmt_func_on(WMTDRV_TYPE_FM)) {
        WCN_DBG(FM_ALT | CHIP, "WMT turn on FM Fail!\n");
        return -FM_ELINK;
    } else {
        WCN_DBG(FM_ALT | CHIP, "WMT turn on FM OK!\n");
        return 0;
    }
}


static fm_s32 mt6620_pwroff(fm_s32 data)
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

static struct fm_res_ctx *res = NULL;

fm_s32 mt6620_get_read_result(struct fm_res_ctx* result)
{
    FMR_ASSERT(result);
    res = result;

    return 0;
}

static fm_s32 mt6620_read(fm_u8 addr, fm_u16 *val)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_get_reg(cmd_buf.buf, cmd_buf.size, addr);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_FSPI_RD, SW_RETRY_CNT, FSPI_RD_TIMEOUT, mt6620_get_read_result);

    if (!ret && res) {
        *val = res->fspi_rd;
    }

    BUF_UNLOCK(&cmd_buf);

    return ret;
}

static fm_s32 mt6620_write(fm_u8 addr, fm_u16 val)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_set_reg(cmd_buf.buf, cmd_buf.size, addr, val);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_FSPI_WR, SW_RETRY_CNT, FSPI_WR_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    return ret;
}

static fm_s32 mt6620_set_bits(fm_u8 addr, fm_u16 bits, fm_u16 mask)
{
    fm_s32 ret = 0;
    fm_u16 val;

    ret = mt6620_read(addr, &val);

    if (ret)
        return ret;

    val = ((val & (mask)) | bits);
    ret = mt6620_write(addr, val);

    return ret;
}

static fm_u16 mt6620_get_chipid(void)
{
    return 0x6620;
}

/*  mt6620_SetAntennaType - set Antenna type
 *  @type - 1,Short Antenna;  0, Long Antenna
 */
static fm_s32 mt6620_SetAntennaType(fm_s32 type)
{
    WCN_DBG(FM_DBG | CHIP, "set ana to %s\n", type ? "short" : "long");

    if (type == FM_LONG_ANA) {
        //Long Antenna RSSI threshold, 0xE0 D0~D9
        mt6620_write(0xE0, ((0xA301 & 0xFC00) | (FMR_RSSI_TH_LONG & 0x03FF)));
        //Turn on Short Antenna LNA and Off TR Switch
        mt6620_write(0x04, 0x0142);
        //Turn off the Short Antenna Capbank biasing
        mt6620_write(0x05, 0x00E7);
        //Turn off the Short Antenna Capbank biasing
        mt6620_write(0x26, 0x0004);
        //Disable concurrent calibration for VCO and SCAL
        mt6620_write(0x2E, 0x0008);
    } else if (type == FM_SHORT_ANA) {
        //Short Antenna RSSI threshold, 0xE0 D0~D9
        mt6620_write(0xE0, ((0xA2E0 & 0xFC00) | (FMR_RSSI_TH_SHORT & 0x03FF)));
        //Turn on Short Antenna LNA and TR Switch
        mt6620_write(0x04, 0x0145);
        //Turn on the Short Antenna Capbank biasing
        mt6620_write(0x05, 0x00FF);
        //Turn on the Short Antenna Capbank biasing
        mt6620_write(0x26, 0x0024);
        //Enable concurrent calibration for VCO and SCAL
        mt6620_write(0x2E, 0x0000);
    } else {
        WCN_DBG(FM_ERR | CHIP, "%s()\n", __func__);
        return -FM_EPARA;
    }

    return 0;
}

static fm_s32 mt6620_GetAntennaType(void)
{
    fm_u16 dataRead;

    mt6620_read(0x2E, &dataRead);

    if (dataRead == 0x0000)
        return FM_SHORT_ANA; //short antenna
    else
        return FM_LONG_ANA; //long antenna
}

static fm_s32 mt6620_Mute(fm_bool mute)
{
    WCN_DBG(FM_DBG | CHIP, "set %s\n", mute ? "mute" : "unmute");

    if (mute) {
        return mt6620_set_bits(0x9C, 0x0008, 0xFFF7); //1:9C D3 = 1
    } else {
        return mt6620_set_bits(0x9c, 0x0000, 0xFFF7); //1:9C D3 = 0
    }
}

static fm_s32 mt6620_RampDown(void)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    WCN_DBG(FM_DBG | CHIP, "ramp down\n");

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_rampdown(cmd_buf.buf, cmd_buf.size);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_RAMPDOWN, SW_RETRY_CNT, RAMPDOWN_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ERR | CHIP, "ramp down failed\n");
    }

    return ret;
}

static fm_s32 mt6620_PowerUp(fm_u16 *chip_id, fm_u16 *device_id)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    FMR_ASSERT(chip_id);
    FMR_ASSERT(device_id);

    WCN_DBG(FM_DBG | CHIP, "pwr on seq......\n");

    //Wholechip FM Power Up: step 1, mt6620_off_2_longANA
    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_off_2_longANA_1(cmd_buf.buf, cmd_buf.size);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_off_2_longANA_1 failed\n");
        return ret;
    }

    Delayms(100);

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_off_2_longANA_2(cmd_buf.buf, cmd_buf.size);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_off_2_longANA_2 failed\n");
        return ret;
    }

    Delayms(50);

    *chip_id = 0x6620;
    *device_id = 0x6620;
    WCN_DBG(FM_NTC | CHIP, "chip_id:0x%04x\n", 0x6620);

    //Wholechip FM Power Up: step 2, FM Digital Init: fm_rgf_maincon
    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_pwrup_digital_init_1(cmd_buf.buf, cmd_buf.size);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_pwrup_digital_init_2 failed\n");
        return ret;;
    }

    Delayms(10);

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_pwrup_digital_init_2(cmd_buf.buf, cmd_buf.size);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_pwrup_digital_init_2 failed\n");
        return ret;;
    }

    Delayms(10);

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_pwrup_digital_init_3(cmd_buf.buf, cmd_buf.size);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_pwrup_digital_init_3 failed\n");
        return ret;;
    }

    WCN_DBG(FM_DBG | CHIP, "pwr on seq ok\n");
    return ret;
}

static fm_s32 mt6620_PowerDown(void)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    WCN_DBG(FM_DBG | CHIP, "pwr down seq\n");

    mt6620_RampDown();

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_pwrdown(cmd_buf.buf, cmd_buf.size);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_EN, SW_RETRY_CNT, EN_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_pwrdown failed\n");
        return ret;
    }

    return ret;
}

static fm_bool mt6620_SetFreq(fm_u16 freq)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    fm_cb_op->cur_freq_set(freq);

    //start tune
    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_tune_1(cmd_buf.buf, cmd_buf.size, freq);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_TUNE, SW_RETRY_CNT, TUNE_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_tune_1 failed\n");
        return ret;;
    }

    Delayms(200);

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_tune_2(cmd_buf.buf, cmd_buf.size, freq);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_TUNE, SW_RETRY_CNT, TUNE_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_tune_2 failed\n");
        return ret;;
    }

    Delayms(35);

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_tune_3(cmd_buf.buf, cmd_buf.size, freq);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_TUNE, SW_RETRY_CNT, TUNE_TIMEOUT, NULL);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_tune_3 failed\n");
        return ret;;
    }

    WCN_DBG(FM_DBG | CHIP, "set freq to %d ok\n", freq);

    return fm_true;
}


/*
* mt6620_Seek
* @pFreq - IN/OUT parm, IN start freq/OUT seek valid freq
* @seekdir - 0:up, 1:down
* @space - 1:50KHz, 2:100KHz, 4:200KHz
* return fm_true:seek success; fm_false:seek failed
*/
static fm_bool mt6620_Seek(fm_u16 min_freq, fm_u16 max_freq, fm_u16 *pFreq, fm_u16 seekdir, fm_u16 space)
{
    fm_s32 ret = 0;
    fm_u16 pkt_size;

    mt6620_Mute(fm_true);

    mt6620_RampDown();

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_seek_1(cmd_buf.buf, cmd_buf.size, seekdir, space, max_freq, min_freq);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_SEEK | FLAG_SEEK_DONE, SW_RETRY_CNT, SEEK_TIMEOUT, mt6620_get_read_result);
    BUF_UNLOCK(&cmd_buf);

    if (!ret && res) {
        *pFreq = res->seek_result;
        fm_cb_op->cur_freq_set(*pFreq);
    } else {
        WCN_DBG(FM_ALT | CHIP, "mt6620_seek_1 failed\n");
        return ret;
    }

    Delayms(35);

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_seek_2(cmd_buf.buf, cmd_buf.size, seekdir, space, max_freq, min_freq);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_SEEK | FLAG_SEEK_DONE, SW_RETRY_CNT, SEEK_TIMEOUT, mt6620_get_read_result);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_seek_2 failed\n");
        return ret;
    }

    //get the result freq
    WCN_DBG(FM_NTC | CHIP, "seek, result freq:%d\n", *pFreq);
    mt6620_Mute(fm_false);

    return fm_true;
}

static fm_bool mt6620_Scan(fm_u16 min_freq, fm_u16 max_freq, fm_u16 *pFreq, fm_u16 *pScanTBL,
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

    mt6620_Mute(fm_true);

    mt6620_RampDown();

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_scan_1(cmd_buf.buf, cmd_buf.size, scandir, space, max_freq, min_freq);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_SCAN | FLAG_SCAN_DONE, SW_RETRY_CNT, SCAN_TIMEOUT, mt6620_get_read_result);
    BUF_UNLOCK(&cmd_buf);

    if (!ret && res) {
        fm_memcpy(pScanTBL, res->scan_result, sizeof(fm_u16)*FM_SCANTBL_SIZE);
        WCN_DBG(FM_INF | CHIP, "Rx Scan Result:\n");

        for (offset = 0; offset < tmp_scanTBLsize; offset++) {
            WCN_DBG(FM_INF | CHIP, "%d: %04x\n", (fm_s32)offset, *(pScanTBL + offset));
        }

        *ScanTBLsize = tmp_scanTBLsize;
    } else {
        WCN_DBG(FM_ALT | CHIP, "mt6620_scan_1 failed\n");
        return ret;
    }

    Delayms(35);

    BUF_LOCK(&cmd_buf);
    pkt_size = mt6620_scan_2(cmd_buf.buf, cmd_buf.size, scandir, space, max_freq, min_freq);
    ret = fm_cmd_tx(cmd_buf.buf, pkt_size, FLAG_SEEK | FLAG_SEEK_DONE, SW_RETRY_CNT, SEEK_TIMEOUT, mt6620_get_read_result);
    BUF_UNLOCK(&cmd_buf);

    if (ret) {
        WCN_DBG(FM_ALT | CHIP, "mt6620_scan_2 failed\n");
        return ret;
    }

    mt6620_Mute(fm_false);

    return fm_true;
}

/*
 * mt6620_GetCurRSSI - get current freq's RSSI value
 * RS=RSSI
 * If RS>511, then RSSI(dBm)= (RS-1024)/16*6
 *				   else RSSI(dBm)= RS/16*6
 */
static fm_s32 mt6620_GetCurRSSI(fm_s32 *pRSSI)
{
    fm_u16 tmp_reg;

    mt6620_read(FM_RSSI_IND, &tmp_reg);
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

static fm_s32 mt6620_SetVol(fm_u8 vol)
{
#define MT6620_VOL_MAX   0x2B	// 43 volume(0-15)
    int ret = 0;
    fm_u8 tmp_vol = vol & 0x3f;

    if (tmp_vol > MT6620_VOL_MAX)
        tmp_vol = MT6620_VOL_MAX;

    ret = mt6620_set_bits(0x9C, (tmp_vol << 8), 0xC0FF);

    if (ret) {
        WCN_DBG(FM_ERR | CHIP, "Set vol=%d Failed\n", tmp_vol);
        return ret;
    } else {
        WCN_DBG(FM_DBG | CHIP, "Set vol=%d OK\n", tmp_vol);
    }

    return 0;
}

static fm_s32 mt6620_GetVol(fm_u8 *pVol)
{
    int ret = 0;
    fm_u16 tmp_reg;

    FMR_ASSERT(pVol);

    ret = mt6620_read(0x9C, &tmp_reg);

    if (ret) {
        *pVol = 0;
        WCN_DBG(FM_ERR | CHIP, "Get vol Failed\n");
        return ret;
    } else {
        *pVol = (tmp_reg >> 8) & 0x3f;
        WCN_DBG(FM_DBG | CHIP, "Get vol=%d OK\n", *pVol);
    }

    return 0;

}

static fm_s32 mt6620_dump_reg(void)
{
    return 0;
}

static fm_bool mt6620_GetMonoStereo(fm_u16 *pMonoStereo)
{
    fm_u16 tmp_reg;

    mt6620_write(FM_MAIN_PGSEL, 0x0001);

    if (pMonoStereo) {
        mt6620_read(0xF8, &tmp_reg);
        *pMonoStereo = (tmp_reg & 0x0400) >> 10;
    } else {
        WCN_DBG(FM_ERR | CHIP, "MonoStero: para err\n");
        return fm_false;
    }

    mt6620_write(FM_MAIN_PGSEL, 0x0000);

    WCN_DBG(FM_DBG | CHIP, "MonoStero:0x%04x\n", *pMonoStereo);
    return fm_true;
}

static fm_s32 mt6620_GetCapArray(fm_s32 *ca)
{
    fm_u16 dataRead;

    FMR_ASSERT(ca);

    mt6620_read(0x26, &dataRead);
    *ca = dataRead;

    return 0;
}

/*
 * mt6620_GetCurPamd - get current freq's PAMD value
 * PA=PAMD
 * If PA>511 then PAMD(dB)=  (PA-1024)/16*6,
 *				else PAMD(dB)=PA/16*6
 */
static fm_bool mt6620_GetCurPamd(fm_u16 *pPamdLevl)
{
    fm_u16 tmp_reg;
    fm_u16 dBvalue;

    if (mt6620_read(FM_ADDR_PAMD, &tmp_reg))
        return fm_false;

    tmp_reg &= 0x03FF;
    dBvalue = (tmp_reg > 511) ? ((1024 - tmp_reg) * 6 / 16) : 0;

    *pPamdLevl = dBvalue;
    return fm_true;
}

static fm_s32 mt6620_ScanStop(void)
{
    return fm_force_active_event(FLAG_SCAN);
}

static fm_s32 mt6620_SeekStop(void)
{
    return fm_force_active_event(FLAG_SEEK);
}

/*
 * mt6620_I2s_Setting - set the I2S state on MT6620
 * @onoff - I2S on/off
 * @mode - I2S mode: Master or Slave
 *
 * Return:0, if success; error code, if failed
 */
static fm_s32 mt6620_I2s_Setting(fm_s32 onoff, fm_s32 mode, fm_s32 sample)
{
    fm_u16 tmp_state = 0;
    fm_u16 tmp_mode = 0;
    fm_u16 tmp_sample = 0;
    fm_s32 ret = 0;

    if (onoff == FM_I2S_ON) {
        tmp_state = 0x0080; //I2S Frequency tracking on, 0x61 D7=1
    } else if (onoff == FM_I2S_OFF) {
        tmp_state = 0x0000; //I2S Frequency tracking off, 0x61 D7=0
    } else {
        WCN_DBG(FM_ERR | CHIP, "%s():[onoff=%d]\n", __func__, onoff);
        ret = -FM_EPARA;
        goto out;
    }

    if (mode == FM_I2S_MASTER) {
        tmp_mode = 0x03; //6620 as I2S master
    } else if (mode == FM_I2S_SLAVE) {
        tmp_mode = 0x0B; //6620 as I2S slave
    } else {
        WCN_DBG(FM_ERR | CHIP, "%s():[mode=%d]\n", __func__, mode);
        ret = -FM_EPARA;
        goto out;
    }

    if (sample == FM_I2S_32K) {
        tmp_sample = 0x0000; //6620 I2S 32KHz sample rate
    } else if (sample == FM_I2S_44K) {
        tmp_sample = 0x0800; //6620 I2S 44.1KHz sample rate
    } else if (sample == FM_I2S_48K) {
        tmp_sample = 0x1000; //6620 I2S 48KHz sample rate
    } else {
        WCN_DBG(FM_ERR | CHIP, "%s():[sample=%d]\n", __func__, sample);
        ret = -FM_EPARA;
        goto out;
    }

    if ((ret = mt6620_set_bits(0x5F, tmp_sample, 0xE7FF)))
        goto out;

    if ((ret = mt6620_write(0x9B, tmp_mode)))
        goto out;

    if ((ret = mt6620_set_bits(0x56, tmp_state, 0xFF7F)))
        goto out;

    WCN_DBG(FM_NTC | CHIP, "[onoff=%s][mode=%s][sample=%d](0)33KHz,(1)44.1KHz,(2)48KHz\n",
            (onoff == FM_I2S_ON) ? "On" : "Off",
            (mode == FM_I2S_MASTER) ? "Master" : "Slave",
            sample);
out:
    return ret;
}

static fm_bool mt6620_em_test(fm_u16 group_idx, fm_u16 item_idx, fm_u32 item_value)
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

    ops->bi.low_pwr_wa = fm_low_power_wa_default;
    ops->bi.pwron = mt6620_pwron;
    ops->bi.pwroff = mt6620_pwroff;
    ops->bi.msdelay = Delayms;
    ops->bi.usdelay = Delayus;
    ops->bi.read = mt6620_read;
    ops->bi.write = mt6620_write;
    ops->bi.setbits = mt6620_set_bits;
    ops->bi.chipid_get = mt6620_get_chipid;
    ops->bi.mute = mt6620_Mute;
    ops->bi.rampdown = mt6620_RampDown;
    ops->bi.pwrupseq = mt6620_PowerUp;
    ops->bi.pwrdownseq = mt6620_PowerDown;
    ops->bi.setfreq = mt6620_SetFreq;
    ops->bi.seek = mt6620_Seek;
    ops->bi.seekstop = mt6620_SeekStop;
    ops->bi.scan = mt6620_Scan;
    ops->bi.scanstop = mt6620_ScanStop;
    ops->bi.rssiget = mt6620_GetCurRSSI;
    ops->bi.volset = mt6620_SetVol;
    ops->bi.volget = mt6620_GetVol;
    ops->bi.dumpreg = mt6620_dump_reg;
    ops->bi.msget = mt6620_GetMonoStereo;
    ops->bi.pamdget = mt6620_GetCurPamd;
    ops->bi.em = mt6620_em_test;
    ops->bi.anaswitch = mt6620_SetAntennaType;
    ops->bi.anaget = mt6620_GetAntennaType;
    ops->bi.caparray_get = mt6620_GetCapArray;
    ops->bi.i2s_set = mt6620_I2s_Setting;

    return ret;
}

fm_s32 fm_low_ops_unregister(struct fm_lowlevel_ops *ops)
{
    fm_s32 ret = 0;
    //Basic functions.

    FMR_ASSERT(ops);

    fm_memset(&ops->bi, 0, sizeof(struct fm_basic_interface));
    return ret;
}

