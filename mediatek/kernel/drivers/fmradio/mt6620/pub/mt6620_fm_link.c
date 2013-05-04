/* mt6620_fm_link.c
 *
 * (C) Copyright 2011
 * MediaTek <www.MediaTek.com>
 * Hongcheng <hongcheng.xia@MediaTek.com>
 *
 * MT6620 FM Radio Driver -- setup data link
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
#include <linux/slab.h>
#include <linux/interrupt.h>

#include "mtkstp.h"
#include "stp_drv.h"
#include "wmt_exp.h"

#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_stdlib.h"

#include "mt6620_fm.h"
#include "mt6620_fm_link.h"

static struct fm_link_event *link_event;

static fm_u32 fm_event_set(struct fm_link_event* thiz, fm_u32 mask)
{
    thiz->flag |= mask;
    WCN_DBG(FM_DBG | LINK, "set flag 0x%08x\n", thiz->flag);
    wake_up(&thiz->wq);

    return thiz->flag;
}

static fm_u32 fm_event_clr(struct fm_link_event* thiz, fm_u32 mask)
{
    thiz->flag &= ~mask;
    WCN_DBG(FM_DBG | LINK, "clr flag 0x%08x\n", thiz->flag);
    return thiz->flag;
}

/**
 * fm_event_check - sleep until a condition gets true or a timeout elapses
 * @thiz: the pointer of current object
 * @mask: bitmap in fm_u32
 * @timeout: timeout, in jiffies
 *
 * fm_event_set() has to be called after changing any variable that could
 * change the result of the wait condition.
 *
 * The function returns 0 if the @timeout elapsed, and the remaining
 * jiffies if the condition evaluated to true before the timeout elapsed.
 */
static long fm_event_check(struct fm_link_event* thiz, fm_u32 mask, long timeout)
{
    return wait_event_timeout(thiz->wq, ((thiz->flag & mask) == mask), timeout*HZ);
}

static fm_u32 fm_event_get(struct fm_link_event* thiz)
{
    return thiz->flag;

}

static fm_u32 fm_event_rst(struct fm_link_event* thiz)
{
    return thiz->flag = 0;
}

fm_s32 fm_link_setup(void* data)
{
    if (!(link_event = kzalloc(sizeof(struct fm_link_event), GFP_KERNEL))) {
        WCN_DBG(FM_ALT | LINK, "kzalloc(fm_link_event) -ENOMEM\n");
        return -1;
    }

    link_event->rst = fm_event_rst;
    link_event->get = fm_event_get;
    link_event->clr = fm_event_clr;
    link_event->check = fm_event_check;
    link_event->set = fm_event_set;
    init_waitqueue_head(&link_event->wq);

    link_event->rst(link_event); //set flag to 0x00000000

    WCN_DBG(FM_NTC | LINK, "fm link setup\n");
    return 0;
}

fm_s32 fm_link_release(void)
{
    if (link_event) {
        kfree(link_event);
    }

    WCN_DBG(FM_NTC | LINK, "fm link release\n");
    return 0;
}



/*
 * fm_ctrl_rx
 * the low level func to read a rigister
 * @addr - rigister address
 * @val - the pointer of target buf
 * If success, return 0; else error code
 */
fm_s32 fm_ctrl_rx(fm_u8 addr, fm_u16 *val)
{
    return 0;
}

/*
 * fm_ctrl_tx
 * the low level func to write a rigister
 * @addr - rigister address
 * @val - value will be writed in the rigister
 * If success, return 0; else error code
 */
fm_s32 fm_ctrl_tx(fm_u8 addr, fm_u16 val)
{
    return 0;
}

/*
 * fm_cmd_tx() - send cmd to FM firmware and wait event
 * @buf - send buffer
 * @len - the length of cmd
 * @mask - the event flag mask
 * @	cnt - the retry conter
 * @timeout - timeout per cmd
 * Return 0, if success; error code, if failed
 */
fm_s32 fm_cmd_tx(fm_u8* buf, fm_u16 len, fm_s32 mask, fm_s32 cnt, fm_s32 timeout, fm_s32(*callback)(struct fm_res_ctx* result))
{
    fm_s32 ret_time = 0;

    if ((NULL == buf) || (len < 0) || (0 == mask)
            || (cnt > SW_RETRY_CNT_MAX) || (timeout > SW_WAIT_TIMEOUT_MAX)) {
        WCN_DBG(FM_ERR | LINK, "cmd tx, invalid para\n");
        return -FM_EPARA;
    }

sw_retry:

    //send cmd to FM firmware
    if (mtk_wcn_stp_send_data(buf, len, FM_TASK_INDX) == 0) {
        WCN_DBG(FM_EMG | LINK, "send data over stp failed\n");
        return -FM_ELINK;
    }

    //wait the response form FM firmware
    ret_time = link_event->check(link_event, mask, timeout);

    if (!ret_time) {
        if (0 < cnt--) {
            WCN_DBG(FM_WAR | LINK, "wait even timeout, [retry_cnt=%d]\n", cnt);
            goto sw_retry; //retry if timeout and retry cnt > 0
        } else {
            WCN_DBG(FM_ALT | LINK, "fatal error, SW retry failed, reset HW\n");
            return -FM_EFW;
        }
    }

    link_event->clr(link_event, mask);

    if (callback) {
        callback(&link_event->result);
    }

    return 0;
}

fm_s32 fm_event_parser(fm_s32(*rds_parser)(struct rds_rx_t*, fm_s32))
{
    fm_s32 len;
    fm_s32 i = 0;
    fm_u8 opcode = 0;
    fm_u16 length = 0;
    fm_u8 ch;
    fm_u8 rx_buf[RX_BUF_SIZE + 10] = {0}; //the 10 bytes are protect gaps
    static volatile fm_task_parser_state state = FM_TASK_RX_PARSER_PKT_TYPE;

    len = mtk_wcn_stp_receive_data(rx_buf, RX_BUF_SIZE, FM_TASK_INDX);
    WCN_DBG(FM_DBG | LINK, "[len=%d],[CMD=0x%02x 0x%02x 0x%02x 0x%02x]\n", len, rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3]);

    while (i < len) {
        ch = rx_buf[i];

        switch (state) {
        case FM_TASK_RX_PARSER_PKT_TYPE:

            if (ch == FM_TASK_EVENT_PKT_TYPE) {
                if ((i + 5) < RX_BUF_SIZE) {
                    WCN_DBG(FM_DBG | LINK, "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n", rx_buf[i], rx_buf[i+1], rx_buf[i+2], rx_buf[i+3], rx_buf[i+4], rx_buf[i+5]);
                } else {
                    WCN_DBG(FM_DBG | LINK, "0x%02x 0x%02x\n", rx_buf[i], rx_buf[i+1]);
                }

                state = FM_TASK_RX_PARSER_OPCODE;
            } else {
                WCN_DBG(FM_ALT | LINK, "event pkt type error (rx_buf[%d] = 0x%02x)\n", i, ch);
            }

            i++;
            break;

        case FM_TASK_RX_PARSER_OPCODE:
            i++;
            opcode = ch;
            state = FM_TASK_RX_PARSER_PKT_LEN_1;
            break;

        case FM_TASK_RX_PARSER_PKT_LEN_1:
            i++;
            length = ch;
            state = FM_TASK_RX_PARSER_PKT_LEN_2;
            break;

        case FM_TASK_RX_PARSER_PKT_LEN_2:
            i++;
            length |= (fm_u16)(ch << 0x8);

            if (length > 0) {
                state = FM_TASK_RX_PARSER_PKT_PAYLOAD;
            } else {
                state = FM_TASK_RX_PARSER_PKT_TYPE;
                link_event->set(link_event, (1 << opcode));
            }

            break;

        case FM_TASK_RX_PARSER_PKT_PAYLOAD:

            switch (opcode) {
            case FM_SEEK_OPCODE:

                if ((i + 1) < RX_BUF_SIZE) {
                    link_event->result.seek_result = (rx_buf[i] + (rx_buf[i+1] << 8)) / 10;
                }

                link_event->set(link_event, FLAG_SEEK_DONE);
                break;

            case FM_SCAN_OPCODE:

                //check if the result data is long enough
                if ((RX_BUF_SIZE - i) < (sizeof(fm_u16) * FM_SCANTBL_SIZE)) {
                    WCN_DBG(FM_ALT | LINK, "FM_SCAN_OPCODE err, [tblsize=%d],[bufsize=%d]\n", (sizeof(fm_u16) * FM_SCANTBL_SIZE), (RX_BUF_SIZE - i));
                    link_event->set(link_event, FLAG_SCAN_DONE);
                    return 0;
                }

                fm_memcpy(link_event->result.scan_result, &rx_buf[i], sizeof(fm_u16) * FM_SCANTBL_SIZE);
                link_event->set(link_event, FLAG_SCAN_DONE);
                break;

            case FSPI_READ_OPCODE:

                if ((i + 1) < RX_BUF_SIZE) {
                    link_event->result.fspi_rd = (rx_buf[i] + (rx_buf[i+1] << 8));
                }

                link_event->set(link_event, (1 << opcode));
                break;

            case RDS_RX_DATA_OPCODE:

                //check if the rds data is long enough
                if ((RX_BUF_SIZE - i) < length) {
                    WCN_DBG(FM_ALT | LINK, "RDS RX err, [rxlen=%d],[bufsize=%d]\n", (fm_s32)length, (RX_BUF_SIZE - i));
                    link_event->set(link_event, (1 << opcode));
                    break;
                }

                //copy rds data to rds buf
                fm_memcpy(&link_event->result.rds_rx_result, &rx_buf[i], length);

                /*Handle the RDS data that we get*/
                if (rds_parser) {
                    rds_parser(&link_event->result.rds_rx_result, length);
                } else {
                    WCN_DBG(FM_WAR | LINK, "no method to parse RDS data\n");
                }

                link_event->set(link_event, (1 << opcode));
                break;

            default:
                link_event->set(link_event, (1 << opcode));
                break;
            }

            state = FM_TASK_RX_PARSER_PKT_TYPE;
            i += length;
            break;

        default:
            break;
        }
    }

    return 0;
}

fm_bool fm_wait_stc_done(fm_u32 sec)
{
    return fm_true;
}

fm_s32 fm_force_active_event(fm_u32 mask)
{
    fm_u32 flag;

    flag = link_event->get(link_event);
    WCN_DBG(FM_WAR | LINK, "befpre force active event, [flag=0x%04x]\n", flag);
    flag = link_event->set(link_event, mask);
    WCN_DBG(FM_WAR | LINK, "after force active event, [flag=0x%04x]\n", flag);

    return 0;
}

