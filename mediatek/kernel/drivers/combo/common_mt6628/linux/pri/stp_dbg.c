#include <linux/kernel.h> // GFP_KERNEL
#include <linux/timer.h>  //init_timer, add_time, del_timer_sync
#include <linux/time.h>  //gettimeofday
#include <linux/delay.h>
#include <linux/slab.h> //kzalloc
#include <linux/sched.h> //task's status
#include <linux/vmalloc.h>
#include <linux/err.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>

#include <net/sock.h>
#include <net/netlink.h>
#include <linux/skbuff.h>
#include <net/genetlink.h>

#include "stp_dbg.h"
//#include "stp_btm.h"
#include "btm_core.h"

#define PFX_STP_DBG                      "[STPDbg]"
#define STP_DBG_LOG_LOUD                 4
#define STP_DBG_LOG_DBG                  3
#define STP_DBG_LOG_INFO                 2
#define STP_DBG_LOG_WARN                 1
#define STP_DBG_LOG_ERR                  0

unsigned int gStpDbgDbgLevel = STP_DBG_LOG_INFO;
unsigned int gStpDbgLogOut = 0;

#define STP_DBG_LOUD_FUNC(fmt, arg...)   if(gStpDbgDbgLevel >= STP_DBG_LOG_LOUD){  printk(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_DBG_FUNC(fmt, arg...)    if(gStpDbgDbgLevel >= STP_DBG_LOG_DBG){  printk(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_INFO_FUNC(fmt, arg...)   if(gStpDbgDbgLevel >= STP_DBG_LOG_INFO){ printk(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_WARN_FUNC(fmt, arg...)   if(gStpDbgDbgLevel >= STP_DBG_LOG_WARN){ printk(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_ERR_FUNC(fmt, arg...)    if(gStpDbgDbgLevel >= STP_DBG_LOG_ERR){  printk(PFX_STP_DBG "%s: "   fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_TRC_FUNC(f)              if(gStpDbgDbgLevel >= STP_DBG_LOG_DBG){  printk(PFX_STP_DBG "<%s> <%d>\n", __FUNCTION__, __LINE__);}


MTKSTP_DBG_T *g_stp_dbg = NULL;



#define STP_DBG_FAMILY_NAME        "STP_DBG"
#define MAX_BIND_PROCESS    (4)

enum {
    __STP_DBG_ATTR_INVALID,
    STP_DBG_ATTR_MSG,
    __STP_DBG_ATTR_MAX,
};
#define STP_DBG_ATTR_MAX       (__STP_DBG_ATTR_MAX - 1)

enum {
    __STP_DBG_COMMAND_INVALID,
    STP_DBG_COMMAND_BIND,
    STP_DBG_COMMAND_RESET,
    __STP_DBG_COMMAND_MAX,
};
#define MTK_WIFI_COMMAND_MAX    (__STP_DBG_COMMAND_MAX - 1)

static struct genl_family stp_dbg_gnl_family = {
    .id         = GENL_ID_GENERATE,
    .hdrsize    = 0,
    .name       = STP_DBG_FAMILY_NAME,
    .version    = 1,
    .maxattr    = STP_DBG_ATTR_MAX,
};

static void stp_dbg_nl_init(void);
static void stp_dbg_nl_deinit(void);
static int stp_dbg_nl_bind(
    struct sk_buff *skb,
    struct genl_info *info
    );
static int stp_dbg_nl_reset(
    struct sk_buff *skb,
    struct genl_info *info
    );

/* attribute policy */
static struct nla_policy stp_dbg_genl_policy[STP_DBG_ATTR_MAX + 1] = {
    [STP_DBG_ATTR_MSG] = { .type = NLA_NUL_STRING },
};

/* operation definition */
static struct genl_ops stp_dbg_gnl_ops_bind = {
    .cmd = STP_DBG_COMMAND_BIND,
    .flags  = 0,
    .policy = stp_dbg_genl_policy,
    .doit   = stp_dbg_nl_bind,
    .dumpit = NULL,
};

static struct genl_ops stp_dbg_gnl_ops_reset = {
    .cmd = STP_DBG_COMMAND_RESET,
    .flags  = 0,
    .policy = stp_dbg_genl_policy,
    .doit   = stp_dbg_nl_reset,
    .dumpit = NULL,
};

static unsigned int stp_dbg_seqnum = 0;
static int num_bind_process = 0;
static pid_t bind_pid[MAX_BIND_PROCESS];

static void stp_dbg_dump_data(unsigned char *pBuf, char * title,  int len)
{
    int k = 0;
    printk(" %s-len:%d\n", title, len);
    //printk("    ", title, len);
    for(k=0; k < len ; k++){
        if(k%16 == 0 && k != 0)  printk("\n    ");
        printk("0x%02x ",  pBuf[k]);
    }
    printk("--end\n");
}


static int _stp_dbg_enable(MTKSTP_DBG_T *stp_dbg){

    unsigned long flags;

    spin_lock_irqsave(&(stp_dbg->logsys->lock), flags);
    stp_dbg->pkt_trace_no=0;
    stp_dbg->is_enable=1;
    spin_unlock_irqrestore(&(stp_dbg->logsys->lock), flags);

    return 0;
}

static int _stp_dbg_disable(MTKSTP_DBG_T *stp_dbg){

    unsigned long flags;

    spin_lock_irqsave(&(stp_dbg->logsys->lock), flags);
    stp_dbg->pkt_trace_no=0;
    memset(stp_dbg->logsys, 0, sizeof(MTKSTP_LOG_SYS_T));
    stp_dbg->is_enable=0;
    spin_unlock_irqrestore(&(stp_dbg->logsys->lock), flags);

    return 0;
}

static int _stp_dbg_dmp_in(MTKSTP_DBG_T *stp_dbg, char *buf, int len){

    unsigned long flags;
    unsigned int internalFlag = stp_dbg->logsys->size < STP_DBG_LOG_ENTRY_NUM;
    //#ifdef CONFIG_LOG_STP_INTERNAL   
    //Here we record log in this circle buffer, if buffer is full , select to overlap earlier log, logic should be okay
        internalFlag = 1;
    //#endif
    spin_lock_irqsave(&(stp_dbg->logsys->lock), flags);

    if(internalFlag){
        stp_dbg->logsys->queue[stp_dbg->logsys->in].id = 0;
        stp_dbg->logsys->queue[stp_dbg->logsys->in].len = len;
        memcpy(&(stp_dbg->logsys->queue[stp_dbg->logsys->in].buffer[0]),
            buf, ((len >= STP_DBG_LOG_ENTRY_SZ)? (STP_DBG_LOG_ENTRY_SZ):(len)));

        stp_dbg->logsys->size++;
        stp_dbg->logsys->size = (stp_dbg->logsys->size > STP_DBG_LOG_ENTRY_NUM) ? STP_DBG_LOG_ENTRY_NUM : stp_dbg->logsys->size;
        
        if (0 != gStpDbgLogOut)
        {
            STP_DBG_HDR_T *pHdr = NULL;
            char* pBuf = NULL;
            unsigned int len =0;
            pHdr = (STP_DBG_HDR_T *)&(stp_dbg->logsys->queue[stp_dbg->logsys->in].buffer[0]);
            pBuf = (char *)&(stp_dbg->logsys->queue[stp_dbg->logsys->in].buffer[0]) + sizeof (STP_DBG_HDR_T);
            len = stp_dbg->logsys->queue[stp_dbg->logsys->in].len - sizeof (STP_DBG_HDR_T);
            printk("STP-DBG:%d.%ds, %s:pT%sn(%d)l(%d)s(%d)a(%d)\n", \
                pHdr->sec,
                pHdr->usec,
                pHdr->dir == PKT_DIR_TX ? "Tx" : "Rx",
                gStpDbgType[pHdr->type],
                pHdr->no,
                pHdr->len,
                pHdr->seq,
                pHdr->ack

            );
            if (0 < len){
                stp_dbg_dump_data(pBuf, pHdr->dir == PKT_DIR_TX ? "Tx" : "Rx", len);
            }
            
        }
        stp_dbg->logsys->in = (stp_dbg->logsys->in >= (STP_DBG_LOG_ENTRY_NUM - 1))?(0):(stp_dbg->logsys->in + 1);
        STP_DBG_DBG_FUNC("logsys size = %d, in = %d\n", stp_dbg->logsys->size, stp_dbg->logsys->in);
    } else {
        STP_DBG_WARN_FUNC("logsys FULL!\n");
    }

    spin_unlock_irqrestore(&(stp_dbg->logsys->lock), flags);

    return 0;
}

int stp_gdb_notify_btm_dmp_wq(MTKSTP_DBG_T *stp_dbg){
    int retval = 0;    
//#ifndef CONFIG_LOG_STP_INTERNAL

    if(stp_dbg->btm != NULL){
        retval += stp_btm_notify_wmt_dmp_wq((MTKSTP_BTM_T *)stp_dbg->btm);
    }
    
//#endif

    return retval;
}

int stp_dbg_log_ctrl (unsigned int on)
{
    if (on != 0)    
    {
        gStpDbgLogOut = 1;
        printk ("STP-DBG: enable pkt log dump out.\n");
    }
    else
    {
        gStpDbgLogOut = 0;
        printk ("STP-DBG: disable pkt log dump out.\n");
    }
    return 0;
}

int stp_dbg_dmp_in(MTKSTP_DBG_T *stp_dbg, char *buf, int len){
    return _stp_dbg_dmp_in(stp_dbg, buf, len);
}


int stp_dbg_dmp_printk(MTKSTP_DBG_T *stp_dbg)
{
    #define MAX_DMP_NUM 80
    unsigned long flags;
    char *pBuf = NULL;
    int len = 0;
    STP_DBG_HDR_T *pHdr = NULL;
    UINT32 dumpSize = 0;
    UINT32 inIndex = 0;
    UINT32 outIndex = 0;
    spin_lock_irqsave(&(stp_dbg->logsys->lock), flags);
    //Not to dequeue from loging system
    inIndex = stp_dbg->logsys->in;
    dumpSize = stp_dbg->logsys->size;
    if (STP_DBG_LOG_ENTRY_NUM == dumpSize)
    {
        outIndex = inIndex;
    }
    else
    {
        outIndex = ((inIndex + STP_DBG_LOG_ENTRY_NUM) - dumpSize) % STP_DBG_LOG_ENTRY_NUM;
    }
    
    STP_DBG_INFO_FUNC("loged packet size = %d, in(%d), out(%d)\n", dumpSize, inIndex, outIndex);
    if (dumpSize > MAX_DMP_NUM)
    {
        
        outIndex += (dumpSize - MAX_DMP_NUM);
        outIndex %= STP_DBG_LOG_ENTRY_NUM;
        dumpSize = MAX_DMP_NUM;
        
    }
    while(dumpSize > 0){
        pHdr = (STP_DBG_HDR_T *)&(stp_dbg->logsys->queue[outIndex].buffer[0]);
        pBuf = &(stp_dbg->logsys->queue[outIndex].buffer[0]) + sizeof (STP_DBG_HDR_T);
        len = stp_dbg->logsys->queue[outIndex].len - sizeof (STP_DBG_HDR_T);
        len = len > STP_PKT_SZ ? STP_PKT_SZ : len;
        printk("STP-DBG:%d.%ds, %s:pT%sn(%d)l(%d)s(%d)a(%d)\n", \
            pHdr->sec,
            pHdr->usec,
            pHdr->dir == PKT_DIR_TX ? "Tx" : "Rx",
            gStpDbgType[pHdr->type],
            pHdr->no,
            pHdr->len,
            pHdr->seq,
            pHdr->ack
        );
        
        
        if (0 < len){
            stp_dbg_dump_data(pBuf, pHdr->dir == PKT_DIR_TX ? "Tx" : "Rx", len);
        }
        outIndex = (outIndex >= (STP_DBG_LOG_ENTRY_NUM - 1))?(0):(outIndex + 1);
        dumpSize--;
          
    } 

      
    spin_unlock_irqrestore(&(stp_dbg->logsys->lock), flags);

    return 0;
}

int stp_dbg_dmp_out_ex (char *buf, int *len){
    return stp_dbg_dmp_out(g_stp_dbg, buf, len);
}

int stp_dbg_dmp_out(MTKSTP_DBG_T *stp_dbg, char *buf, int *len){

    unsigned long flags;
    int remaining = 0;
    *len = 0;
    spin_lock_irqsave(&(stp_dbg->logsys->lock), flags);

    if(stp_dbg->logsys->size > 0){
        memcpy(buf, &(stp_dbg->logsys->queue[stp_dbg->logsys->out].buffer[0]),
             stp_dbg->logsys->queue[stp_dbg->logsys->out].len);

        (*len) = stp_dbg->logsys->queue[stp_dbg->logsys->out].len;
        stp_dbg->logsys->out = (stp_dbg->logsys->out >= (STP_DBG_LOG_ENTRY_NUM - 1))?(0):(stp_dbg->logsys->out + 1);
        stp_dbg->logsys->size--;

        STP_DBG_DBG_FUNC("logsys size = %d, out = %d\n", stp_dbg->logsys->size, stp_dbg->logsys->out);
    } else {
        STP_DBG_LOUD_FUNC("logsys EMPTY!\n");
    }

    remaining = (stp_dbg->logsys->size == 0)?(0):(1);

    spin_unlock_irqrestore(&(stp_dbg->logsys->lock), flags);

    return remaining;
}

static int stp_dbg_fill_hdr(struct stp_dbg_pkt_hdr *hdr, int type, int ack, int seq, int crc, int dir, int len, int dbg_type){

    struct timeval now;

    if(!hdr){
        STP_DBG_ERR_FUNC("function invalid\n");
        return -EINVAL;
    } else {
        do_gettimeofday(&now);
        hdr->dbg_type = dbg_type;
        hdr->ack = ack;
        hdr->seq = seq;
        hdr->sec = now.tv_sec;
        hdr->usec = now.tv_usec;
        hdr->crc  = crc;
        hdr->dir  = dir;//rx
        hdr->dmy  = 0xffffffff;
        hdr->len  =  len;
        hdr->type = type;
        return 0;
    }
}

static int
stp_dbg_add_pkt (
    MTKSTP_DBG_T*stp_dbg,
    struct stp_dbg_pkt_hdr *hdr,
    const unsigned char *body
    )
{
     //fix the frame size large issues.
    static struct stp_dbg_pkt stp_pkt;
    uint32_t hdr_sz = sizeof(struct stp_dbg_pkt_hdr);
    uint32_t body_sz = 0;

    BUG_ON(!stp_dbg);

    if(hdr->dbg_type == STP_DBG_PKT){
        body_sz = (hdr->len <= STP_PKT_SZ)?(hdr->len):(STP_PKT_SZ);
    }else{
        body_sz = (hdr->len <= STP_DMP_SZ)?(hdr->len):(STP_DMP_SZ);
    }

    hdr->no = stp_dbg->pkt_trace_no++;
    memcpy((uint8_t *)&stp_pkt.hdr, (uint8_t*)hdr, hdr_sz);
    if(body != NULL){
        memcpy((uint8_t *)&stp_pkt.raw[0], body, body_sz);
    }
    _stp_dbg_dmp_in(stp_dbg, (char *) &stp_pkt, hdr_sz + body_sz);
    //Only FW DMP MSG should inform BTM-CORE to dump packet to native process
    if(hdr->dbg_type == STP_DBG_FW_DMP)
    {
        stp_gdb_notify_btm_dmp_wq(stp_dbg);
    }

    return 0;
}

int stp_dbg_log_pkt(MTKSTP_DBG_T *stp_dbg, int dbg_type,
    int type, int ack_no, int seq_no, int crc, int dir, int len, const unsigned char *body) {

    struct stp_dbg_pkt_hdr hdr;

    if (stp_dbg->is_enable == 0) {
        /*dbg is disable,and not to log*/
    }
    else {
        stp_dbg_fill_hdr(&hdr,
            (int) type,
            (int) ack_no,
            (int) seq_no,
            (int) crc,
            (int) dir,
            (int) len,
            (int) dbg_type);

        stp_dbg_add_pkt(stp_dbg, &hdr, body);
    }

    return 0;
}

int stp_dbg_enable(MTKSTP_DBG_T *stp_dbg){
    return _stp_dbg_enable(stp_dbg);
}

int stp_dbg_disable(MTKSTP_DBG_T *stp_dbg){
    return _stp_dbg_disable(stp_dbg);
}

static void stp_dbg_nl_init(void)
{
    if(genl_register_family(&stp_dbg_gnl_family) != 0) 
    {
       STP_DBG_ERR_FUNC("%s(): GE_NELINK family registration fail\n", __func__);
    }
    else 
    {
        if(genl_register_ops(&stp_dbg_gnl_family, &stp_dbg_gnl_ops_bind) != 0) 
        {
           STP_DBG_ERR_FUNC("%s(): BIND operation registration fail\n", __func__);
        }

        if(genl_register_ops(&stp_dbg_gnl_family, &stp_dbg_gnl_ops_reset) != 0) 
        {
           STP_DBG_ERR_FUNC("%s(): RESET operation registration fail\n", __func__);
        }
    }

    return;
}

static void stp_dbg_nl_deinit(void)
{
    genl_unregister_family(&stp_dbg_gnl_family);
    return;
}

static int stp_dbg_nl_bind(
    struct sk_buff *skb,
    struct genl_info *info
    )
{
    struct nlattr *na;
    char * mydata;

    if (info == NULL) 
    {
        goto out;
    }

    STP_DBG_INFO_FUNC("%s():->\n", __func__);
    
    na = info->attrs[STP_DBG_ATTR_MSG];
    
    if (na) 
    {
        mydata = (char *)nla_data(na);
    }

    if(num_bind_process < MAX_BIND_PROCESS) 
    {
        bind_pid[num_bind_process] = info->snd_pid;
        num_bind_process++;
        STP_DBG_INFO_FUNC("%s():-> pid  = %d\n", __func__, info->snd_pid);
    }
    else 
    {
        STP_DBG_ERR_FUNC("%s(): exceeding binding limit %d\n", __func__, MAX_BIND_PROCESS);
    }

out:
    return 0;
}

static int stp_dbg_nl_reset(
    struct sk_buff *skb,
    struct genl_info *info
    )
{
    STP_DBG_ERR_FUNC("%s(): should not be invoked\n", __func__);

    return 0;
}

char
stp_dbg_nl_send(
    char *  aucMsg,
    unsigned char      cmd
    )
{
    struct sk_buff *skb = NULL;
    void *msg_head = NULL;
    int rc = -1;
    int i;

    if(num_bind_process == 0) 
    {
        /* no listening process */
        STP_DBG_ERR_FUNC("%s(): the process is not invoked\n", __func__);
        return 0;
    }

    for(i = 0 ; i < num_bind_process ; i++) 
    {
        skb = genlmsg_new(2048, GFP_KERNEL);

        if(skb) 
        {
            msg_head = genlmsg_put(skb, 0, stp_dbg_seqnum++, &stp_dbg_gnl_family, 0, cmd);
            if(msg_head == NULL) 
            {
                nlmsg_free(skb);
                STP_DBG_ERR_FUNC("%s(): genlmsg_put fail...\n", __func__);
                return -1;
            }

            rc = nla_put_string(skb, STP_DBG_ATTR_MSG, aucMsg);
            if(rc != 0) 
            {            
                nlmsg_free(skb);
                STP_DBG_ERR_FUNC("%s(): nla_put_string fail...\n", __func__);
                return -1;
            }
        
            /* finalize the message */
            genlmsg_end(skb, msg_head);
        
            /* sending message */
            rc = genlmsg_unicast(&init_net, skb, bind_pid[i]);
            if(rc != 0) 
            {
                STP_DBG_ERR_FUNC("%s(): genlmsg_unicast fail...\n", __func__);
                return -1;
            }
        }
        else 
        {
            STP_DBG_ERR_FUNC("%s(): genlmsg_new fail...\n", __func__);
            return -1;
        }
    }

    return 0;
}

MTKSTP_DBG_T *stp_dbg_init(void *btm_half){

    MTKSTP_DBG_T *stp_dbg= NULL;
    STP_DBG_INFO_FUNC("stp-dbg init\n");

    stp_dbg = kzalloc(sizeof(MTKSTP_DBG_T), GFP_KERNEL);
    if(IS_ERR(stp_dbg)){
        STP_DBG_ERR_FUNC("-ENOMEM\n");
        goto ERR_EXIT1;
    }

    stp_dbg->logsys = vmalloc(sizeof(MTKSTP_LOG_SYS_T));
    if(IS_ERR(stp_dbg->logsys)){
        STP_DBG_ERR_FUNC("-ENOMEM stp_gdb->logsys\n");
        goto ERR_EXIT2;
    }
    memset(stp_dbg->logsys, 0, sizeof(MTKSTP_LOG_SYS_T));
    spin_lock_init(&(stp_dbg->logsys->lock));
    stp_dbg->pkt_trace_no=0;
    stp_dbg->is_enable=0;
    g_stp_dbg = stp_dbg;

    if(btm_half != NULL){
       stp_dbg->btm = btm_half;
    } else {
       stp_dbg->btm = NULL;
    }

    //bind to netlink
    stp_dbg_nl_init();
    
    return stp_dbg;

ERR_EXIT2:
    kfree(stp_dbg);
    return NULL;

ERR_EXIT1:
    return NULL;
}

int stp_dbg_deinit(MTKSTP_DBG_T *stp_dbg){

    STP_DBG_INFO_FUNC("stp-dbg deinit\n");

    //unbind with netlink
    stp_dbg_nl_deinit();

    if(stp_dbg->logsys){
        vfree(stp_dbg->logsys);
    }

    if(stp_dbg){
        kfree(stp_dbg);
    }

    return 0;
}







