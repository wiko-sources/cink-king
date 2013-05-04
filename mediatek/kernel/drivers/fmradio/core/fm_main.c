/* fm_main.c
 *
 * (C) Copyright 2011
 * MediaTek <www.MediaTek.com>
 * Hongcheng <hongcheng.xia@MediaTek.com>
 *
 * FM Radio Driver -- main functions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include "fm_main.h"
#include "fm_config.h"
#include "fm_cust_cfg.h"

//fm main data structure
static struct fm *g_fm_struct = NULL;
//we must get low level interface first, when add a new chip, the main effort is this interface
static struct fm_lowlevel_ops fm_low_ops;
//MTK FM Radio private advanced features
static struct fm_priv priv_adv;
//mutex for char device ops
static struct fm_lock *fm_ops_lock;
//mutex for RDS parsing and read result
static struct fm_lock *fm_read_lock;
//mutex for fm timer, RDS reset
static struct fm_lock *fm_timer_lock;
static struct fm_timer *fm_timer_sys;

static struct fm_i2s_info fm_i2s_inf = {
    .status = 0,    //i2s off
    .mode = 0,      //slave mode
    .rate = 48000,  //48000 sample rate
};

static fm_bool scan_stop_flag = fm_false;

//RDS reset related functions
static fm_u16 fm_cur_freq_get(void);
static fm_s32 fm_cur_freq_set(fm_u16 new_freq);
static enum fm_op_state fm_op_state_get(struct fm *fmp);
static enum fm_op_state fm_op_state_set(struct fm *fmp, enum fm_op_state sta);
static void fm_timer_func(unsigned long data);
static void fm_enable_rds_BlerCheck(struct fm *fm);
static void fm_disable_rds_BlerCheck(void);
static void fm_rds_reset_work_func(unsigned long data);
//when interrupt be triggered by FM chip, fm_eint_handler will first be executed
//then fm_eint_handler will schedule fm_eint_work_func to run
static void fm_eint_handler(void);
static void fm_eint_work_func(unsigned long data);
static fm_s32 fm_rds_parser(struct rds_rx_t *rds_raw, fm_s32 rds_size);
static fm_s32 fm_callback_register(struct fm_lowlevel_ops *ops);
static fm_s32 fm_callback_unregister(struct fm_lowlevel_ops *ops);

static fm_s32 pwrdown_flow(struct fm *fm);

static 	fm_u16 fm_cur_freq_get(void)
{
    return g_fm_struct ? g_fm_struct->cur_freq : 0;
}

static	fm_s32 fm_cur_freq_set(fm_u16 new_freq)
{
    if (g_fm_struct)
        g_fm_struct->cur_freq = new_freq;

    return 0;
}

static enum fm_op_state fm_op_state_get(struct fm *fmp)
{
    if (fmp) {
        WCN_DBG(FM_DBG | MAIN, "op state get %d\n", fmp->op_sta);
        return fmp->op_sta;
    } else {
        WCN_DBG(FM_ERR | MAIN, "op state get para error\n");
        return FM_STA_UNKOWN;
    }
}

static enum fm_op_state fm_op_state_set(struct fm *fmp, enum fm_op_state sta)
{
    if (fmp && (sta < FM_STA_MAX)) {
        fmp->op_sta = sta;
        WCN_DBG(FM_DBG | MAIN, "op state set to %d\n", sta);
        return fmp->op_sta;
    } else {
        WCN_DBG(FM_ERR | MAIN, "op state set para error, %d\n", sta);
        return FM_STA_UNKOWN;
    }
}

enum fm_pwr_state fm_pwr_state_get(struct fm *fmp)
{
    if (fmp) {
        WCN_DBG(FM_DBG | MAIN, "pwr state get %d\n", fmp->pwr_sta);
        return fmp->pwr_sta;
    } else {
        WCN_DBG(FM_ERR | MAIN, "pwr state get para error\n");
        return FM_PWR_MAX;
    }
}

enum fm_pwr_state fm_pwr_state_set(struct fm *fmp, enum fm_pwr_state sta)
{
    if (fmp && (sta < FM_PWR_MAX)) {
        fmp->pwr_sta = sta;
        WCN_DBG(FM_DBG | MAIN, "pwr state set to %d\n", sta);
        return fmp->pwr_sta;
    } else {
        WCN_DBG(FM_ERR | MAIN, "pwr state set para error, %d\n", sta);
        return FM_PWR_MAX;
    }
}

static volatile fm_s32 subsys_rst_state = FM_SUBSYS_RST_OFF;

fm_s32 fm_sys_state_get(struct fm *fmp)
{
    return subsys_rst_state;
}

fm_s32 fm_sys_state_set(struct fm *fmp, fm_s32 sta)
{
    if ((sta >= FM_SUBSYS_RST_OFF) && (sta < FM_SUBSYS_RST_MAX)) {
        WCN_DBG(FM_NTC | MAIN, "sys state set from %d to %d\n", subsys_rst_state, sta);
        subsys_rst_state = sta;
    } else {
        WCN_DBG(FM_ERR | MAIN, "sys state set para error, %d\n", sta);
    }

    return subsys_rst_state;
}


fm_s32 fm_subsys_reset(struct fm *fm)
{
    //check if we are resetting
    if (fm_sys_state_get(fm) != FM_SUBSYS_RST_OFF) {
        WCN_DBG(FM_NTC | MAIN, "subsys reset is ongoing\n");
        goto out;
    }

    FMR_ASSERT(fm);
    fm->timer_wkthd->add_work(fm->timer_wkthd, fm->rst_wk);

out:
    return 0;
}


fm_s32 fm_wholechip_rst_cb(fm_s32 sta)
{
    struct fm *fm = g_fm_struct;
    
    if (!fm) return 0;

    if (sta == 1) { 
        if (fm_sys_state_get(fm) == FM_SUBSYS_RST_OFF) {
            fm_sys_state_set(fm, FM_SUBSYS_RST_START);
        }
    } else {
        fm->timer_wkthd->add_work(fm->timer_wkthd, fm->rst_wk);
    }
    return 0;
}


fm_s32 fm_open(struct fm *fmp)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fmp);
    FM_LOCK(fm_ops_lock);

    //makesure fmp->ref >= 0
    fmp->ref = (fmp->ref < 0) ? 0 : fmp->ref;
    fmp->ref++;

    if ((fmp->ref > 0) && (fmp->chipon == fm_false)) {
        ret = fm_low_ops.bi.pwron(0);

        if (ret) {
            fmp->ref--;
            ret = -ENODEV;
            goto out;
        }

        fm_eint_pin_cfg(FM_EINT_PIN_EINT_MODE);
        fm_request_eint(fm_eint_handler);
        fmp->chipon = fm_true;
    }

out:
    WCN_DBG(FM_NTC | MAIN, "fm->ref:%d\n", fmp->ref);
    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_close(struct fm *fmp)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fmp);
    FM_LOCK(fm_ops_lock);

    fmp->ref--;

    if (fmp->ref <= 0) {
        pwrdown_flow(fmp);
        if (fmp->chipon == fm_true) {
            fm_eint_pin_cfg(FM_EINT_PIN_GPIO_MODE);
            fm_low_ops.bi.pwroff(0);
            fmp->chipon = fm_false;
        }
    }

    //makesure fm->ref >= 0
    fmp->ref = (fmp->ref < 0) ? 0 : fmp->ref;
    WCN_DBG(FM_NTC | MAIN, "fmp->ref:%d\n", fmp->ref);
    FM_UNLOCK(fm_ops_lock);

    return ret;
}

fm_s32 fm_rds_read(struct fm *fmp, fm_s8 *dst, fm_s32 len)
{
    fm_s32 copy_len = 0, left = 0;
    copy_len = sizeof(rds_t);

RESTART:

    if (FM_EVENT_GET(fmp->rds_event) == FM_RDS_DATA_READY) {
        FM_LOCK(fm_read_lock);

        if ((left = copy_to_user((void *)dst, fmp->pstRDSData, (unsigned long)copy_len))) {
            WCN_DBG(FM_ALT | MAIN, "fm_read copy failed\n");
        } else {
            fmp->pstRDSData->event_status = 0x0000;
        }

        WCN_DBG(FM_DBG | MAIN, "fm_read copy len:%d\n", (copy_len - left));

        FM_EVENT_RESET(fmp->rds_event);
        FM_UNLOCK(fm_read_lock);
    } else {
        if (FM_EVENT_WAIT(fmp->rds_event, FM_RDS_DATA_READY) == 0) {
            WCN_DBG(FM_DBG | MAIN, "fm_read wait ok\n");
            goto RESTART;
        } else {
            WCN_DBG(FM_ALT | MAIN, "fm_read wait err\n");
            return 0;
        }
    }

    return (copy_len - left);
}

fm_s32 fm_powerup(struct fm *fm, struct fm_tune_parm *parm)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.pwron);
    FMR_ASSERT(fm_low_ops.bi.pwrupseq);
    FMR_ASSERT(fm_low_ops.bi.low_pwr_wa);
    FM_LOCK(fm_ops_lock);

    //for normal case
    if (fm->chipon == fm_false) {
        fm_low_ops.bi.pwron(0);
        fm->chipon = fm_true;
    }

    if (FM_PWR_RX_ON == fm_pwr_state_get(fm)) {
        WCN_DBG(FM_NTC | MAIN, "already pwron!\n");
        goto out;
    }

    
    fm_pwr_state_set(fm, FM_PWR_RX_ON);

    //execute power on sequence
    ret = fm_low_ops.bi.pwrupseq(&fm->chip_id, &fm->device_id);

    if (ret) {
        goto out;
    }

    fm_enable_eint();

    WCN_DBG(FM_DBG | MAIN, "pwron ok\n");
    fm_cur_freq_set(parm->freq);

    parm->err = FM_SUCCESS;
    fm_low_ops.bi.low_pwr_wa(1);

    fm->vol = 15;

out:
    FM_UNLOCK(fm_ops_lock);
    return ret;
}

static fm_s32 pwrdown_flow(struct fm *fm)
{
    fm_s32 ret = 0;
    FMR_ASSERT(fm_low_ops.ri.rds_onoff);
    FMR_ASSERT(fm_low_ops.bi.pwrdownseq);
    FMR_ASSERT(fm_low_ops.bi.low_pwr_wa);
    if (FM_PWR_OFF == fm_pwr_state_get(fm)) {
        WCN_DBG(FM_NTC | MAIN, "already pwroff!\n");
        goto out;
    }
    
    //Disable all interrupt
    fm_disable_rds_BlerCheck();
    fm_low_ops.ri.rds_onoff(fm->pstRDSData, fm_false);
    fm_disable_eint();

    fm_pwr_state_set(fm, FM_PWR_OFF);

    //execute power down sequence
    ret = fm_low_ops.bi.pwrdownseq();

    fm_low_ops.bi.low_pwr_wa(0);
    WCN_DBG(FM_ALT | MAIN, "pwrdown ok\n");

out:
    return ret;
}

fm_s32 fm_powerdown(struct fm *fm)
{
    fm_s32 ret = 0;

    FM_LOCK(fm_ops_lock);

    ret = pwrdown_flow(fm);

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_seek(struct fm *fm, struct fm_seek_parm *parm)
{
    fm_s32 ret = 0;
    fm_u16 seekdir, space;

    FMR_ASSERT(fm_low_ops.bi.seek);
    FM_LOCK(fm_ops_lock);

    if (fm_pwr_state_get(fm) != FM_PWR_RX_ON) {
        parm->err = FM_BADSTATUS;
        ret = -EPERM;
        goto out;
    }

    if (parm->space == FM_SPACE_100K) {
        space = 0x0002;
    } else if (parm->space == FM_SPACE_50K) {
        space = 0x0001;
    } else if (parm->space == FM_SPACE_200K) {
        space = 0x0004;
    } else {
        //default
        space = 0x0002;
    }

    if (parm->band == FM_BAND_UE) {
        fm->min_freq = FM_UE_FREQ_MIN;
        fm->max_freq = FM_UE_FREQ_MAX;
    } else if (parm->band == FM_BAND_JAPANW) {
        fm->min_freq = FM_JP_FREQ_MIN;
        fm->max_freq = FM_JP_FREQ_MAX;
    } else if (parm->band == FM_BAND_SPECIAL) {
        fm->min_freq = FM_RX_BAND_FREQ_L;
        fm->max_freq = FM_RX_BAND_FREQ_H;
    } else {
        WCN_DBG(FM_ALT | MAIN, "band:%d out of range\n", parm->band);
        parm->err = FM_EPARM;
        ret = -EPERM;
        goto out;
    }

    if (parm->freq < fm->min_freq || parm->freq > fm->max_freq) {
        WCN_DBG(FM_ALT | MAIN, "freq:%d out of range\n", parm->freq);
        parm->err = FM_EPARM;
        ret = -EPERM;
        goto out;
    }

    if (parm->seekdir == FM_SEEK_UP) {
        seekdir = FM_SEEK_UP;
    } else {
        seekdir = FM_SEEK_DOWN;
    }

    fm_op_state_set(fm, FM_STA_SEEK);

    // seek successfully
    if (fm_true == fm_low_ops.bi.seek(fm->min_freq, fm->max_freq, &(parm->freq), seekdir, space)) {
        parm->err = FM_SUCCESS;
    } else {
        parm->err = FM_SEEK_FAILED;
        ret = -EPERM;
    }

    if (parm->space != FM_SPACE_50K) {
        parm->freq /= 10; //(8750 / 10) = 875
    }

    fm_op_state_set(fm, FM_STA_PLAY);
out:
    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32  fm_scan(struct fm *fm, struct fm_scan_parm *parm)
{
    fm_s32 ret = 0;
    fm_u16 scandir = FM_SEEK_UP, space;

    FMR_ASSERT(fm_low_ops.bi.scan);
    FM_LOCK(fm_ops_lock);

    if (fm_pwr_state_get(fm) != FM_PWR_RX_ON) {
        parm->err = FM_BADSTATUS;
        ret = -EPERM;
        goto out;
    }

    if (parm->space == FM_SPACE_100K) {
        space = 0x0002;
    } else if (parm->space == FM_SPACE_50K) {
        space = 0x0001;
    } else if (parm->space == FM_SPACE_200K) {
        space = 0x0004;
    } else {
        //default
        space = 0x0002;
    }

    if (parm->band == FM_BAND_UE) {
        fm->min_freq = FM_UE_FREQ_MIN;
        fm->max_freq = FM_UE_FREQ_MAX;
    } else if (parm->band == FM_BAND_JAPANW) {
        fm->min_freq = FM_JP_FREQ_MIN;
        fm->max_freq = FM_JP_FREQ_MAX;
    } else if (parm->band == FM_BAND_SPECIAL) {
        fm->min_freq = FM_RX_BAND_FREQ_L;
        fm->max_freq = FM_RX_BAND_FREQ_H;
    } else {
        WCN_DBG(FM_ALT | MAIN, "band:%d out of range\n", parm->band);
        parm->err = FM_EPARM;
        ret = -EPERM;
        goto out;
    }

    fm_op_state_set(fm, FM_STA_SCAN);
    scan_stop_flag = fm_false;

    if (fm_true == fm_low_ops.bi.scan(fm->min_freq, fm->max_freq, &(parm->freq), parm->ScanTBL, &(parm->ScanTBLSize), scandir, space)) {
        parm->err = FM_SUCCESS;
    } else {
        WCN_DBG(FM_ALT | MAIN, "fm_scan failed\n");
        parm->err = FM_SEEK_FAILED;
        ret = -EPERM;
    }

    fm_op_state_set(fm, FM_STA_STOP);

out:
    FM_UNLOCK(fm_ops_lock);
    return ret;
}


#define SCAN_SEG_LEN 250
static struct fm_cqi cqi_buf[SCAN_SEG_LEN];

fm_s32 fm_scan_new(struct fm *fm, struct fm_scan_t *parm)
{
    fm_s32 ret = 0;
    fm_s32 tmp;
    fm_s32 cnt, seg;
    fm_s32 i, j;
    fm_s32 start_freq, end_freq;
    fm_u16 scan_tbl[FM_SCANTBL_SIZE]; //need no less than the chip
    fm_u16 tbl_size = FM_SCANTBL_SIZE;
    fm_u16 tmp_freq = 0;
    fm_s32 chl_cnt;
    fm_s32 ch_offset, step, tmp_val;
    fm_u16 space_idx = 0x0002;
    
    fm_s32 cqi_cnt, cqi_idx;
    fm_s8 *buf = (fm_s8*)cqi_buf;
    
    FMR_ASSERT(fm_low_ops.bi.scan);
    FMR_ASSERT(fm_low_ops.bi.cqi_get);
    FM_LOCK(fm_ops_lock);

    // caculate channel number, get segment count
    cnt = (parm->upper - parm->lower) / parm->space + 1; //Eg, (10800 - 8750) / 5 = 411
    seg = (cnt / SCAN_SEG_LEN) + ((cnt % SCAN_SEG_LEN) ? 1 : 0); //Eg, (411 / 200) + ((411 % 200) ? 1 : 0) = 2 + 1 = 3

    WCN_DBG(FM_NTC | MAIN, "total ch %d, seg %d\n", cnt, seg);

    // alloc memory
    tmp = cnt * sizeof(struct fm_ch_rssi*);
    if (parm->sr_size < tmp) {
        if (parm->sr.ch_rssi_buf) {
            fm_free(parm->sr.ch_rssi_buf);
            parm->sr.ch_rssi_buf = NULL;
        }
        parm->sr_size = tmp;                
    }
    
    if (!parm->sr.ch_rssi_buf) {
        parm->sr.ch_rssi_buf = (struct fm_ch_rssi*)fm_zalloc(parm->sr_size);
        if (!parm->sr.ch_rssi_buf) {
            WCN_DBG(FM_ERR | MAIN, "scan alloc mem failed\n");
            parm->sr_size = 0;
            return -2;
        }
    }
    
    if (parm->space == 5) {
        space_idx = 0x0001; // 50Khz
    } else if (parm->space == 10) {
        space_idx = 0x0002; // 100Khz
    } else if (parm->space == 20) {
        space_idx = 0x0004; // 200Khz
    } 

    
    fm_op_state_set(fm, FM_STA_SCAN);
    
    // do scan
    chl_cnt = 0;
    for (i = 0; (i < seg) && (fm_false == scan_stop_flag); i++) {
        cqi_cnt = 0;
        cqi_idx = 0;
        
        start_freq = parm->lower + SCAN_SEG_LEN * parm->space * i;
        end_freq = parm->lower + SCAN_SEG_LEN * parm->space * (i + 1) - parm->space;
        end_freq = (end_freq > parm->upper) ? parm->upper : end_freq;

        WCN_DBG(FM_NTC | MAIN, "seg %d, start %d, end %d\n", i, start_freq, end_freq);
        if(fm_false == fm_low_ops.bi.scan(start_freq, end_freq, &tmp_freq, scan_tbl, &tbl_size, FM_SEEK_UP, space_idx)) {
            ret = -1;
            goto out;
        }

        // get channel count
        for (ch_offset = 0; ch_offset < FM_SCANTBL_SIZE; ch_offset++) {
		    if (scan_tbl[ch_offset] == 0)
			    continue;
		    for (step = 0; step < 16; step++) {
			    if (scan_tbl[ch_offset] & (1 << step)) {
                    tmp_val =  start_freq + (ch_offset * 16 + step) * parm->space;
                    if (tmp_val <= end_freq) {
                        // record valid  result channel
                        WCN_DBG(FM_NTC | MAIN, "freq %d\n", tmp_val);
                        parm->sr.ch_rssi_buf[chl_cnt].freq = tmp_val;
                        chl_cnt++; 
                        cqi_cnt++;
                    }
			    }
		    }
	    }  

        // get cqi
        tmp = cqi_cnt;
        while ((cqi_cnt > 0) && (fm_false == scan_stop_flag)) {
            ret = fm_low_ops.bi.cqi_get(buf + (16 * sizeof(struct fm_cqi) * cqi_idx), 
                sizeof(cqi_buf) - (16 * sizeof(struct fm_cqi) * cqi_idx));
            if (ret) {
                goto out;
            }

            cqi_cnt -= 16;
            cqi_idx++;
        }
        cqi_cnt = tmp;

        // fill cqi to result buffer
        for (j = 0; j < cqi_cnt; j++) {
            tmp = chl_cnt - cqi_cnt + j; // target pos
            parm->sr.ch_rssi_buf[tmp].freq = (fm_u16)cqi_buf[j].ch;
            parm->sr.ch_rssi_buf[tmp].rssi= cqi_buf[j].rssi;
            WCN_DBG(FM_NTC | MAIN, "idx %d, freq %d, rssi %d \n", tmp, parm->sr.ch_rssi_buf[tmp].freq, parm->sr.ch_rssi_buf[tmp].rssi);
        }
    }
     
    fm_op_state_set(fm, FM_STA_STOP);
    
out:
    scan_stop_flag = fm_false;
    FM_UNLOCK(fm_ops_lock);
    parm->num = chl_cnt;
    return ret;
}


fm_s32 fm_seek_new(struct fm *fm, struct fm_seek_t *parm)
{
    fm_s32 ret = 0;
    fm_s32 space_idx = 0x0002;

    FMR_ASSERT(fm_low_ops.bi.setfreq);
    FMR_ASSERT(fm_low_ops.bi.rssiget);
    FMR_ASSERT(fm_low_ops.bi.rampdown);

    FM_LOCK(fm_ops_lock);
     
    if (parm->freq < parm->lower || parm->freq > parm->upper) {
        WCN_DBG(FM_ERR | MAIN, "seek start freq:%d out of range\n", parm->freq);
        ret = -EPERM;
        goto out;
    }

    // tune to start freq
    fm_low_ops.bi.rampdown();
    fm_low_ops.bi.setfreq(parm->freq);

    if (parm->space == 5) {
        space_idx = 0x0001;
    } else if (parm->space == 10) {
        space_idx = 0x0002;
    } else if (parm->space == 20) {
        space_idx = 0x0004;
    }
    
    if (fm_false == fm_low_ops.bi.seek(parm->lower, parm->upper, &(parm->freq), parm->dir, space_idx)) {
        ret = -1;
        goto out;
    } 

    // tune to new channel
    fm_low_ops.bi.setfreq(parm->freq);
    fm_low_ops.bi.rssiget(&parm->th);
    
out:
    FM_UNLOCK(fm_ops_lock);
    return ret;    
}


fm_s32 fm_tune_new(struct fm *fm, struct fm_tune_t *parm)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.mute);
    FMR_ASSERT(fm_low_ops.bi.rampdown);
    FMR_ASSERT(fm_low_ops.bi.setfreq);

    FM_LOCK(fm_ops_lock);

    WCN_DBG(FM_DBG | MAIN, "%s\n", __func__);

    if (fm_pwr_state_get(fm) != FM_PWR_RX_ON) {
        ret = -EPERM;
        goto out;
    }

    if (parm->freq < parm->lower || parm->freq > parm->upper) {
        WCN_DBG(FM_ERR | MAIN, "tune freq:%d out of range\n", parm->freq);
        ret = -EPERM;
        goto out;
    }
    
    fm_low_ops.bi.mute(fm_true);
    fm_low_ops.bi.rampdown();

    if (fm_cur_freq_get() != parm->freq) {
        fm_memset(fm->pstRDSData, 0, sizeof(rds_t));
    }

    //HILO side adjust if need
    if (priv_adv.priv_tbl.hl_dese) {
        if ((ret = priv_adv.priv_tbl.hl_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "HILO side %d\n", ret);
    }

    //Frequency avoid adjust if need
    if (priv_adv.priv_tbl.fa_dese) {
        if ((ret = priv_adv.priv_tbl.fa_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "FA %d\n", ret);
    }

    //MCU clock adjust if need
    if (priv_adv.priv_tbl.mcu_dese) {
        if ((ret = priv_adv.priv_tbl.mcu_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "MCU %d\n", ret);
    }

    //GPS clock adjust if need
    if (priv_adv.priv_tbl.gps_dese) {
        if ((ret = priv_adv.priv_tbl.gps_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "GPS %d\n", ret);
    }

    fm_op_state_set(fm, FM_STA_TUNE);
    WCN_DBG(FM_ALT | MAIN, "tuning to %d\n", parm->freq);

    if (fm_false == fm_low_ops.bi.setfreq(parm->freq)) {
        WCN_DBG(FM_ALT | MAIN, "FM tune failed\n");
        ret = -EPERM;
        goto out;
    }

    fm_low_ops.bi.mute(fm_false);
    fm_op_state_set(fm, FM_STA_PLAY);
out:
    FM_UNLOCK(fm_ops_lock);
    return ret;
}


fm_s32  fm_cqi_get(struct fm *fm, fm_s32 ch_num, fm_s8 *buf, fm_s32 buf_size)
{
    fm_s32 ret = 0;
    fm_s32 idx = 0;

    FMR_ASSERT(fm_low_ops.bi.cqi_get);
    FM_LOCK(fm_ops_lock);

    if (fm_true == scan_stop_flag) {
        WCN_DBG(FM_NTC | MAIN, "scan flow aborted, do not get CQI\n");
        ret = -1;
        goto out;
    }
    
    if (fm_pwr_state_get(fm) != FM_PWR_RX_ON) {
        ret = -EPERM;
        goto out;
    }

    if (ch_num*sizeof(struct fm_cqi) > buf_size) {
        ret = -EPERM;
        goto out;
    }

    fm_op_state_set(fm, FM_STA_SCAN);

    idx = 0;
    WCN_DBG(FM_NTC | MAIN, "cqi num %d\n", ch_num);

    while (ch_num > 0) {
        ret = fm_low_ops.bi.cqi_get(buf + 16 * sizeof(struct fm_cqi) * idx, buf_size - 16 * sizeof(struct fm_cqi) * idx);

        if (ret) {
            goto out;
        }

        ch_num -= 16;
        idx++;
    }

    fm_op_state_set(fm, FM_STA_STOP);

out:
    FM_UNLOCK(fm_ops_lock);
    return ret;
}


/*  fm_is_dese_chan -- check if gived channel is a de-sense channel or not
  *  @pfm - fm driver global DS
  *  @freq - gived channel
  *  return value: 0, not a dese chan; 1, a dese chan; else error NO.
  */
fm_s32 fm_is_dese_chan(struct fm *pfm, fm_u16 freq)
{
    if ((priv_adv.state == INITED) && (priv_adv.priv_tbl.is_dese_chan)) {
        return priv_adv.priv_tbl.is_dese_chan(freq);
    }

    return 0;
}


/*  fm_get_hw_info -- hw info: chip id, ECO version, DSP ROM version, Patch version
  *  @pfm - fm driver global DS
  *  @freq - target buffer
  *  return value: 0, success; else error NO.
  */
fm_s32 fm_get_hw_info(struct fm *pfm, struct fm_hw_info *req)
{
    fm_s32 ret = 0;

    FMR_ASSERT(req);

    //default value for all chips
    req->chip_id = 0x000066FF;
    req->eco_ver = 0x00000000;
    req->rom_ver = 0x00000001;
    req->patch_ver = 0x00000100;
    req->reserve = 0x00000000;

    //get actual chip hw info
    if (fm_low_ops.bi.hwinfo_get) {
        FM_LOCK(fm_ops_lock);
        ret = fm_low_ops.bi.hwinfo_get(req);
        FM_UNLOCK(fm_ops_lock);
    }

    return ret;
}


/*  fm_get_i2s_info -- i2s info: on/off, master/slave, sample rate
  *  @pfm - fm driver global DS
  *  @freq - target buffer
  *  return value: 0, success; else error NO.
  */
fm_s32 fm_get_i2s_info(struct fm *pfm, struct fm_i2s_info *req)
{
    FMR_ASSERT(req);

    if (fm_low_ops.bi.i2s_get) {
        return fm_low_ops.bi.i2s_get(&req->status, &req->mode, &req->rate);
    } else {
        req->status = fm_i2s_inf.status;
        req->mode = fm_i2s_inf.mode;
        req->rate = fm_i2s_inf.rate;

        return 0;
    }
}


fm_s32  fm_hwscan_stop(struct fm *fm)
{
    fm_s32 ret = 0;

    if (FM_STA_SCAN != fm_op_state_get(fm)) {
        WCN_DBG(FM_WAR | MAIN, "fm isn't on scan, no need stop\n");
        return ret;
    }

    FMR_ASSERT(fm_low_ops.bi.scanstop);

    fm_low_ops.bi.scanstop();
    scan_stop_flag = fm_true;
    WCN_DBG(FM_DBG | MAIN, "fm will stop scan\n");

    FM_LOCK(fm_ops_lock);
    
    fm_low_ops.bi.rampdown();
    fm_low_ops.bi.setfreq(fm_cur_freq_get());

    FM_UNLOCK(fm_ops_lock);
    
    return ret;
}

/* fm_ana_switch -- switch antenna to long/short
 * @fm - fm driver main data structure
 * @antenna - 0, long; 1, short
 * If success, return 0; else error code
 */
fm_s32 fm_ana_switch(struct fm *fm, fm_s32 antenna)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.anaswitch);
    FM_LOCK(fm_ops_lock);

    WCN_DBG(FM_DBG | MAIN, "Switching ana to %s\n", antenna ? "short" : "long");
    fm->ana_type = antenna;
    ret = fm_low_ops.bi.anaswitch(antenna);

    if (ret) {
        WCN_DBG(FM_ALT | MAIN, "Switch ana Failed\n");
    } else {
        WCN_DBG(FM_DBG | MAIN, "Switch ana OK!\n");
    }

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

//volume?[0~15]
fm_s32 fm_setvol(struct fm *fm, fm_u32 vol)
{
    fm_u8 tmp_vol;

    FMR_ASSERT(fm_low_ops.bi.volset);
    FM_LOCK(fm_ops_lock);

    tmp_vol = (vol > 15) ? 15 : vol;
    fm_low_ops.bi.volset(tmp_vol);
    fm->vol = (fm_s32)tmp_vol;

    FM_UNLOCK(fm_ops_lock);
    return 0;
}

fm_s32 fm_getvol(struct fm *fm, fm_u32 *vol)
{
    fm_u8 tmp_vol;

    FMR_ASSERT(fm_low_ops.bi.volget);
    FM_LOCK(fm_ops_lock);

    fm_low_ops.bi.volget(&tmp_vol);
    *vol = (fm_u32)tmp_vol;

    FM_UNLOCK(fm_ops_lock);
    return 0;
}

fm_s32 fm_mute(struct fm *fm, fm_u32 bmute)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.mute);
    FM_LOCK(fm_ops_lock);

    if (bmute) {
        ret = fm_low_ops.bi.mute(fm_true);
        fm->mute = fm_true;
    } else {
        ret = fm_low_ops.bi.mute(fm_false);
        fm->mute = fm_false;
    }

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_getrssi(struct fm *fm, fm_s32 *rssi)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.rssiget);
    FM_LOCK(fm_ops_lock);

    ret = fm_low_ops.bi.rssiget(rssi);

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_reg_read(struct fm *fm, fm_u8 addr, fm_u16 *val)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.read);
    FM_LOCK(fm_ops_lock);

    ret = fm_low_ops.bi.read(addr, val);

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_reg_write(struct fm *fm, fm_u8 addr, fm_u16 val)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.write);
    FM_LOCK(fm_ops_lock);

    ret = fm_low_ops.bi.write(addr, val);

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_chipid_get(struct fm *fm, fm_u16 *chipid)
{
    FMR_ASSERT(chipid);
    FM_LOCK(fm_ops_lock);

    *chipid = fm->chip_id;

    FM_UNLOCK(fm_ops_lock);
    return 0;
}

fm_s32 fm_monostereo_get(struct fm *fm, fm_u16 *ms)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.msget);
    FMR_ASSERT(ms);
    FM_LOCK(fm_ops_lock);

    if (fm_low_ops.bi.msget(ms) == fm_false) {
        ret = -FM_EPARA;
    }

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

/*
 * Force set to stero/mono mode
 * @MonoStereo -- 0, auto; 1, mono
 * If success, return 0; else error code
 */
fm_s32 fm_monostereo_set(struct fm *fm, fm_s32 ms)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.msset);
    FM_LOCK(fm_ops_lock);

    ret = fm_low_ops.bi.msset(ms);

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_pamd_get(struct fm *fm, fm_u16 *pamd)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.pamdget);
    FMR_ASSERT(pamd);
    FM_LOCK(fm_ops_lock);

    if (fm_low_ops.bi.pamdget(pamd) == fm_false) {
        ret = -FM_EPARA;
    }

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_caparray_get(struct fm *fm, fm_s32 *ca)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.caparray_get);
    FMR_ASSERT(ca);
    FM_LOCK(fm_ops_lock);

    ret = fm_low_ops.bi.caparray_get(ca);

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_em_test(struct fm *fm, fm_u16 group, fm_u16 item, fm_u32 val)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.em);
    FM_LOCK(fm_ops_lock);

    if (fm_false == fm_low_ops.bi.em(group, item, val)) {
        ret = -FM_EPARA;
    }

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_rds_onoff(struct fm *fm, fm_u16 rdson_off)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_onoff);
    FM_LOCK(fm_ops_lock);

    if (rdson_off) {
        fm->rds_on = fm_true;
        if (fm_low_ops.ri.rds_onoff(fm->pstRDSData, fm_true) == fm_false) {
            WCN_DBG(FM_ALT | MAIN, "FM_IOCTL_RDS_ONOFF faield\n");
            ret = -EPERM;
            goto out;
        }

        fm_enable_rds_BlerCheck(fm);
    } else {
        fm->rds_on = fm_false;
        fm_disable_rds_BlerCheck();
        fm_low_ops.ri.rds_onoff(fm->pstRDSData, fm_false);
    }

out:
    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_rds_good_bc_get(struct fm *fm, fm_u16 *gbc)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_gbc_get);
    FMR_ASSERT(gbc);
    FM_LOCK(fm_ops_lock);

    *gbc = fm_low_ops.ri.rds_gbc_get();

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_rds_bad_bc_get(struct fm *fm, fm_u16 *bbc)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_bbc_get);
    FMR_ASSERT(bbc);
    FM_LOCK(fm_ops_lock);

    *bbc = fm_low_ops.ri.rds_bbc_get();

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_rds_bler_ratio_get(struct fm *fm, fm_u16 *bbr)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_bbr_get);
    FMR_ASSERT(bbr);
    FM_LOCK(fm_ops_lock);

    *bbr = (fm_u16)fm_low_ops.ri.rds_bbr_get();

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_rds_group_cnt_get(struct fm *fm, struct rds_group_cnt_t *dst)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_gc_get);
    FMR_ASSERT(dst);
    FM_LOCK(fm_read_lock);

    ret = fm_low_ops.ri.rds_gc_get(dst, fm->pstRDSData);

    FM_UNLOCK(fm_read_lock);
    return ret;
}

fm_s32 fm_rds_group_cnt_reset(struct fm *fm)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_gc_reset);
    FM_LOCK(fm_read_lock);

    ret = fm_low_ops.ri.rds_gc_reset(fm->pstRDSData);

    FM_UNLOCK(fm_read_lock);
    return ret;
}

fm_s32 fm_rds_log_get(struct fm *fm, struct rds_rx_t *dst, fm_s32 *dst_len)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_log_get);
    FMR_ASSERT(dst);
    FMR_ASSERT(dst_len);
    FM_LOCK(fm_read_lock);

    ret = fm_low_ops.ri.rds_log_get(dst, dst_len);

    FM_UNLOCK(fm_read_lock);
    return ret;
}

fm_s32 fm_rds_block_cnt_reset(struct fm *fm)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.ri.rds_bc_reset);
    FM_LOCK(fm_ops_lock);

    ret = fm_low_ops.ri.rds_bc_reset();

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

fm_s32 fm_i2s_set(struct fm *fm, fm_s32 onoff, fm_s32 mode, fm_s32 sample)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.i2s_set);
    FM_LOCK(fm_ops_lock);

    ret = fm_low_ops.bi.i2s_set(onoff, mode, sample);

    FM_UNLOCK(fm_ops_lock);
    return ret;
}

/*
 *  fm_tune
 */
fm_s32 fm_tune(struct fm *fm, struct fm_tune_parm *parm)
{
    fm_s32 ret = 0;

    FMR_ASSERT(fm_low_ops.bi.mute);
    FMR_ASSERT(fm_low_ops.bi.rampdown);
    FMR_ASSERT(fm_low_ops.bi.setfreq);

    FM_LOCK(fm_ops_lock);

    WCN_DBG(FM_DBG | MAIN, "%s\n", __func__);

    if (fm_pwr_state_get(fm) != FM_PWR_RX_ON) {
        parm->err = FM_BADSTATUS;
        ret = -EPERM;
        goto out;
    }

    fm_low_ops.bi.mute(fm_true);
    fm_low_ops.bi.rampdown();

    if (fm_cur_freq_get() != parm->freq) {
        fm_memset(fm->pstRDSData, 0, sizeof(rds_t));
    }

    //HILO side adjust if need
    if (priv_adv.priv_tbl.hl_dese) {
        if ((ret = priv_adv.priv_tbl.hl_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "HILO side %d\n", ret);
    }

    //Frequency avoid adjust if need
    if (priv_adv.priv_tbl.fa_dese) {
        if ((ret = priv_adv.priv_tbl.fa_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "FA %d\n", ret);
    }

    //MCU clock adjust if need
    if (priv_adv.priv_tbl.mcu_dese) {
        if ((ret = priv_adv.priv_tbl.mcu_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "MCU %d\n", ret);
    }

    //GPS clock adjust if need
    if (priv_adv.priv_tbl.gps_dese) {
        if ((ret = priv_adv.priv_tbl.gps_dese(parm->freq, NULL)) < 0) {
            goto out;
        }

        WCN_DBG(FM_INF | MAIN, "GPS %d\n", ret);
    }

    fm_op_state_set(fm, FM_STA_TUNE);
    WCN_DBG(FM_ALT | MAIN, "tuning to %d\n", parm->freq);

    if (fm_false == fm_low_ops.bi.setfreq(parm->freq)) {
        parm->err = FM_TUNE_FAILED;
        WCN_DBG(FM_ALT | MAIN, "FM tune failed\n");
        ret = -EPERM;
    }

    fm_low_ops.bi.mute(fm_false);
    fm_op_state_set(fm, FM_STA_PLAY);
out:
    FM_UNLOCK(fm_ops_lock);
    return ret;
}

static void fm_timer_func(unsigned long data)
{
    struct fm *fm = g_fm_struct;

    FM_LOCK(fm_timer_lock);

    if (fm_timer_sys->update(fm_timer_sys)) {
        goto out; //fm timer is stoped before timeout
    }

    if (fm != NULL) {
        fm->timer_wkthd->add_work(fm->timer_wkthd, fm->rds_wk);
    }

out:
    FM_UNLOCK(fm_timer_lock);
}


static void fm_enable_rds_BlerCheck(struct fm *fm)
{
    FM_LOCK(fm_timer_lock);
    fm_timer_sys->start(fm_timer_sys);
    FM_UNLOCK(fm_timer_lock);
}

static void fm_disable_rds_BlerCheck(void)
{
    FM_LOCK(fm_timer_lock);
    fm_timer_sys->stop(fm_timer_sys);
    FM_UNLOCK(fm_timer_lock);
}

void fm_rds_reset_work_func(unsigned long data)
{
    fm_s32 ret = 0;

    if (!fm_low_ops.ri.rds_blercheck) {
        return;
    }

    FM_LOCK(fm_read_lock);
    ret = fm_low_ops.ri.rds_blercheck(g_fm_struct->pstRDSData);

    if (!ret && g_fm_struct->pstRDSData->event_status) {
        FM_EVENT_SEND(g_fm_struct->rds_event, FM_RDS_DATA_READY);
    }

    FM_UNLOCK(fm_read_lock);
}


void fm_subsys_reset_work_func(unsigned long data)
{
    g_dbg_level = 0xffffffff;
    FM_LOCK(fm_ops_lock);

    fm_sys_state_set(g_fm_struct, FM_SUBSYS_RST_START);
    
    // subsystem power off
    fm_low_ops.bi.pwroff(0);
    
    // prepare to reset
    
    // wait 3s
    fm_low_ops.bi.msdelay(2000);
    
    // subsystem power on
    fm_low_ops.bi.pwron(0);
    
    // recover context
    if (g_fm_struct->chipon == fm_false) {
        fm_low_ops.bi.pwroff(0);
        WCN_DBG(FM_ALT | MAIN, "no need do recover\n");
        goto out;
    }

    if (FM_PWR_RX_ON == fm_pwr_state_get(g_fm_struct)) {
        fm_low_ops.bi.pwrupseq(&g_fm_struct->chip_id, &g_fm_struct->device_id);
    } else {
        WCN_DBG(FM_ALT | MAIN, "no need do re-powerup\n");
        goto out;
    }

    fm_low_ops.bi.anaswitch(g_fm_struct->ana_type);
    
    fm_low_ops.bi.setfreq(fm_cur_freq_get()); 

    fm_low_ops.bi.volset((fm_u8)g_fm_struct->vol);

    fm_low_ops.bi.mute(g_fm_struct->mute);

    fm_low_ops.ri.rds_onoff(g_fm_struct->pstRDSData, g_fm_struct->rds_on);

    WCN_DBG(FM_ALT | MAIN, "recover done\n");

out:
    fm_sys_state_set(g_fm_struct, FM_SUBSYS_RST_END);
    fm_sys_state_set(g_fm_struct, FM_SUBSYS_RST_OFF);

    FM_UNLOCK(fm_ops_lock);
    g_dbg_level = 0xfffffff5;
}


static void fm_eint_handler(void)
{
    struct fm *fm = g_fm_struct;
    WCN_DBG(FM_DBG | MAIN, "intr occur, ticks:%d\n", jiffies_to_msecs(jiffies));

    if (fm != NULL) {
        fm->eint_wkthd->add_work(fm->eint_wkthd, fm->eint_wk);
    }
}

static fm_s32 fm_rds_parser(struct rds_rx_t *rds_raw, fm_s32 rds_size)
{
    struct fm *fm = g_fm_struct;//(struct fm *)work->data;
    rds_t *pstRDSData = fm->pstRDSData;

    FM_LOCK(fm_read_lock);
    //parsing RDS data
    fm_low_ops.ri.rds_parser(pstRDSData, rds_raw, rds_size, fm_cur_freq_get);
    FM_UNLOCK(fm_read_lock);

    if ((pstRDSData->event_status != 0x0000) && (pstRDSData->event_status != RDS_EVENT_AF_LIST)) {
        WCN_DBG(FM_DBG | MAIN, "Notify user to read, [event:%04x]\n", pstRDSData->event_status);
        FM_EVENT_SEND(fm->rds_event, FM_RDS_DATA_READY);
    }

    return 0;
}

static void fm_eint_work_func(unsigned long data)
{
    fm_event_parser(fm_rds_parser);
    //re-enable eint if need
    fm_enable_eint();
}

static fm_s32 fm_callback_register(struct fm_lowlevel_ops *ops)
{
    FMR_ASSERT(ops);

    ops->cb.cur_freq_get = fm_cur_freq_get;
    ops->cb.cur_freq_set = fm_cur_freq_set;
    return 0;
}

static fm_s32 fm_callback_unregister(struct fm_lowlevel_ops *ops)
{
    FMR_ASSERT(ops);

    fm_memset(&ops->cb, 0, sizeof(struct fm_callback));
    return 0;
}


static fm_s32 fm_para_init(struct fm *fmp)
{
    FMR_ASSERT(fmp);

    fmp->band = FM_BAND_SPECIAL;
    fmp->min_freq = FM_RX_BAND_FREQ_L;
    fmp->max_freq = FM_RX_BAND_FREQ_H;
    fmp->cur_freq = 0;
    
    return 0;
}


struct fm* fm_dev_init(fm_u32 arg) 
{
    fm_s32 ret = 0;
    struct fm *fm = NULL;

    if (!fm_low_ops.ri.rds_bci_get)
        return NULL;

    if (!fm_low_ops.bi.chipid_get)
        return NULL;

    //alloc fm main data structure
    if (!(fm = fm_zalloc(sizeof(struct fm)))) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    }

    fm->ref = 0;
    fm->chipon = fm_false;
    fm_pwr_state_set(fm, FM_PWR_OFF);
    fm->chip_id = fm_low_ops.bi.chipid_get();

    if (!(fm->rds_event = fm_flag_event_create("fm_rds_event"))) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM for RDS event\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    }

    fm_flag_event_get(fm->rds_event);

    //alloc fm rds data structure
    if (!(fm->pstRDSData = fm_zalloc(sizeof(rds_t)))) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM for RDS\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    }

    g_fm_struct = fm;

    fm->timer_wkthd = fm_workthread_create("fm_timer_wq");

    if (!fm->timer_wkthd) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM for fm_timer_wq\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    }

    fm_workthread_get(fm->timer_wkthd);

    fm->eint_wkthd = fm_workthread_create("fm_eint_wq");

    if (!fm->eint_wkthd) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM for fm_eint_wq\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    }

    fm_workthread_get(fm->eint_wkthd);

    fm->eint_wk = fm_work_create("fm_eint_work");

    if (!fm->eint_wk) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM for eint_wk\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    } else {
        fm_work_get(fm->eint_wk);
        fm->eint_wk->init(fm->eint_wk, fm_eint_work_func, (unsigned long)fm);
    }

    // create reset work
    fm->rst_wk = fm_work_create("fm_rst_work");

    if (!fm->rst_wk) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM for rst_wk\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    } else {
        fm_work_get(fm->rst_wk);
        fm->rst_wk->init(fm->rst_wk, fm_subsys_reset_work_func, (unsigned long)fm);
    }

    fm->rds_wk = fm_work_create("fm_rds_work");
    if (!fm->rds_wk) {
        WCN_DBG(FM_ALT | MAIN, "-ENOMEM for rds_wk\n");
        ret = -ENOMEM;
        goto ERR_EXIT;
    } else {
        fm_work_get(fm->rds_wk);
        fm->rds_wk->init(fm->rds_wk, fm_rds_reset_work_func, (unsigned long)fm);
    }

    //fm timer was created in fm_env_setp()
    fm_timer_sys->init(fm_timer_sys, fm_timer_func, (unsigned long)g_fm_struct, fm_low_ops.ri.rds_bci_get(), 0);
    fm_timer_sys->start(fm_timer_sys);

    //init customer config parameter
    fm_cust_config_setup(NULL);

    fm_para_init(fm);
    
    return g_fm_struct;

ERR_EXIT:

    if (fm->eint_wkthd) {
        ret = fm_workthread_put(fm->eint_wkthd);

        if (!ret)
            fm->eint_wkthd = NULL;
    }

    if (fm->timer_wkthd) {
        ret = fm_workthread_put(fm->timer_wkthd);

        if (!ret)
            fm->timer_wkthd = NULL;
    }

    if (fm->eint_wk) {
        ret = fm_work_put(fm->eint_wk);

        if (!ret)
            fm->eint_wk = NULL;
    }

    if (fm->rds_wk) {
        ret = fm_work_put(fm->rds_wk);

        if (!ret)
            fm->rds_wk = NULL;
    }

    if (fm->rst_wk) {
        ret = fm_work_put(fm->rst_wk);

        if (!ret)
            fm->rst_wk = NULL;
    }
    
    if (fm->pstRDSData) {
        fm_free(fm->pstRDSData);
        fm->pstRDSData = NULL;
    }

    fm_free(fm);
    g_fm_struct = NULL;
    return NULL;
}

fm_s32 fm_dev_destroy(struct fm *fm)
{
    fm_s32 ret = 0;

    WCN_DBG(FM_DBG | MAIN, "%s\n", __func__);

    fm_timer_sys->stop(fm_timer_sys);

    if (fm->eint_wkthd) {
        ret = fm_workthread_put(fm->eint_wkthd);

        if (!ret)
            fm->eint_wkthd = NULL;
    }

    if (fm->timer_wkthd) {
        ret = fm_workthread_put(fm->timer_wkthd);

        if (!ret)
            fm->timer_wkthd = NULL;
    }

    if (fm->eint_wk) {
        ret = fm_work_put(fm->eint_wk);

        if (!ret)
            fm->eint_wk = NULL;
    }

    if (fm->rds_wk) {
        ret = fm_work_put(fm->rds_wk);

        if (!ret)
            fm->rds_wk = NULL;
    }

    if (fm->rst_wk) {
        ret = fm_work_put(fm->rst_wk);

        if (!ret)
            fm->rst_wk = NULL;
    }
    
    if (fm->pstRDSData) {
        fm_free(fm->pstRDSData);
        fm->pstRDSData = NULL;
    }

    if (fm->pstRDSData) {
        fm_free(fm->pstRDSData);
        fm->pstRDSData = NULL;
    }

    fm_flag_event_put(fm->rds_event);

    // free all memory
    if (fm) {
        fm_free(fm);
        fm = NULL;
        g_fm_struct = NULL;
    }

    return ret;
}

fm_s32 fm_env_setup(void)
{
    fm_s32 ret = 0;

    WCN_DBG(FM_NTC | MAIN, "%s\n", __func__);
    //register call back functions
    ret = fm_callback_register(&fm_low_ops);

    if (ret) {
        return ret;
    }

    WCN_DBG(FM_NTC | MAIN, "1. fm callback registered\n");
    //get low level functions
    ret = fm_low_ops_register(&fm_low_ops);

    if (ret) {
        return ret;
    }

    WCN_DBG(FM_NTC | MAIN, "2. fm low ops registered\n");
    //get rds level functions
    ret = fm_rds_ops_register(&fm_low_ops);

    if (ret) {
        return ret;
    }

    WCN_DBG(FM_NTC | MAIN, "3. fm rds ops registered\n");

    fm_ops_lock = fm_lock_create("basic_ops");

    if (!fm_ops_lock) {
        return -1;
    }

    fm_read_lock = fm_lock_create("rds_data");

    if (!fm_read_lock) {
        return -1;
    }

    fm_timer_lock = fm_lock_create("timer");

    if (!fm_timer_lock) {
        return -1;
    }

    fm_lock_get(fm_ops_lock);
    fm_lock_get(fm_read_lock);
    fm_lock_get(fm_timer_lock);
    WCN_DBG(FM_NTC | MAIN, "4. fm locks created\n");

    fm_timer_sys = fm_timer_create("fm_sys_timer");

    if (!fm_timer_sys) {
        return -1;
    }

    fm_timer_get(fm_timer_sys);
    WCN_DBG(FM_NTC | MAIN, "5. fm timer created\n");

    ret = fm_link_setup((void*)fm_wholechip_rst_cb);

    if (ret) {
        WCN_DBG(FM_ERR | MAIN, "fm link setup Failed\n");
        return -1;
    }

    return ret;
}

fm_s32 fm_env_destroy(void)
{
    fm_s32 ret = 0;

    WCN_DBG(FM_NTC | MAIN, "%s\n", __func__);

    fm_link_release();

    //register call back functions
    ret = fm_callback_unregister(&fm_low_ops);

    if (ret) {
        return -1;
    }

    //put low level functions
    ret = fm_low_ops_unregister(&fm_low_ops);

    if (ret) {
        return -1;
    }

    //put rds func
    ret = fm_rds_ops_unregister(&fm_low_ops);

    if (ret) {
        return -1;
    }

    ret = fm_lock_put(fm_ops_lock);

    if (!ret)
        fm_ops_lock = NULL;

    ret = fm_lock_put(fm_read_lock);

    if (!ret)
        fm_read_lock = NULL;

    ret = fm_lock_put(fm_timer_lock);

    if (!ret)
        fm_timer_lock = NULL;

    ret = fm_timer_put(fm_timer_sys);

    if (!ret)
        fm_timer_sys = NULL;

    return ret;
}


fm_s32 fm_priv_register(struct fm_priv *pri, struct fm_pub *pub)
{
    fm_s32 ret = 0;
    //Basic functions.

    WCN_DBG(FM_NTC | MAIN, "%s\n", __func__);
    FMR_ASSERT(pri);
    FMR_ASSERT(pub);

    // functions provided by private module
    priv_adv.priv_tbl.hl_dese = pri->priv_tbl.hl_dese;
    priv_adv.priv_tbl.fa_dese = pri->priv_tbl.fa_dese;
    priv_adv.priv_tbl.mcu_dese = pri->priv_tbl.mcu_dese;
    priv_adv.priv_tbl.gps_dese = pri->priv_tbl.gps_dese;
    priv_adv.priv_tbl.chan_para_get = pri->priv_tbl.chan_para_get;
    priv_adv.priv_tbl.is_dese_chan = pri->priv_tbl.is_dese_chan;
    priv_adv.state = INITED;
    priv_adv.data = NULL;

    // for special chip(chip with DSP) use
    fm_low_ops.cb.chan_para_get = priv_adv.priv_tbl.chan_para_get;

    // private module will use these functions
    pub->pub_tbl.read = fm_low_ops.bi.read;
    pub->pub_tbl.write = fm_low_ops.bi.write;
    pub->pub_tbl.setbits = fm_low_ops.bi.setbits;
    pub->pub_tbl.rampdown = fm_low_ops.bi.rampdown;
    pub->pub_tbl.msdelay = fm_low_ops.bi.msdelay;
    pub->pub_tbl.usdelay = fm_low_ops.bi.usdelay;
    pub->pub_tbl.log = (fm_s32 (*)(const fm_s8 *arg1, ...))printk;
    pub->state = INITED;
    pub->data = NULL;

    return ret;
}

fm_s32 fm_priv_unregister(struct fm_priv *pri, struct fm_pub *pub)
{
    WCN_DBG(FM_NTC | MAIN, "%s\n", __func__);

    //FMR_ASSERT(pri);
    FMR_ASSERT(pub);

    fm_memset(&priv_adv, 0, sizeof(struct fm_priv));
    fm_low_ops.cb.chan_para_get = NULL;
    fm_memset(pub, 0, sizeof(struct fm_pub));

    return 0;
}

