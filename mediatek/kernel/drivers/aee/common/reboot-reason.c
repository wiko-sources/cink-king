#include <linux/sched.h>
#include <linux/aee.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#define RR_PROC_NAME "reboot-reason"

static struct proc_dir_entry *aee_rr_file;

struct last_reboot_reason aee_rr_last_rec;

#define WDT_NORMAL_BOOT 0
#define WDT_HW_REBOOT 1
#define WDT_SW_REBOOT 2

#ifdef CONFIG_LOCAL_WDT
enum wk_wdt_type {
        WK_WDT_LOC_TYPE,
        WK_WDT_EXT_TYPE
};
extern void mpcore_wk_wdt_stop(void);
extern void mpcore_wdt_restart(enum wk_wdt_type type);
extern void mtk_wdt_restart(enum wk_wdt_type type);
#else
extern void mtk_wdt_restart(void);
#endif

void aee_rr_last(struct last_reboot_reason *lrr)
{
	memcpy(&aee_rr_last_rec, lrr, sizeof(struct last_reboot_reason));
}

static ssize_t aee_rr_read_reboot_reason(char *page, char **start, off_t off, 
					 int count,int *eof, void *data)
{
	return sprintf(page, "WDT status: %d\nshutdown mode: %u\n"
		       "current-jiffies: %lu ms\n"
		       "external-wdk-kick: %lu ms\n"
		       "idle(last-enter-jiffies, count): %lu ms, %lu\n"
		       "last irq enter  [%d, %llu], [%d, %llu]\n" 
		       "last irq exit   [%d, %llu], [%d, %llu]\n" 
		       "last sched      [%llu:%s], [%llu:%s]\n",
		       aee_rr_last_rec.wdt_status, aee_rr_last_rec.shutdown_mode,
		       (unsigned long) jiffies_to_msecs(aee_rr_last_rec.jiffies_current - INITIAL_JIFFIES),
		       (unsigned long) jiffies_to_msecs(aee_rr_last_rec.jiffies_wdk_kick - INITIAL_JIFFIES), 
		       (unsigned long) jiffies_to_msecs(aee_rr_last_rec.jiffies_idle - INITIAL_JIFFIES), (unsigned long) aee_rr_last_rec.in_idle,
		       aee_rr_last_rec.last_irq_enter[0], aee_rr_last_rec.jiffies_last_irq_enter[0], aee_rr_last_rec.last_irq_enter[1], aee_rr_last_rec.jiffies_last_irq_enter[1],
		       aee_rr_last_rec.last_irq_exit[0], aee_rr_last_rec.jiffies_last_irq_exit[0], aee_rr_last_rec.last_irq_exit[1], aee_rr_last_rec.jiffies_last_irq_exit[1],
		       aee_rr_last_rec.jiffies_last_sched[0], aee_rr_last_rec.last_sched_comm[0], aee_rr_last_rec.jiffies_last_sched[1], aee_rr_last_rec.last_sched_comm[1]) + 1;
}

void aee_rr_proc_init(struct proc_dir_entry *aed_proc_dir)
{
	aee_rr_file = create_proc_read_entry(RR_PROC_NAME, 
					     0444, aed_proc_dir, 
					     aee_rr_read_reboot_reason,
					     NULL);
	if (aee_rr_file == NULL) {
		printk(KERN_ERR "%s: Can't create rr proc entry\n", __func__);
	}
}
EXPORT_SYMBOL(aee_rr_proc_init);

void aee_rr_proc_done(struct proc_dir_entry *aed_proc_dir)
{
	remove_proc_entry(RR_PROC_NAME, aed_proc_dir);
}
EXPORT_SYMBOL(aee_rr_proc_done);

static char buffer_print_stack[4096];
extern int get_memory_size(void);
static int dump_stack_top(char *buf, int buf_len, 
                            unsigned long bottom,
		                    unsigned long top)
{
    int count = 0;
    unsigned long p;

    int len = 0;
    /*should check stack address in kernel range*/
    if (bottom & 3) {
        len += snprintf(buf, buf_len, "%s bottom unaligned %08lx \n", __func__, bottom);
        printk("bottom unaligned\n");
        return 0;
    }
    if (!((bottom >= (PAGE_OFFSET + THREAD_SIZE)) && 
                (bottom <= (PAGE_OFFSET + get_memory_size())))
       ){
        len += snprintf(buf + len, buf_len -len, "%s bottom out of kernel addr space %08lx \n", __func__, bottom);
        printk("bottom out of kernel addr space\n");
        return 0;
    }

    if (!((top >= (PAGE_OFFSET + THREAD_SIZE)) && 
                (top <= (PAGE_OFFSET + get_memory_size())))
       ){
        len += snprintf(buf + len, buf_len - len, "%s top out of kernel addr space %08lx \n", __func__, top);
        printk("top out of kerner addr space\n");
        return 0;
    }

    len += snprintf(buf + len, buf_len - len, "stack (0x%08lx to 0x%08lx)\n",bottom, top);

    for (p = bottom; p < top; p += 4) {
        unsigned long val;
        if(count == 0)
        {
            len += snprintf(buf + len, buf_len - len, "%04lx: ", p & 0xffff);
        }
        val = *((unsigned long *)(p));
        len += snprintf(buf + len, buf_len - len, "%08lx ",val);
        
        count++;
        if(count == 8)
        {
            len += snprintf(buf + len, buf_len - len, "\n");
            count = 0;
        }
    }

    return len;
}
extern void aee_sram_fiq_log(const char *msg);
void aee_stop_nested_panic(struct pt_regs *regs)
{
	struct thread_info *thread = current_thread_info();
    int i = 0;
    int len = 0;
    int timeout = 1000000;
#ifdef CONFIG_LOCAL_WDT
	/* stop the local WDT and restart the global WDT */
	mpcore_wk_wdt_stop();
    mtk_wdt_restart(WK_WDT_EXT_TYPE);
#else
	/* restart the WDT */
    mtk_wdt_restart();
#endif

	local_irq_disable();
	preempt_disable();

	aee_sram_printk("Nested panic\n");

	aee_sram_printk("Log for the previous panic:\n");
	aee_sram_printk("pc: %08lx lr: %08lx psr: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_pc, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_lr, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_cpsr);
	aee_sram_printk("sp: %08lx ip: %08lx fp: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_sp, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_ip, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_fp);
	aee_sram_printk("r10: %08lx r9: %08lx r8: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_r10, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r9, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r8);
	aee_sram_printk("r7: %08lx r6: %08lx r5: %08lx r4: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_r7, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r6, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r5, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r4);
	aee_sram_printk("r3: %08lx r2: %08lx r1: %08lx r0: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_r3, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r2, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r1, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r0);

	aee_sram_printk("Log for the current panic:\n");
	aee_sram_printk("pc: %08lx lr: %08lx psr: %08lx\n",
			regs->ARM_pc, 
			regs->ARM_lr, 
			regs->ARM_cpsr);
	aee_sram_printk("sp: %08lx ip: %08lx fp: %08lx\n",
			regs->ARM_sp, 
			regs->ARM_ip, 
			regs->ARM_fp);
	aee_sram_printk("r10: %08lx r9: %08lx r8: %08lx\n",
			regs->ARM_r10, 
			regs->ARM_r9, 
			regs->ARM_r8);
	aee_sram_printk("r7: %08lx r6: %08lx r5: %08lx r4: %08lx\n",
			regs->ARM_r7, 
			regs->ARM_r6, 
			regs->ARM_r5, 
			regs->ARM_r4);
	aee_sram_printk("r3: %08lx r2: %08lx r1: %08lx r0: %08lx\n",
			regs->ARM_r3, 
			regs->ARM_r2, 
			regs->ARM_r1, 
			regs->ARM_r0);

	printk("Nested panic\n");
	/* print mem around reg may caused nested panic again. 
       When unfortunately nested panic happens, we just want
       to know the registers and stack, then returns.
       we don't wanna add another nested panic in this function
     */
    memset(buffer_print_stack, sizeof buffer_print_stack, 0);
    
    /*Dump first panic stack*/
    len = dump_stack_top(buffer_print_stack, 
                        sizeof(buffer_print_stack), 
                         ((struct pt_regs *)thread->regs_on_excp)->ARM_sp,
                        ((struct pt_regs *)thread->regs_on_excp)->ARM_sp + 512
                         );
    
    /*Dump second panic stack*/
    len +=  dump_stack_top(buffer_print_stack + len, 
                            sizeof(buffer_print_stack) - len, 
                            regs->ARM_sp, 
                            regs->ARM_sp + 256);

    aee_sram_fiq_log(buffer_print_stack);
    printk("%d stack bytes\n", len);
    /* waiting for the WDT timeout */
	while (1)
    {
        printk("%s hang here%d\t", __func__, i++);
        while(timeout--)
        {
            udelay(1);
        }
        timeout = 1000000;
    }

}


