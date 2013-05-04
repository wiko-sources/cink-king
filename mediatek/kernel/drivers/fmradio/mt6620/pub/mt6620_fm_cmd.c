#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fm.h>

#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_rds.h"

#include "mt6620_fm_reg.h"
#include "mt6620_fm_link.h"
#include "mt6620_fm.h"
#include "mt6620_fm_cmd.h"


static fm_s32 fm_bop_write(fm_u8 addr, fm_u16 value, fm_u8 *buf, fm_s32 size)
{
    if (size < (FM_WRITE_BASIC_OP_SIZE + 2)) {
        return (-1);
    }

    if (buf == NULL) {
        return (-2);
    }

    buf[0] = FM_WRITE_BASIC_OP;
    buf[1] = FM_WRITE_BASIC_OP_SIZE;
    buf[2] = addr;
    buf[3] = (fm_u8)((value) & 0x00FF);
    buf[4] = (fm_u8)((value >> 8) & 0x00FF);

    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4]);

    return (FM_WRITE_BASIC_OP_SIZE + 2);
}

static fm_s32 fm_bop_udelay(fm_u32 value, fm_u8 *buf, fm_s32 size)
{
    if (size < (FM_UDELAY_BASIC_OP_SIZE + 2)) {
        return (-1);
    }

    if (buf == NULL) {
        return (-2);
    }

    buf[0] = FM_UDELAY_BASIC_OP;
    buf[1] = FM_UDELAY_BASIC_OP_SIZE;
    buf[2] = (fm_u8)((value) & 0x000000FF);
    buf[3] = (fm_u8)((value >> 8) & 0x000000FF);
    buf[4] = (fm_u8)((value >> 16) & 0x000000FF);
    buf[5] = (fm_u8)((value >> 24) & 0x000000FF);

    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

    return (FM_UDELAY_BASIC_OP_SIZE + 2);
}

static fm_s32 fm_bop_rd_until(fm_u8 addr, fm_u16 mask, fm_u16 value, fm_u8 *buf, fm_s32 size)
{
    if (size < (FM_RD_UNTIL_BASIC_OP_SIZE + 2)) {
        return (-1);
    }

    if (buf == NULL) {
        return (-2);
    }

    buf[0] = FM_RD_UNTIL_BASIC_OP;
    buf[1] = FM_RD_UNTIL_BASIC_OP_SIZE;
    buf[2] = addr;
    buf[3] = (fm_u8)((mask) & 0x00FF);
    buf[4] = (fm_u8)((mask >> 8) & 0x00FF);
    buf[5] = (fm_u8)((value) & 0x00FF);
    buf[6] = (fm_u8)((value >> 8) & 0x00FF);

    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    return (FM_RD_UNTIL_BASIC_OP_SIZE + 2);
}

static fm_s32 fm_bop_modify(fm_u8 addr, fm_u16 mask_and, fm_u16 mask_or, fm_u8 *buf, fm_s32 size)
{
    if (size < (FM_MODIFY_BASIC_OP_SIZE + 2)) {
        return (-1);
    }

    if (buf == NULL) {
        return (-2);
    }

    buf[0] = FM_MODIFY_BASIC_OP;
    buf[1] = FM_MODIFY_BASIC_OP_SIZE;
    buf[2] = addr;
    buf[3] = (fm_u8)((mask_and) & 0x00FF);
    buf[4] = (fm_u8)((mask_and >> 8) & 0x00FF);
    buf[5] = (fm_u8)((mask_or) & 0x00FF);
    buf[6] = (fm_u8)((mask_or >> 8) & 0x00FF);

    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    return (FM_MODIFY_BASIC_OP_SIZE + 2);
}

#if 0
static fm_s32 fm_bop_msleep(fm_u32 value, fm_u8 *buf, fm_s32 size)
{
    if (size < (FM_MSLEEP_BASIC_OP_SIZE + 2)) {
        return (-1);
    }

    if (buf == NULL) {
        return (-2);
    }

    buf[0] = FM_MSLEEP_BASIC_OP;
    buf[1] = FM_MSLEEP_BASIC_OP_SIZE;
    buf[2] = (fm_u8)((value) & 0x000000FF);
    buf[3] = (fm_u8)((value >> 8) & 0x000000FF);
    buf[4] = (fm_u8)((value >> 16) & 0x000000FF);
    buf[5] = (fm_u8)((value >> 24) & 0x000000FF);

    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

    return (FM_MSLEEP_BASIC_OP_SIZE + 2);
}
#endif

/*
 * mt6620_pwrup_clock_on - Wholechip FM Power Up: step 1, FM Digital Clock enable
 * @buf - target buf
 * @buf_size - buffer size
 * return package size
 */
fm_s32 mt6620_off_2_longANA_1(fm_u8 *buf, fm_s32 buf_size)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_ENABLE_OPCODE;
    pkt_size = 4;

    //A01 Turn on the bandgap and central biasing core
    pkt_size += fm_bop_write(0x01, 0x4A00, &buf[pkt_size], buf_size - pkt_size);//wr 1 4A00
    pkt_size += fm_bop_udelay(30, &buf[pkt_size], buf_size - pkt_size);//delay 30
    pkt_size += fm_bop_write(0x01, 0x6A00, &buf[pkt_size], buf_size - pkt_size);//wr 1 6A00
    pkt_size += fm_bop_udelay(50, &buf[pkt_size], buf_size - pkt_size);//delay 50
    //A02 Initialise the LDO's Output
    pkt_size += fm_bop_write(0x02, 0x299C, &buf[pkt_size], buf_size - pkt_size);//wr 2 299C
    //A03 Enable RX, ADC and ADPLL LDO
    pkt_size += fm_bop_write(0x01, 0x6B82, &buf[pkt_size], buf_size - pkt_size);//wr 1 6B82
    //A04 Update FMRF optimized register settings
    pkt_size += fm_bop_write(0x04, 0x0142, &buf[pkt_size], buf_size - pkt_size);//wr 4 0142
    pkt_size += fm_bop_write(0x05, 0x00E7, &buf[pkt_size], buf_size - pkt_size);//wr 5 00E7
    pkt_size += fm_bop_write(0x0A, 0x0060, &buf[pkt_size], buf_size - pkt_size);//wr a 0060
    pkt_size += fm_bop_write(0x0C, 0xA88C, &buf[pkt_size], buf_size - pkt_size);//wr c A88C
    pkt_size += fm_bop_write(0x0D, 0x0888, &buf[pkt_size], buf_size - pkt_size);//wr d 0888
    pkt_size += fm_bop_write(0x10, 0x1E8D, &buf[pkt_size], buf_size - pkt_size);//wr 10 1E8D
    pkt_size += fm_bop_write(0x27, 0x0005, &buf[pkt_size], buf_size - pkt_size);//wr 27 0005
    pkt_size += fm_bop_write(0x0E, 0x0040, &buf[pkt_size], buf_size - pkt_size);//wr e 0040
    pkt_size += fm_bop_write(0x03, 0x50F0, &buf[pkt_size], buf_size - pkt_size);//wr 3 50f0
    pkt_size += fm_bop_write(0x3F, 0xAD06, &buf[pkt_size], buf_size - pkt_size);//wr 3f AD06
    pkt_size += fm_bop_write(0x3E, 0x3280, &buf[pkt_size], buf_size - pkt_size);//wr 3e 3280
    pkt_size += fm_bop_write(0x06, 0x0124, &buf[pkt_size], buf_size - pkt_size);//wr 6 0124
    pkt_size += fm_bop_write(0x08, 0x15B8, &buf[pkt_size], buf_size - pkt_size);//wr 8 15B8
    //A05 Enable RX related blocks
    pkt_size += fm_bop_write(0x28, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 28 0000
    pkt_size += fm_bop_write(0x00, 0x0166, &buf[pkt_size], buf_size - pkt_size);//wr 0 0166
    pkt_size += fm_bop_write(0x3A, 0x0004, &buf[pkt_size], buf_size - pkt_size);//wr 3a 0004
    pkt_size += fm_bop_write(0x37, 0x2590, &buf[pkt_size], buf_size - pkt_size);//wr 37 2590
    // FM ADPLL Power Up
    // () for 16.384M mode, otherwise 15.36M
    pkt_size += fm_bop_write(0x25, 0x040F, &buf[pkt_size], buf_size - pkt_size);//wr 25 040f
    pkt_size += fm_bop_write(0x20, 0x2720, &buf[pkt_size], buf_size - pkt_size);//wr 20 2720
    //XHC, 2011/03/18, [wr 22 9980->6680]
    pkt_size += fm_bop_write(0x22, 0x6680, &buf[pkt_size], buf_size - pkt_size);//wr 22 9980
    pkt_size += fm_bop_write(0x25, 0x080F, &buf[pkt_size], buf_size - pkt_size);//wr 25 080f
    pkt_size += fm_bop_write(0x1E, 0x0863, &buf[pkt_size], buf_size - pkt_size);//wr 1e 0863(0A63)
    pkt_size += fm_bop_udelay(5000, &buf[pkt_size], buf_size - pkt_size);//delay 5000
    pkt_size += fm_bop_write(0x1E, 0x0865, &buf[pkt_size], buf_size - pkt_size);//wr 1e 0865 (0A65)
    pkt_size += fm_bop_udelay(5000, &buf[pkt_size], buf_size - pkt_size);//delay 5000
    pkt_size += fm_bop_write(0x1E, 0x0871, &buf[pkt_size], buf_size - pkt_size);//wr 1e 0871 (0A71)

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_off_2_longANA_2(fm_u8 *buf, fm_s32 buf_size)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_ENABLE_OPCODE;
    pkt_size = 4;

    pkt_size += fm_bop_udelay(100000, &buf[pkt_size], buf_size - pkt_size);//delay 100000
    pkt_size += fm_bop_write(0x2A, 0x1026, &buf[pkt_size], buf_size - pkt_size);//wr 2a 1026
    // FM RC Calibration
    pkt_size += fm_bop_write(0x00, 0x01E6, &buf[pkt_size], buf_size - pkt_size);//wr 0 01E6
    pkt_size += fm_bop_udelay(1, &buf[pkt_size], buf_size - pkt_size);//delay 1
    pkt_size += fm_bop_write(0x1B, 0x0094, &buf[pkt_size], buf_size - pkt_size);//wr 1b 0094
    pkt_size += fm_bop_write(0x1B, 0x0095, &buf[pkt_size], buf_size - pkt_size);//wr 1b 0095
    pkt_size += fm_bop_udelay(200, &buf[pkt_size], buf_size - pkt_size);//delay 200
    pkt_size += fm_bop_write(0x1B, 0x0094, &buf[pkt_size], buf_size - pkt_size);//wr 1b 0094
    pkt_size += fm_bop_write(0x00, 0x0166, &buf[pkt_size], buf_size - pkt_size);//wr 0 0166
    // FM VCO Enable
    pkt_size += fm_bop_write(0x01, 0x6B8A, &buf[pkt_size], buf_size - pkt_size);//wr 1 6B8A
    pkt_size += fm_bop_udelay(1, &buf[pkt_size], buf_size - pkt_size);//delay 1
    pkt_size += fm_bop_write(0x00, 0xC166, &buf[pkt_size], buf_size - pkt_size);//wr 0 C166
    pkt_size += fm_bop_udelay(3000, &buf[pkt_size], buf_size - pkt_size);//delay 3000
    pkt_size += fm_bop_write(0x00, 0xF166, &buf[pkt_size], buf_size - pkt_size);//wr 0 F166
    pkt_size += fm_bop_write(0x09, 0x2964, &buf[pkt_size], buf_size - pkt_size);//wr 9 2964
    // FM RFDIG settings
    pkt_size += fm_bop_write(0x2E, 0x0008, &buf[pkt_size], buf_size - pkt_size);//wr 2e 8
    pkt_size += fm_bop_write(0x2B, 0x0064, &buf[pkt_size], buf_size - pkt_size);//wr 2b 64
    pkt_size += fm_bop_write(0x2C, 0x0032, &buf[pkt_size], buf_size - pkt_size);//wr 2c 32
    pkt_size += fm_bop_write(0x11, 0x17d4, &buf[pkt_size], buf_size - pkt_size);//wr 11 17d4
    //Update dynamic subband switching setting, XHC 2011/05/17
    pkt_size += fm_bop_write(0x13, 0xFA00, &buf[pkt_size], buf_size - pkt_size);//wr 13 FA00
    pkt_size += fm_bop_write(0x14, 0x0580, &buf[pkt_size], buf_size - pkt_size);//wr 14 0580
    pkt_size += fm_bop_write(0x15, 0xFA80, &buf[pkt_size], buf_size - pkt_size);//wr 15 FA80
    pkt_size += fm_bop_write(0x16, 0x0580, &buf[pkt_size], buf_size - pkt_size);//wr 16 0580
    pkt_size += fm_bop_write(0x33, 0x0008, &buf[pkt_size], buf_size - pkt_size);//wr 33 0008
    // FM DCOC Calibration
    pkt_size += fm_bop_write(0x64, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 64 1
    pkt_size += fm_bop_write(0x63, 0x0020, &buf[pkt_size], buf_size - pkt_size);//wr 63 20
    pkt_size += fm_bop_write(0x9C, 0x0044, &buf[pkt_size], buf_size - pkt_size);//wr 9C 0044
    //pkt_size += fm_bop_write(0x6B, 0x0100, &buf[pkt_size], buf_size - pkt_size);//"Disable other interrupts except for STC_DONE(dependent on interrupt output source selection)"
    pkt_size += fm_bop_write(0x0F, 0x1A08, &buf[pkt_size], buf_size - pkt_size);//wr F 1A08
    pkt_size += fm_bop_write(0x63, 0x0021, &buf[pkt_size], buf_size - pkt_size);//wr 63 21
    pkt_size += fm_bop_rd_until(0x69, 0x0001, 0x0001, &buf[pkt_size], buf_size - pkt_size);//Poll fm_intr_stc_done (69H D0) = 1
    pkt_size += fm_bop_write(0x69, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 69 1
    pkt_size += fm_bop_write(0x63, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 63 0
    pkt_size += fm_bop_rd_until(0x6F, 0x0001, 0x0000, &buf[pkt_size], buf_size - pkt_size);//Poll stc_done (6FH D0)= 0
    // Others
    pkt_size += fm_bop_write(0x00, 0xF167, &buf[pkt_size], buf_size - pkt_size);//wr 0 F167

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}


/*
 * mt6620_patch_download - Wholechip FM Power Up: step 3, download patch to f/w,
 * @buf - target buf
 * @buf_size - buffer size
 * @seg_num - total segments that this patch divided into
 * @seg_id - No. of Segments: segment that will now be sent
 * @src - patch source buffer
 * @seg_len - segment size: segment that will now be sent
 * return package size
 */
fm_s32 mt6620_patch_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id, const fm_u8 *src, fm_s32 seg_len)
{
    fm_s32 pkt_size = 0;
    fm_u8 *dst = NULL;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_PATCH_DOWNLOAD_OPCODE;
    pkt_size = 4;

    buf[pkt_size++] = seg_num;
    buf[pkt_size++] = seg_id;

    if (seg_len > (buf_size - pkt_size)) {
        return -1;
    }

    dst = &buf[pkt_size];
    pkt_size += seg_len;

    //copy patch to tx buffer
    while (seg_len--) {
        *dst = *src;
        //printk(KERN_ALERT "%02x ", *dst);
        src++;
        dst++;
    }

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);
    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    return pkt_size;
}

/*
 * mt6620_coeff_download - Wholechip FM Power Up: step 3,download coeff to f/w,
 * @buf - target buf
 * @buf_size - buffer size
 * @seg_num - total segments that this patch divided into
 * @seg_id - No. of Segments: segment that will now be sent
 * @src - patch source buffer
 * @seg_len - segment size: segment that will now be sent
 * return package size
 */
fm_s32 mt6620_coeff_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id, const fm_u8 *src, fm_s32 seg_len)
{
    fm_s32 pkt_size = 0;
    fm_u8 *dst = NULL;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_COEFF_DOWNLOAD_OPCODE;
    pkt_size = 4;

    buf[pkt_size++] = seg_num;
    buf[pkt_size++] = seg_id;

    if (seg_len > (buf_size - pkt_size)) {
        return -1;
    }

    dst = &buf[pkt_size];
    pkt_size += seg_len;

    //copy patch to tx buffer
    while (seg_len--) {
        *dst = *src;
        //printk(KERN_ALERT "%02x ", *dst);
        src++;
        dst++;
    }

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);
    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    return pkt_size;
}

/*
 * mt6620_hwcoeff_download - Wholechip FM Power Up: step 3,download hwcoeff to f/w,
 * @buf - target buf
 * @buf_size - buffer size
 * @seg_num - total segments that this patch divided into
 * @seg_id - No. of Segments: segment that will now be sent
 * @src - patch source buffer
 * @seg_len - segment size: segment that will now be sent
 * return package size
 */
fm_s32 mt6620_hwcoeff_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id, const fm_u8 *src, fm_s32 seg_len)
{
    fm_s32 pkt_size = 0;
    fm_u8 *dst = NULL;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_HWCOEFF_DOWNLOAD_OPCODE;
    pkt_size = 4;

    buf[pkt_size++] = seg_num;
    buf[pkt_size++] = seg_id;

    if (seg_len > (buf_size - pkt_size)) {
        return -1;
    }

    dst = &buf[pkt_size];
    pkt_size += seg_len;

    //copy patch to tx buffer
    while (seg_len--) {
        *dst = *src;
        //printk(KERN_ALERT "%02x ", *dst);
        src++;
        dst++;
    }

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);
    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    return pkt_size;
}

/*
 * mt6620_rom_download - Wholechip FM Power Up: step 3,download rom to f/w,
 * @buf - target buf
 * @buf_size - buffer size
 * @seg_num - total segments that this patch divided into
 * @seg_id - No. of Segments: segment that will now be sent
 * @src - patch source buffer
 * @seg_len - segment size: segment that will now be sent
 * return package size
 */
fm_s32 mt6620_rom_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id, const fm_u8 *src, fm_s32 seg_len)
{
    fm_s32 pkt_size = 0;
    fm_u8 *dst = NULL;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_ROM_DOWNLOAD_OPCODE;
    pkt_size = 4;

    buf[pkt_size++] = seg_num;
    buf[pkt_size++] = seg_id;

    if (seg_len > (buf_size - pkt_size)) {
        return -1;
    }

    dst = &buf[pkt_size];
    pkt_size += seg_len;

    //copy patch to tx buffer
    while (seg_len--) {
        *dst = *src;
        //printk(KERN_ALERT "%02x ", *dst);
        src++;
        dst++;
    }

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);
    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    return pkt_size;
}

/*
 * mt6620_pwrup_digital_init - Wholechip FM Power Up: step 4, FM Digital Init: fm_rgf_maincon
 * @buf - target buf
 * @buf_size - buffer size
 * return package size
 */
fm_s32 mt6620_pwrup_digital_init_1(fm_u8 *buf, fm_s32 buf_size)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_ENABLE_OPCODE;
    pkt_size = 4;


    // fm_rgf_maincon
    //rd 62
    pkt_size += fm_bop_write(0x65, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 65 0
    pkt_size += fm_bop_write(0x64, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 64 1
    pkt_size += fm_bop_write(0x63, 0x0480, &buf[pkt_size], buf_size - pkt_size);//wr 63 480
    pkt_size += fm_bop_write(0x6D, 0x1AB2, &buf[pkt_size], buf_size - pkt_size);//wr 6d 1ab2
    pkt_size += fm_bop_write(0x6B, 0x2100, &buf[pkt_size], buf_size - pkt_size);//wr 6b 2100
    pkt_size += fm_bop_write(0x68, 0xE100, &buf[pkt_size], buf_size - pkt_size);//wr 68 E100

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_pwrup_digital_init_2(fm_u8 *buf, fm_s32 buf_size)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_ENABLE_OPCODE;
    pkt_size = 4;

    pkt_size += fm_bop_write(0x68, 0xE000, &buf[pkt_size], buf_size - pkt_size);//wr 68 E000
    // fm_rgf_dac
    pkt_size += fm_bop_write(0x9C, 0xab48, &buf[pkt_size], buf_size - pkt_size);//wr 9c ab48
    pkt_size += fm_bop_write(0x9E, 0x000C, &buf[pkt_size], buf_size - pkt_size);//wr 9e c
    pkt_size += fm_bop_write(0x71, 0x807f, &buf[pkt_size], buf_size - pkt_size);//wr 71 807f
    pkt_size += fm_bop_write(0x72, 0x878f, &buf[pkt_size], buf_size - pkt_size);//wr 72 878f
    //XHC, 2011/04/29 update 0x73 form 0x07C3 to 0x07C1 speed up I/Q calibration
    pkt_size += fm_bop_write(0x73, 0x07c1, &buf[pkt_size], buf_size - pkt_size);//wr 73 7c3
    // fm_rgf_front
    pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 9f 0
    pkt_size += fm_bop_write(0xCB, 0x00B0, &buf[pkt_size], buf_size - pkt_size);//wr cb b0
    //XHC, 2011/05/06 FM RSSI config
    pkt_size += fm_bop_write(0xE0, ((0xA301 & 0xFC00) | (FMR_RSSI_TH_LONG & 0x03FF)), &buf[pkt_size], buf_size - pkt_size);//wr e0 a301
    pkt_size += fm_bop_write(0xE1, ((0x00E9 & 0xFF00) | (FMR_CQI_TH & 0x00FF)), &buf[pkt_size], buf_size - pkt_size);//wr e1 D0~D7, PAMD TH
    //XHC, 2011/04/15 update search MR threshold
    pkt_size += fm_bop_write(0xE3, FMR_MR_TH, &buf[pkt_size], buf_size - pkt_size);//wr e3 1B0
    pkt_size += fm_bop_write(0xE4, 0x008F, &buf[pkt_size], buf_size - pkt_size);//wr e4 8f
    pkt_size += fm_bop_write(0xCC, 0x0488, &buf[pkt_size], buf_size - pkt_size);//wr cc 488
    pkt_size += fm_bop_write(0xD6, 0x036A, &buf[pkt_size], buf_size - pkt_size);//wr d6 36a
    pkt_size += fm_bop_write(0xD7, 0x836a, &buf[pkt_size], buf_size - pkt_size);//wr d7 836a
    pkt_size += fm_bop_write(0xDD, 0x0080, &buf[pkt_size], buf_size - pkt_size);//wr dd 80
    pkt_size += fm_bop_write(0xB0, 0xcd00, &buf[pkt_size], buf_size - pkt_size);//wr b0 cd00
    //XHC, 2011/03/18 Update AFC gain[wr 96 41E2->4000][wr 97 049A->021F]
    //[wr 98 0B66->0D00][wr 99 0E1E->0E7F][wr D0 8233->8192][wr D1 20BC->2086]
    //[wr 90 03FF->0192][wr 91 01BE->0086][wr 92 03FF->0192][wr 93 0354->0086]
    //[wr 94 03FF->0192][wr 95 0354->0086][wr 52 17F3]
    pkt_size += fm_bop_write(0x96, 0x4000, &buf[pkt_size], buf_size - pkt_size);//wr 96     41E2
    pkt_size += fm_bop_write(0x97, 0x021F, &buf[pkt_size], buf_size - pkt_size);//wr 97     049A
    pkt_size += fm_bop_write(0x98, 0x0D00, &buf[pkt_size], buf_size - pkt_size);//wr  98    0B66
    pkt_size += fm_bop_write(0x99, 0x0E7F, &buf[pkt_size], buf_size - pkt_size);//wr 99     0E1E
    pkt_size += fm_bop_write(0xD0, 0x8192, &buf[pkt_size], buf_size - pkt_size);//wr D0    8233
    pkt_size += fm_bop_write(0xD1, 0x2086, &buf[pkt_size], buf_size - pkt_size);//wr D1    20BC
    pkt_size += fm_bop_write(0x90, 0x0192, &buf[pkt_size], buf_size - pkt_size);//wr 90     03ff
    pkt_size += fm_bop_write(0x91, 0x0086, &buf[pkt_size], buf_size - pkt_size);//wr 91     01BE
    pkt_size += fm_bop_write(0x92, 0x0192, &buf[pkt_size], buf_size - pkt_size);//wr 92     03FF
    pkt_size += fm_bop_write(0x93, 0x0086, &buf[pkt_size], buf_size - pkt_size);//wr 93     0354
    pkt_size += fm_bop_write(0x94, 0x0192, &buf[pkt_size], buf_size - pkt_size);//wr 94     03FF
    pkt_size += fm_bop_write(0x95, 0x0086, &buf[pkt_size], buf_size - pkt_size);//wr 95     0354
    pkt_size += fm_bop_write(0x52, 0x17F3, &buf[pkt_size], buf_size - pkt_size);//wr 95     0354
    // fm_rgf_fmx
    pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 9f 1
    pkt_size += fm_bop_write(0xDE, 0x3388, &buf[pkt_size], buf_size - pkt_size);//wr de 3388
    pkt_size += fm_bop_write(0xC2, 0x0180, &buf[pkt_size], buf_size - pkt_size);//wr c2 180
    pkt_size += fm_bop_write(0xC3, 0x0180, &buf[pkt_size], buf_size - pkt_size);//wr c3 180
    pkt_size += fm_bop_write(0xDB, 0x0181, &buf[pkt_size], buf_size - pkt_size);//wr db 181
    pkt_size += fm_bop_write(0xDC, 0x0184, &buf[pkt_size], buf_size - pkt_size);//wr dc 184
    pkt_size += fm_bop_write(0xA2, 0x03C0, &buf[pkt_size], buf_size - pkt_size);//wr a2 3c0
    pkt_size += fm_bop_write(0xA3, 0x03C0, &buf[pkt_size], buf_size - pkt_size);//wr a3 3c0
    pkt_size += fm_bop_write(0xA4, 0x03C0, &buf[pkt_size], buf_size - pkt_size);//wr a4 3c0
    pkt_size += fm_bop_write(0xA5, 0x03C0, &buf[pkt_size], buf_size - pkt_size);//wr a5 3c0
    pkt_size += fm_bop_write(0xA6, 0x03C0, &buf[pkt_size], buf_size - pkt_size);//wr a6 3c0
    pkt_size += fm_bop_write(0xA7, 0x03C0, &buf[pkt_size], buf_size - pkt_size);//wr a7 3c0
    pkt_size += fm_bop_write(0xE3, 0x0280, &buf[pkt_size], buf_size - pkt_size);//wr e3 280
    pkt_size += fm_bop_write(0xE4, 0x0280, &buf[pkt_size], buf_size - pkt_size);//wr e4 280
    pkt_size += fm_bop_write(0xE5, 0x0280, &buf[pkt_size], buf_size - pkt_size);//wr e5 280
    pkt_size += fm_bop_write(0xE6, 0x026C, &buf[pkt_size], buf_size - pkt_size);//wr e6 26c
    pkt_size += fm_bop_write(0xE7, 0x026C, &buf[pkt_size], buf_size - pkt_size);//wr e7 26c
    pkt_size += fm_bop_write(0xEA, 0x026C, &buf[pkt_size], buf_size - pkt_size);//wr ea 26c
    pkt_size += fm_bop_udelay(1000, &buf[pkt_size], buf_size - pkt_size);//delay 1000
    pkt_size += fm_bop_write(0xB6, 0x03FC, &buf[pkt_size], buf_size - pkt_size);//wr b6 3fc
    pkt_size += fm_bop_write(0xB7, 0x02C1, &buf[pkt_size], buf_size - pkt_size);//wr b7 2c1
    pkt_size += fm_bop_write(0xA8, 0x0820, &buf[pkt_size], buf_size - pkt_size);//wr a8 820
    pkt_size += fm_bop_write(0xAC, 0x065E, &buf[pkt_size], buf_size - pkt_size);//wr ac 65e
    pkt_size += fm_bop_write(0xAD, 0x09AD, &buf[pkt_size], buf_size - pkt_size);//wr ad 9ad
    pkt_size += fm_bop_write(0xAE, 0x0CF8, &buf[pkt_size], buf_size - pkt_size);//wr ae cf8
    pkt_size += fm_bop_write(0xAF, 0x0302, &buf[pkt_size], buf_size - pkt_size);//wr af 302
    pkt_size += fm_bop_write(0xB0, 0x04A6, &buf[pkt_size], buf_size - pkt_size);//wr b0 4a6
    pkt_size += fm_bop_write(0xB1, 0x062C, &buf[pkt_size], buf_size - pkt_size);//wr b1 62c
    pkt_size += fm_bop_write(0xEC, 0x014A, &buf[pkt_size], buf_size - pkt_size);//wr ec 14a
    pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);//wr c8 232
    pkt_size += fm_bop_write(0xC7, 0x0184, &buf[pkt_size], buf_size - pkt_size);//wr c7 0184
    pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);//wr d8 0e8
    pkt_size += fm_bop_write(0xFB, 0x051F, &buf[pkt_size], buf_size - pkt_size);//wr fb 51f
    pkt_size += fm_bop_udelay(1000, &buf[pkt_size], buf_size - pkt_size);//delay 1000
    //XHC,2011/03/18 [wr C9 01F0][wr CA 0250][wr D4 2657]
    pkt_size += fm_bop_write(0xC9, 0x01F0, &buf[pkt_size], buf_size - pkt_size);
    pkt_size += fm_bop_write(0xCA, 0x0250, &buf[pkt_size], buf_size - pkt_size);
    pkt_size += fm_bop_write(0xD4, 0x2657, &buf[pkt_size], buf_size - pkt_size);
    pkt_size += fm_bop_write(0x9F, 0x0002, &buf[pkt_size], buf_size - pkt_size);//wr 9f 2
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0xFF80, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff80
    pkt_size += fm_bop_write(0xA8, 0x0061, &buf[pkt_size], buf_size - pkt_size);//wr a8 61
    pkt_size += fm_bop_write(0xA8, 0xFF22, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff22
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0x0100, &buf[pkt_size], buf_size - pkt_size);//wr a8 100
    pkt_size += fm_bop_write(0xA8, 0x009A, &buf[pkt_size], buf_size - pkt_size);//wr a8 9a
    pkt_size += fm_bop_write(0xA8, 0xFF80, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff80
    pkt_size += fm_bop_write(0xA8, 0x0140, &buf[pkt_size], buf_size - pkt_size);//wr a8 140
    pkt_size += fm_bop_write(0xA8, 0xFF42, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff42
    pkt_size += fm_bop_write(0xA8, 0xFFE0, &buf[pkt_size], buf_size - pkt_size);//wr a8 ffe0
    pkt_size += fm_bop_write(0xA8, 0x0114, &buf[pkt_size], buf_size - pkt_size);//wr a8 114
    pkt_size += fm_bop_write(0xA8, 0xFF4E, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff4e
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0x003E, &buf[pkt_size], buf_size - pkt_size);//wr a8 3e
    pkt_size += fm_bop_write(0xA8, 0xFF6E, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff6e
    pkt_size += fm_bop_write(0xA8, 0x0087, &buf[pkt_size], buf_size - pkt_size);//wr a8 87
    pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a8 0
    pkt_size += fm_bop_write(0xA8, 0xFEDC, &buf[pkt_size], buf_size - pkt_size);//wr a8 fedc
    pkt_size += fm_bop_write(0xA8, 0x0015, &buf[pkt_size], buf_size - pkt_size);//wr a8 15
    pkt_size += fm_bop_write(0xA8, 0x00CF, &buf[pkt_size], buf_size - pkt_size);//wr a8 cf
    pkt_size += fm_bop_write(0xA8, 0xFF6B, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff6b
    pkt_size += fm_bop_write(0xA8, 0x0164, &buf[pkt_size], buf_size - pkt_size);//wr a8 164
    pkt_size += fm_bop_write(0xA8, 0x002B, &buf[pkt_size], buf_size - pkt_size);//wr a8 2b
    pkt_size += fm_bop_write(0xA8, 0xFF7E, &buf[pkt_size], buf_size - pkt_size);//wr a8 ff7e
    pkt_size += fm_bop_write(0xA8, 0x0065, &buf[pkt_size], buf_size - pkt_size);//wr a8 65

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_pwrup_digital_init_3(fm_u8 *buf, fm_s32 buf_size)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_ENABLE_OPCODE;
    pkt_size = 4;

    pkt_size += fm_bop_write(0x9F, 0x0002, &buf[pkt_size], buf_size - pkt_size);//wr 9f 2
    pkt_size += fm_bop_write(0xA9, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a9 0
    pkt_size += fm_bop_write(0xA9, 0xFFEE, &buf[pkt_size], buf_size - pkt_size);//wr a9 ffee
    pkt_size += fm_bop_write(0xA9, 0xFF81, &buf[pkt_size], buf_size - pkt_size);//wr a9 ff81
    //XHC,2011/03/18 [wr A9 FFC3]
    pkt_size += fm_bop_write(0xA9, 0xFFC3, &buf[pkt_size], buf_size - pkt_size);
    pkt_size += fm_bop_write(0xA9, 0x0022, &buf[pkt_size], buf_size - pkt_size);//wr a9 22
    pkt_size += fm_bop_write(0xA9, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr a9 0
    pkt_size += fm_bop_write(0xA9, 0xFFCC, &buf[pkt_size], buf_size - pkt_size);//wr a9 ffcc
    pkt_size += fm_bop_write(0xA9, 0x0023, &buf[pkt_size], buf_size - pkt_size);//wr a9 23
    pkt_size += fm_bop_write(0xA9, 0xFFDA, &buf[pkt_size], buf_size - pkt_size);//wr a9 ffda
    pkt_size += fm_bop_write(0xA9, 0xFFF7, &buf[pkt_size], buf_size - pkt_size);//wr a9 fff7
    pkt_size += fm_bop_udelay(10, &buf[pkt_size], buf_size - pkt_size);//delay 10
    pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 9f 1
    pkt_size += fm_bop_write(0xD3, 0x250b, &buf[pkt_size], buf_size - pkt_size);//wr d3 250b
    pkt_size += fm_bop_write(0xBB, 0x2320, &buf[pkt_size], buf_size - pkt_size);//wr bb 2320
    pkt_size += fm_bop_write(0xD0, 0x02f8, &buf[pkt_size], buf_size - pkt_size);//wr d0 02f8
    pkt_size += fm_bop_write(0xEC, 0x019a, &buf[pkt_size], buf_size - pkt_size);//wr ec 19a
    pkt_size += fm_bop_write(0xFE, 0x2140, &buf[pkt_size], buf_size - pkt_size);//wr fe 2140
    pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 9f 0
    // fm_rgf_rds
    pkt_size += fm_bop_write(0x9F, 0x0003, &buf[pkt_size], buf_size - pkt_size);//wr 9f 3
    pkt_size += fm_bop_write(0xBD, 0x37EB, &buf[pkt_size], buf_size - pkt_size);//wr bd 37eb
    pkt_size += fm_bop_write(0xBC, 0x0808, &buf[pkt_size], buf_size - pkt_size);//wr bc 808
    pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 9f 0

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

/*
 * mt6620_pwrdown - Wholechip FM Power down: Digital Modem Power Down
 * @buf - target buf
 * @buf_size - buffer size
 * return package size
 */
fm_s32 mt6620_pwrdown(fm_u8 *buf, fm_s32 buf_size)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_ENABLE_OPCODE;
    pkt_size = 4;

    //Digital Modem Power Down
    pkt_size += fm_bop_write(0x63, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 63 0
    pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);// clear 0x6e[1:0], round1
    pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);// clear 0x6e[1:0], round2
    pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);// clear 0x6e[1:0], round3
    pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);// clear 0x6e[1:0], round4
    //ADPLL Power Off Sequence
    pkt_size += fm_bop_write(0x2A, 0x0022, &buf[pkt_size], buf_size - pkt_size);//wr 2a 22
    pkt_size += fm_bop_write(0x1E, 0x0860, &buf[pkt_size], buf_size - pkt_size);//wr 1E 0860
    pkt_size += fm_bop_write(0x20, 0x0720, &buf[pkt_size], buf_size - pkt_size);//wr 20 0720
    pkt_size += fm_bop_write(0x20, 0x2720, &buf[pkt_size], buf_size - pkt_size);//wr 20 2720
    //ANALOG/RF Power Off Sequence
    pkt_size += fm_bop_write(0x00, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 0 0
    pkt_size += fm_bop_write(0x01, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 1 0
    pkt_size += fm_bop_write(0x04, 0x0141, &buf[pkt_size], buf_size - pkt_size);//wr 4 0141
    pkt_size += fm_bop_write(0x09, 0x0964, &buf[pkt_size], buf_size - pkt_size);//wr 9 0964
    pkt_size += fm_bop_write(0x0C, 0x288C, &buf[pkt_size], buf_size - pkt_size);//wr c 288c
    pkt_size += fm_bop_write(0x26, 0x0004, &buf[pkt_size], buf_size - pkt_size);//wr 26 0004
    pkt_size += fm_bop_write(0x3A, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 3A 0000
    pkt_size += fm_bop_write(0x3B, 0x00C3, &buf[pkt_size], buf_size - pkt_size);//wr 3B 00C3
    pkt_size += fm_bop_write(0x3E, 0x3280, &buf[pkt_size], buf_size - pkt_size);//wr 3E 3280
    pkt_size += fm_bop_write(0x3F, 0x4E16, &buf[pkt_size], buf_size - pkt_size);//wr 3F 4E16
    pkt_size += fm_bop_write(0x41, 0x0004, &buf[pkt_size], buf_size - pkt_size);//wr 41 0004
    //clear TX settings
    pkt_size += fm_bop_write(0x12, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 12 0000
    pkt_size += fm_bop_write(0x47, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 47 0000

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

/*
 * mt6620_rampdown - f/w will wait for STC_DONE interrupt
 * @buf - target buf
 * @buf_size - buffer size
 * return package size
 */
fm_s32 mt6620_rampdown(fm_u8 *buf, fm_s32 buf_size)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_RAMPDOWN_OPCODE;
    pkt_size = 4;

    pkt_size += fm_bop_modify(0x9C, 0xFF87, 0x0068, &buf[pkt_size], buf_size - pkt_size);//wr 9c[3] = 1, ramp down
    pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 9f 1
    pkt_size += fm_bop_write(0xC8, 0x0101, &buf[pkt_size], buf_size - pkt_size);//wr c8 101
    pkt_size += fm_bop_write(0xDD, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr dd 0
    pkt_size += fm_bop_write(0xD8, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr d8 0
    pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 9f 0
    pkt_size += fm_bop_udelay(35000, &buf[pkt_size], buf_size - pkt_size);//delay 35000
    pkt_size += fm_bop_modify(0x63, 0xFFF0, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 63[3:0] = 0, ramp down
    pkt_size += fm_bop_rd_until(0x6f, 0x0001, 0x0000, &buf[pkt_size], buf_size - pkt_size);//Poll 6f[0] = b'0

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

/*
 * mt6620_tune - execute tune action,
 * @buf - target buf
 * @buf_size - buffer size
 * @freq - 760 ~ 1080, 100KHz unit
 * return package size
 */
fm_s32 mt6620_tune_1(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    freq = (freq - 640) * 2;

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_TUNE_OPCODE;
    pkt_size = 4;

    pkt_size += fm_bop_modify(0x0F, 0xFC00, freq, &buf[pkt_size], buf_size - pkt_size);// set 0x0F[9:0] = 0x029e, => ((97.5 - 64) * 20)
    pkt_size += fm_bop_modify(0x63, 0xFFFF, 0x0001, &buf[pkt_size], buf_size - pkt_size);// set 0x63[0] = 1

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_tune_2(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    freq = (freq - 640) * 2;

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_TUNE_OPCODE;
    pkt_size = 4;

    pkt_size += fm_bop_modify(0x9C, 0xFFF7, 0x0000, &buf[pkt_size], buf_size - pkt_size);// set 0x9C[3] = 0

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_tune_3(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    freq = (freq - 640) * 2;

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_TUNE_OPCODE;
    pkt_size = 4;

    pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 9F 1
    pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);//wr C8 232
    pkt_size += fm_bop_write(0xDD, 0x8833, &buf[pkt_size], buf_size - pkt_size);//wr DD 8833
    pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);//wr D8 E8
    pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 9F 0

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

/*
 * mt6620_seek - execute seek action,
 * @buf - target buf
 * @buf_size - buffer size
 * @seekdir - 0=seek up, 1=seek down
 * @space - step, 50KHz:001, 100KHz:010, 200KHz:100
 * @max_freq - upper bound
 * @min_freq - lower bound
 * return package size
 */
fm_s32 mt6620_seek_1(fm_u8 *buf, fm_s32 buf_size, fm_u16 seekdir, fm_u16 space, fm_u16 max_freq, fm_u16 min_freq)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_SEEK_OPCODE;
    pkt_size = 4;

    //A1 Program seek direction, 0x66[10]: 0=seek up, 1=seek down
    if (seekdir == 0) {
        pkt_size += fm_bop_modify(0x66, 0xFBFF, 0x0000, &buf[pkt_size], buf_size - pkt_size);//0x66[10] = 0, seek up
    } else {
        pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0400, &buf[pkt_size], buf_size - pkt_size);//0x66[10] = 1, seek down
    }

    //0x66[11] 0=no wrarp, 1=wrap
    pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0800, &buf[pkt_size], buf_size - pkt_size);//0x66[11] = 1, wrap
    //A2 Program scan channel spacing, 0x66[14:12] step 50KHz:001/100KHz:010/200KHz:100
    if (space == 4) {
        pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x4000, &buf[pkt_size], buf_size - pkt_size);//clear 0x66[14:12] then 0x66[14:12]=001
    } else {
        pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x2000, &buf[pkt_size], buf_size - pkt_size);//clear 0x66[14:12] then 0x66[14:12]=010
    }

    //0x66[9:0] freq upper bound
    max_freq = (max_freq - 640) * 2;
    pkt_size += fm_bop_modify(0x66, 0xFC00, max_freq, &buf[pkt_size], buf_size - pkt_size);

    //0x67[9:0] freq lower bound
    min_freq = (min_freq - 640) * 2;
    pkt_size += fm_bop_modify(0x67, 0xFC00, min_freq, &buf[pkt_size], buf_size - pkt_size);
    //A3 Enable hardware controlled seeking sequence
    pkt_size += fm_bop_modify(0x63, 0xFFFF, 0x0002, &buf[pkt_size], buf_size - pkt_size);//0x63[1] = 1

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_seek_2(fm_u8 *buf, fm_s32 buf_size, fm_u16 seekdir, fm_u16 space, fm_u16 max_freq, fm_u16 min_freq)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_SEEK_OPCODE;
    pkt_size = 4;

    //A10 Set softmute to normal mode
    pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 9F 1
    pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);//wr C8 232
    pkt_size += fm_bop_write(0xDD, 0x8833, &buf[pkt_size], buf_size - pkt_size);//wr DD 8833
    pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);//wr D8 E8
    pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 9F 0

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

/*
 * mt6620_scan - execute scan action,
 * @buf - target buf
 * @buf_size - buffer size
 * @scandir - 0=seek up, 1=seek down
 * @space - step, 50KHz:001, 100KHz:010, 200KHz:100
 * @max_freq - upper bound
 * @min_freq - lower bound
 * return package size
 */
fm_s32 mt6620_scan_1(fm_u8 *buf, fm_s32 buf_size, fm_u16 scandir, fm_u16 space, fm_u16 max_freq, fm_u16 min_freq)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_SCAN_OPCODE;
    pkt_size = 4;

    //A1 Program seek direction, 0x66[10]: 0=seek up, 1=seek down
    if (scandir == 0) {
        pkt_size += fm_bop_modify(0x66, 0xFBFF, 0x0000, &buf[pkt_size], buf_size - pkt_size);//0x66[10] = 0, seek up
    } else {
        pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0400, &buf[pkt_size], buf_size - pkt_size);//0x66[10] = 1, seek down
    }

    //0x66[11] 0=no wrarp, 1=wrap
    pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0800, &buf[pkt_size], buf_size - pkt_size);//0x66[11] = 1, wrap
    //A2 Program scan channel spacing, 0x66[14:12] step 50KHz:001/100KHz:010/200KHz:100
    if (space == 4) {
        pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x4000, &buf[pkt_size], buf_size - pkt_size);//clear 0x66[14:12] then 0x66[14:12]=001
    } else {
        pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x2000, &buf[pkt_size], buf_size - pkt_size);//clear 0x66[14:12] then 0x66[14:12]=010
    }

    //0x66[9:0] freq upper bound
    max_freq = (max_freq - 640) * 2;
    pkt_size += fm_bop_modify(0x66, 0xFC00, max_freq, &buf[pkt_size], buf_size - pkt_size);

    //0x67[9:0] freq lower bound
    min_freq = (min_freq - 640) * 2;
    pkt_size += fm_bop_modify(0x67, 0xFC00, min_freq, &buf[pkt_size], buf_size - pkt_size);
    //A3 Enable hardware controlled seeking sequence
    pkt_size += fm_bop_modify(0x63, 0xFFFF, 0x0004, &buf[pkt_size], buf_size - pkt_size);//0x63[1] = 1

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_scan_2(fm_u8 *buf, fm_s32 buf_size, fm_u16 scandir, fm_u16 space, fm_u16 max_freq, fm_u16 min_freq)
{
    fm_s32 pkt_size = 0;

    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FM_SCAN_OPCODE;
    pkt_size = 4;

    //A10 Set softmute to normal mode
    pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);//wr 9F 1
    pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);//wr C8 232
    pkt_size += fm_bop_write(0xDD, 0x8833, &buf[pkt_size], buf_size - pkt_size);//wr DD 8833
    pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);//wr D8 E8
    pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);//wr 9F 0

    buf[2] = (fm_u8)((pkt_size - 4) & 0x00FF);
    buf[3] = (fm_u8)(((pkt_size - 4) >> 8) & 0x00FF);

    return pkt_size;
}

fm_s32 mt6620_get_reg(fm_u8 *buf, fm_s32 buf_size, fm_u8 addr)
{
    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FSPI_READ_OPCODE;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = addr;

    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4]);
    return 5;
}

fm_s32 mt6620_set_reg(fm_u8 *buf, fm_s32 buf_size, fm_u8 addr, fm_u16 value)
{
    if (buf_size < TX_BUF_SIZE) {
        return (-1);
    }

    buf[0] = FM_TASK_COMMAND_PKT_TYPE;
    buf[1] = FSPI_WRITE_OPCODE;
    buf[2] = 0x03;
    buf[3] = 0x00;
    buf[4] = addr;
    buf[5] = (fm_u8)((value) & 0x00FF);
    buf[6] = (fm_u8)((value >> 8) & 0x00FF);

    WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
    return 7;
}

