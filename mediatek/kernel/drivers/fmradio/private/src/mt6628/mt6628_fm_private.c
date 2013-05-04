#include <linux/module.h>
#include <linux/init.h>

#include "stp_exp.h"
#include "wmt_exp.h"
#include "fm_private.h"
#include "mt6628_fm_private.h"
#include "fm_priv_log.h"

static struct fm_pub pub;
static struct fm_pub_cb *pub_cb = &pub.pub_tbl;

static const fm_u16 mt6628_mcu_dese_list[] = {
        7630, 7800, 7940, 8320, 9260, 9600, 9710, 9920, 10400, 10410
};

static const fm_u16 mt6628_gps_dese_list[] = {
        7850, 7860
};

static const fm_s8 mt6628_chan_para_map[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,  //7600~7695
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //7700~7795
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //7800~7895
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //7900~7995
        8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8000~8095
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8100~8195
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8200~8295
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8300~8395
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,  //8400~8495
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8500~8595
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8600~8695
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8700~8795
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8800~8895
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //8900~8995
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //9000~9095
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //9100~9195
        0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //9200~9295
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //9300~9395
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0,  //9400~9495
        1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0,  //9500~9595
        8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //9600~9695
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //9700~9795
        0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //9800~9895
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,  //9900~9995
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //10000~10095
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //10100~10195
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,  //10200~10295
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //10300~10395
        8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //10400~10495
        1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //10500~10595
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //10600~10695
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,  //10700~10795
        0   //10800
};


static const fm_u16 mt6628_scan_dese_list[] = {
        7680, 8210, 8450, 9210, 9220, 9600, 9840, 10400, 10750, 10760
};


static fm_s32 mt6628_is_dese_chan(fm_u16 freq);
static fm_s32 mt6628_hl_dese(fm_u16 freq, void *arg);
static fm_s32 mt6628_fa_dese(fm_u16 freq, void *arg);
static fm_s32 mt6628_mcu_dese(fm_u16 freq, void *arg);
static fm_s32 mt6628_gps_dese(fm_u16 freq, void *arg);
static fm_u16 mt6628_chan_para_get(fm_u16 freq);



/*
 * mt6628_get_channel_space - get the spcace of gived channel
 * @freq - value in 760~1080 or 7600~10800
 *
 * Return 0, if 760~1080; return 1, if 7600 ~ 10800, else err code < 0
 */
static fm_s32 mt6628_get_channel_space(int freq)
{
    if ((freq >= 760) && (freq <= 1080)) {
        return 0;
    } else if ((freq >= 7600) && (freq <= 10800)) {
        return 1;
    } else {
        return -1;
    }
}


// return value: 0, not a de-sense channel; 1, this is a de-sense channel; else error no
static fm_s32 mt6628_is_dese_chan(fm_u16 freq)
{
    fm_s32 size;

    size = sizeof(mt6628_scan_dese_list) / sizeof(mt6628_scan_dese_list[0]);

    if (0 == mt6628_get_channel_space(freq)) {
        freq *= 10;
    }
    
    while (size) {
        if (mt6628_scan_dese_list[size -1] == freq)
            return 1;

        size--;
    }

    return 0;
}



// return value: 0, low side; 1, high side; else error no
static fm_s32 mt6628_hl_dese(fm_u16 freq, void *arg)
{
    return 0;
}


// return value: 0, fa off; 1, fa on; else error no
static fm_s32 mt6628_fa_dese(fm_u16 freq, void *arg)
{
    return 0;
}


// return value: 0, mcu dese disable; 1, enable; else error no
static fm_s32 mt6628_mcu_dese(fm_u16 freq, void *arg)
{
    fm_mcu_desense_t state = FM_MCU_DESE_DISABLE;
    fm_s32 len = 0;
    fm_s32 indx = 0;

    if (0 == mt6628_get_channel_space(freq)) {
        freq *= 10;
    }
    
    FM_LOG_DBG(D_MAIN, "%s, [freq=%d]\n", __func__, (int)freq);

    len = sizeof(mt6628_mcu_dese_list) / sizeof(mt6628_mcu_dese_list[0]);
    indx = 0;

    while ((indx < len) && (state != FM_MCU_DESE_ENABLE)) {
        if (mt6628_mcu_dese_list[indx] == freq) {
            state = FM_MCU_DESE_ENABLE;
        }

        indx++;
    }

    // request 6628 MCU change clk
    if (state == FM_MCU_DESE_DISABLE) {
        if (!mtk_wcn_wmt_dsns_ctrl(WMTDSNS_FM_DISABLE)) {
            return -1;
        }
        return 0;
    } else {
        if (!mtk_wcn_wmt_dsns_ctrl(WMTDSNS_FM_ENABLE)) {
            return -1;
        }
        return 1;
    }
}



// return value: 0,mcu dese disable; 1, enable; else error no
static fm_s32 mt6628_gps_dese(fm_u16 freq, void *arg)
{
    fm_gps_desense_t state = FM_GPS_DESE_DISABLE;
    fm_s32 len = 0;
    fm_s32 indx = 0;

    if (0 == mt6628_get_channel_space(freq)) {
        freq *= 10;
    }
    
    FM_LOG_DBG(D_MAIN, "%s, [freq=%d]\n", __func__, (int)freq);

    len = sizeof(mt6628_gps_dese_list) / sizeof(mt6628_gps_dese_list[0]);
    indx = 0;

    while ((indx < len) && (state != FM_GPS_DESE_ENABLE)) {
        if (mt6628_gps_dese_list[indx] == freq) {
            state = FM_GPS_DESE_ENABLE;
        }

        indx++;
    }

    // request 6628 GPS change clk
    if (state == FM_GPS_DESE_DISABLE) {
        if  (!mtk_wcn_wmt_dsns_ctrl(WMTDSNS_FM_GPS_DISABLE))  {
            return -1;
        }
        return 0;
    } else {
        if (!mtk_wcn_wmt_dsns_ctrl(WMTDSNS_FM_GPS_ENABLE)) {
            return -1;
        }
        return 1;
    }
}


//get channel parameter, HL side/ FA / ATJ
static fm_u16 mt6628_chan_para_get(fm_u16 freq)
{
    fm_s32 pos, size;

    if (0 == mt6628_get_channel_space(freq)) {
        freq *= 10;
    }

    pos = (freq - 7600) / 5;
    
    size = sizeof(mt6628_chan_para_map) / sizeof(mt6628_chan_para_map[0]);

    pos = (pos < 0) ? 0 : pos;
    pos = (pos > (size - 1)) ? (size - 1) : pos;

    return mt6628_chan_para_map[pos];
}


static fm_s32 init(struct fm_pub *ppub)
{
    fm_s32 ret = 0;
    struct fm_priv priv;

    if (!ppub) return -1;

    ppub->pub_tbl.read = NULL;
    ppub->pub_tbl.write = NULL;
    ppub->pub_tbl.setbits = NULL;
    ppub->pub_tbl.rampdown = NULL;
    ppub->pub_tbl.msdelay = NULL;
    ppub->pub_tbl.usdelay = NULL;
    ppub->pub_tbl.log = NULL;
    ppub->state = UNINITED;
    ppub->data = NULL;

    priv.priv_tbl.hl_dese = mt6628_hl_dese;
    priv.priv_tbl.fa_dese = mt6628_fa_dese;
    priv.priv_tbl.mcu_dese = mt6628_mcu_dese;
    priv.priv_tbl.gps_dese = mt6628_gps_dese;
    priv.priv_tbl.chan_para_get = mt6628_chan_para_get;
    priv.priv_tbl.is_dese_chan = mt6628_is_dese_chan;

    ret = fm_priv_register(&priv, ppub);

    if (ret) {
        FM_LOG_ERR(D_INIT, "FM private module init failed\n");
    }

    FM_LOG_NTC(D_INIT, "FM private module init ok\n");

    return ret;
}


static int uninit(struct fm_pub *ppub)
{
    int ret = 0;

    ret = fm_priv_unregister(NULL, ppub);

    if (ret) {
        FM_LOG_ERR(D_INIT, "FM private module deinit failed\n");
    }

    FM_LOG_NTC(D_INIT, "FM private module deinit ok\n");


    return ret;
}

static fm_s32 __init mtk_fm_probe(void)
{
    fm_s32 ret = 0;

    FM_LOG_NTC(D_INIT, "%s, FM probe ...\n", __func__);
    ret = init(&pub);

    if (ret) {
        FM_LOG_ALT(D_INIT, "%s, FM probe failed\n", __func__);
    }

    FM_LOG_NTC(D_INIT, "%s, FM probe ok\n", __func__);

    return ret;
}

static void __exit mtk_fm_remove(void)
{
    fm_s32 ret = 0;
    ret = uninit(&pub);

    if (ret) {
        FM_LOG_ALT(D_INIT, "%s, FM remove failed\n", __func__);
    }

    FM_LOG_NTC(D_INIT, "%s, FM remove ok\n", __func__);

    return;
}

module_init(mtk_fm_probe);
module_exit(mtk_fm_remove);
MODULE_LICENSE("Proprietary. Send bug reports to hongcheng.xia@MediaTek.com");
MODULE_DESCRIPTION("MediaTek FM Driver Private Part, Need be loaded after FM driver");
MODULE_AUTHOR("Hongcheng <hongcheng.xia@MediaTek.com>");

