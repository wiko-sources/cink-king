#ifdef CONFIG_MT_LOAD_BALANCE_PROFILER

#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/tick.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <mtlbprof/mtlbprof.h>

#define MT_LBPROF_VERSION 3

#define MT_LBPROF_LOG_ACTION_CLOSE          0
#define MT_LBPROF_LOG_ACTION_OPEN           1
#define MT_LBPROF_LOG_ACTION_READ           2
/* Return number of unread characters in the log buffer */
#define MT_LBPROF_LOG_ACTION_SIZE_UNREAD    3

#define MT_LBPROF_STATS_SIZE				76800
#define MT_LBPROF_TASK_BUFLEN				(4*1024*1024)
#define MT_LBPROF_LBLOG_BUFLEN				(64*1024)
#define MT_LBPROF_UPDATE_BUFLEN				(4*1024)
#define MT_LBPROF_UPDATE_DEBUG_BUFLEN		(52*1024)

unsigned long mt_lbprof_flags;
static int mt_lbprof_start = 0;
unsigned long long start;
unsigned long long last_update;
static DEFINE_PER_CPU(unsigned long long, start_idle_time);

static DEFINE_SPINLOCK(mt_lbprof_stats_spinlock);
static DEFINE_SPINLOCK(mt_lbprof_log_spinlock);
DECLARE_WAIT_QUEUE_HEAD(mt_lbprof_stats_wait);
DECLARE_WAIT_QUEUE_HEAD(mt_lbprof_log_wait);

struct mt_lbprof_buf_t mt_lbprof_stats = { 0, 0, 0, 0, 0, &mt_lbprof_stats_wait, &mt_lbprof_stats_spinlock} ;
struct mt_lbprof_buf_t mt_lbprof_log = {0, 0, 0, 0, 0, &mt_lbprof_log_wait, &mt_lbprof_log_spinlock};

DEFINE_SPINLOCK(mt_lbprof_check_cpu_idle_spinlock);
EXPORT_SYMBOL(mt_lbprof_check_cpu_idle_spinlock);

static int mt_lbprof_set_flag(unsigned long flags);

/*
 * Ease the printing of nsec fields:
 */
static long long nsec_high(unsigned long long nsec)
{
	if ((long long)nsec < 0) {
		nsec = -nsec;
		do_div(nsec, 1000000);
		return -nsec;
	}
	do_div(nsec, 1000000);

	return nsec;
}

static unsigned long nsec_low(unsigned long long nsec)
{
	if ((long long)nsec < 0)
		nsec = -nsec;

	return do_div(nsec, 1000000);
}

#define SPLIT_NS(x) nsec_high(x), nsec_low(x)

void __mt_lbprof_log_printf(char *str, struct mt_lbprof_buf_t *mt_lbprof_buf)
{
	unsigned long long ts;
	char msg[MT_LBPROF_STR_SIZE+18];
	int len, log_cnt = 0;
	unsigned long irq_flags;	

	spin_lock_irqsave(mt_lbprof_buf->spinlock, irq_flags);
	if ( !mt_lbprof_buf->buf ){
		spin_unlock_irqrestore(mt_lbprof_buf->spinlock, irq_flags);
		return;
	}

	// Ftrace use sched_clock()
	ts = local_clock();
	sprintf(msg, "%Lu.%06lu %s", SPLIT_NS(ts), str);
	len = strlen(msg);
	
	while(log_cnt < len ){	
		mt_lbprof_buf->buf[mt_lbprof_buf->wt_idx] = msg[log_cnt];

		log_cnt ++ ;
		mt_lbprof_buf->wt_idx++;
		if(mt_lbprof_buf->wt_idx >= mt_lbprof_buf->len){
			mt_lbprof_buf->wt_idx = 0;
		}
	}
	spin_unlock_irqrestore(mt_lbprof_buf->spinlock, irq_flags);
}

static ssize_t mt_lbprof_flags_write(struct file *filp, const char *ubuf,
           size_t cnt, loff_t *data)
{
	char buf[MT_LBPROF_STR_SIZE];
	size_t copy_size = cnt;
	unsigned long flags;
	int status;

	if (cnt >= sizeof(buf)) {
		copy_size = MT_LBPROF_STR_SIZE - 1;
	}

	if (copy_from_user(&buf, ubuf, copy_size))
		return -EFAULT;

	if (1 != sscanf(buf, "%lu", &flags))
		return -EPERM;		/*ytchang*/

	status = mt_lbprof_set_flag(flags);
	if( status )
		return status;

	return cnt;
}

static int mt_lbprof_do_log(int type, char __user *buf, int len, struct mt_lbprof_buf_t *mt_lbprof_buf)
{
	char c;
	int error = 0, log_cnt=0;
	unsigned long irq_flags;

	switch (type) {
	case MT_LBPROF_LOG_ACTION_CLOSE:	/* Close log */
		break;
	case MT_LBPROF_LOG_ACTION_OPEN:	/* Open log */
		break;
	case MT_LBPROF_LOG_ACTION_READ:	/* Read from log */
		error = -EINVAL;
		if (!buf || len < 0  )
			goto out;
	
		error = 0;
		if (!len)
			goto out;
		if (!access_ok(VERIFY_WRITE, buf, len)) {
			error = -EFAULT;
			goto out;
		}

		error = wait_event_interruptible(*mt_lbprof_buf->log_wait,
			(mt_lbprof_buf->rd_idx - mt_lbprof_buf->wt_idx));

		if (error)
			goto out;

		spin_lock_irqsave(mt_lbprof_log.spinlock, irq_flags);
		if( !mt_lbprof_buf->buf ){
			error = -EINVAL;
			spin_unlock_irqrestore(mt_lbprof_log.spinlock, irq_flags);
			goto out;
		}

		while (!error && (mt_lbprof_buf->rd_idx != mt_lbprof_buf->wt_idx) && (log_cnt < len ) ){
			c = mt_lbprof_buf->buf[mt_lbprof_buf->rd_idx];
			mt_lbprof_buf->rd_idx++;
			if(mt_lbprof_buf->rd_idx >= mt_lbprof_buf->len)
				mt_lbprof_buf->rd_idx = 0;
			mt_lbprof_buf->cnt--;
			spin_unlock_irqrestore(mt_lbprof_log.spinlock, irq_flags);
			error = __put_user(c, &buf[log_cnt]);
			if(error)
				break;
			log_cnt++;

			cond_resched();
			spin_lock_irqsave(mt_lbprof_log.spinlock, irq_flags);
		}
		spin_unlock_irqrestore(mt_lbprof_log.spinlock, irq_flags);
						
		if (!error)
			error = log_cnt;	

		break;																	
	/* Number of chars in the log buffer */
	case MT_LBPROF_LOG_ACTION_SIZE_UNREAD:
		spin_lock_irqsave(mt_lbprof_log.spinlock, irq_flags);
		error = mt_lbprof_buf->wt_idx - mt_lbprof_buf->rd_idx;
		spin_unlock_irqrestore(mt_lbprof_log.spinlock, irq_flags);
		break;
	default:
		error = -EINVAL;
		break;
	}
out:

	return error;
}

#define MT_LBPROC_ENTRY(name) \
static int mt_##name##_show(struct seq_file *m, void *v);\
static int mt_##name##_write(struct file *filp, const char *ubuf, size_t cnt, loff_t *data);\
static unsigned int mt_##name##_poll(struct file *file, poll_table *wait); \
static int mt_##name##_open(struct inode *inode, struct file *file) \
{ \
    return single_open(file, mt_##name##_show, inode->i_private); \
} \
\
static const struct file_operations mt_##name##_fops = { \
    .open = mt_##name##_open, \
    .write = mt_##name##_write,\
    .read = seq_read, \
    .poll = mt_##name##_poll, \
    .llseek = seq_lseek, \
    .release = single_release, \
};\

/* 1. /proc/mtprof/flags*/
MT_LBPROC_ENTRY(lbprof_flags);
static int mt_lbprof_flags_show(struct seq_file *seq, void *v)
{
	seq_printf(seq,"%lu\n", mt_lbprof_flags); /*ytchang*/

	return 0;
}

static unsigned int mt_lbprof_flags_poll(struct file *file, poll_table *wait)
{
	return 0;
}

/* 2. /proc/mtprof/log*/
static int mt_lbprof_log_open(struct inode * inode, struct file * file);
static int mt_lbprof_do_log(int type, char __user *buf, int len, struct mt_lbprof_buf_t *mt_lbprof_buf);
static ssize_t mt_lbprof_log_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos);
static unsigned int mt_lbprof_log_poll(struct file *file, poll_table *wait);
static int mt_lbprof_log_release(struct inode * inode, struct file * file);
static const struct file_operations mt_lbprof_log_fops = {
	.open		= mt_lbprof_log_open,
	.read		= mt_lbprof_log_read,
	.poll		= mt_lbprof_log_poll,
	.release	= mt_lbprof_log_release,
	.llseek		= generic_file_llseek,
};

static int mt_lbprof_log_open(struct inode * inode, struct file * file)
{
	return mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_OPEN, NULL, 0, &mt_lbprof_log);	
}

static ssize_t mt_lbprof_log_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
	if ((file->f_flags & O_NONBLOCK) &&
	    !mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_SIZE_UNREAD, NULL, 0, &mt_lbprof_log))
		return -EAGAIN;		
	return mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_READ, buf, count, &mt_lbprof_log);	
}

static unsigned int mt_lbprof_log_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, mt_lbprof_log.log_wait, wait);
	if (mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_SIZE_UNREAD, NULL, 0, &mt_lbprof_log))
		return POLLIN | POLLRDNORM;
	return 0;	
}

static int mt_lbprof_log_release(struct inode * inode, struct file * file)
{

	(void) mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_CLOSE, NULL, 0, &mt_lbprof_log);
	return 0;
}

/* 3. /proc/mtprof/lb_stats*/
static int mt_lbprof_stats_open(struct inode * inode, struct file * file);
static ssize_t mt_lbprof_stats_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos);
static unsigned int mt_lbprof_stats_poll(struct file *file, poll_table *wait);
static int mt_lbprof_stats_release(struct inode * inode, struct file * file);
static const struct file_operations mt_lbprof_stats_fops = {
	.open		= mt_lbprof_stats_open,
	.read		= mt_lbprof_stats_read,
	.poll		= mt_lbprof_stats_poll,
	.release	= mt_lbprof_stats_release,
	.llseek		= generic_file_llseek,
};

static int mt_lbprof_stats_open(struct inode * inode, struct file * file)
{
	return mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_OPEN, NULL, 0, &mt_lbprof_stats);	
}

static ssize_t mt_lbprof_stats_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
	if ((file->f_flags & O_NONBLOCK) &&
	    !mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_SIZE_UNREAD, NULL, 0, &mt_lbprof_stats))
		return -EAGAIN;
	return mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_READ, buf, count, &mt_lbprof_stats);	
}

static unsigned int mt_lbprof_stats_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, mt_lbprof_stats.log_wait, wait);
	if (mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_SIZE_UNREAD, NULL, 0, &mt_lbprof_stats))
		return POLLIN | POLLRDNORM;
	return 0;	
}

static int mt_lbprof_stats_release(struct inode * inode, struct file * file)
{
	(void) mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_CLOSE, NULL, 0, &mt_lbprof_stats);
	return 0;
}

/* ---------------------------------------------------------- */
#define SPLIT_PERCENT(x) ((x)/100), ((x)%100)

DEFINE_SPINLOCK(mt_lbprof_output_spinlock);
/* ---------------------------------------------------------- */
static int mt_lbprof_malloc_buf(struct mt_lbprof_buf_t *mt_lbprof_buf, int buf_size)
{
	unsigned long irq_flags;

	if ( !buf_size )
		return -EINVAL;

	spin_lock_irqsave(mt_lbprof_buf->spinlock, irq_flags);
	mt_lbprof_buf->buf = vmalloc(buf_size);
	if ( !mt_lbprof_buf->buf ){
		spin_unlock_irqrestore(mt_lbprof_buf->spinlock, irq_flags);
		return -ENOMEM;	
	}
	mt_lbprof_buf->len = buf_size;
	mt_lbprof_buf->rd_idx = 0;
	mt_lbprof_buf->wt_idx = 0;
	spin_unlock_irqrestore(mt_lbprof_buf->spinlock, irq_flags);
	return 0;		
}

static int mt_lbprof_malloc_log_buf(void)
{
	int buf_size = 0;
	unsigned long flag = mt_lbprof_flags;
	int i=0;
	static int mt_lbprof_bufsize[5]= {0, MT_LBPROF_TASK_BUFLEN, MT_LBPROF_LBLOG_BUFLEN, MT_LBPROF_UPDATE_BUFLEN, MT_LBPROF_UPDATE_DEBUG_BUFLEN};

	while(flag){
		if( 1==(flag & 0x1) )
			buf_size += mt_lbprof_bufsize[i];
		flag >>=1;
		i++;
	};

	if ( !buf_size )
		return -EINVAL;

	return mt_lbprof_malloc_buf(&mt_lbprof_log, buf_size);
}

static void mt_lbprof_free_log_buf(void)
{
	unsigned long irq_flags;
	if ( 0 == mt_lbprof_log.buf )
		return;

	spin_lock_irqsave(mt_lbprof_log.spinlock, irq_flags);	
	mt_lbprof_log.rd_idx = 0;
	mt_lbprof_log.wt_idx = 0;	
	mt_lbprof_log.len = 0;
	vfree(mt_lbprof_log.buf);
	mt_lbprof_log.buf = NULL ;
	spin_unlock_irqrestore(mt_lbprof_log.spinlock, irq_flags);
}

static int mt_lbprof_set_flag(unsigned long flags)
{
	int status;

	if( flags == mt_lbprof_flags ) 
		return 0;

	if (flags & ~MT_LBPROF_ALLENABLE)
		return -EPERM;

	mt_lbprof_flags = flags;
	mt_lbprof_free_log_buf();
	
	status = mt_lbprof_malloc_log_buf();
	return status;
}

static int mt_lbprof_tst_flag(unsigned long flags)
{
	return flags == (mt_lbprof_flags & flags);
}

static int mt_lbprof_any_enable(unsigned long flags)
{
	return mt_lbprof_flags & flags;
}

//http://git.kernel.org/?p=linux/kernel/git/stable/linux-stable.git;a=commitdiff;h=6beea0cda8ce71c01354e688e5735c47e331e84f
static unsigned long long mtprof_get_cpu_idle(int cpu)
{
    unsigned long long *unused = 0, wall;
	unsigned long long idle_time = get_cpu_idle_time_us(cpu, unused);
	idle_time += get_cpu_iowait_time_us(cpu, &wall);
	return idle_time;	 
}
/* ---------------------------------------------------------- */
int __init mt_lbprof_proc_init(void)
{
	struct proc_dir_entry *pe;
	int status, cpu;
	unsigned long irq_flags;
	
	status = mt_lbprof_malloc_buf(&mt_lbprof_stats, MT_LBPROF_STATS_SIZE);
	if (status)
		return status;		
					
	status = mt_lbprof_set_flag(15);
	if (status)
		return status;

	pe = proc_create("mtprof/lb_flags", 0644, NULL, &mt_lbprof_flags_fops);
	if (!pe)
		return -ENOMEM;

	pe = proc_create("mtprof/lb_stats", 0444, NULL, &mt_lbprof_stats_fops);
	if (!pe)
		return -ENOMEM;

	pe = proc_create("mtprof/lb_log", 0444, NULL, &mt_lbprof_log_fops);
	if (!pe)
		return -ENOMEM;

	spin_lock_irqsave(&mt_lbprof_check_cpu_idle_spinlock, irq_flags);
	for_each_present_cpu (cpu) {
		per_cpu(start_idle_time, cpu) = mtprof_get_cpu_idle(cpu);
	}

	last_update = local_clock();
	start = last_update;
	mt_lbprof_start = 1;
	spin_unlock_irqrestore(&mt_lbprof_check_cpu_idle_spinlock, irq_flags);

	return 0;
}
late_initcall(mt_lbprof_proc_init);

/* ---------------------------------------------------------- */
#ifndef arch_idle_time
#define arch_idle_time(cpu) 0
#endif

static DEFINE_PER_CPU(int, lb_state);

static unsigned long time_slice_ns[2][3]={{0,0,0},{0,0,0}};
static unsigned long unbalance_slice_ns;
static unsigned long long period_time;
void mt_lbprof_update_state_has_lock(int cpu, int rq_cnt)
{
	int unbalance = 0;
	unsigned long long now, delta;
	int pre_state[2], post_state[2], state[2];
	int cpu1, cpu2, tmp_cpu;
	struct cpumask cpu_mask;
	unsigned long irq_flags;

	if( !mt_lbprof_start)
		return;
	
	if(per_cpu(lb_state, cpu) == rq_cnt)
		return;

	spin_lock_irqsave(&mt_lbprof_check_cpu_idle_spinlock, irq_flags);

	now = local_clock();

	delta = now - start;

	if( delta > 0 ){
		cpumask_copy(&cpu_mask, cpu_possible_mask );
		for_each_cpu(cpu1, cpu_possible_mask){
			cpumask_clear_cpu(cpu1, &cpu_mask);
			state[0] = per_cpu(lb_state, cpu1);
			if(state[0] < 5){
				for_each_cpu(cpu2, &cpu_mask){
					state[1] = per_cpu(lb_state, cpu2);
					if (state[0] < 2 && (state[1] < 5 && state[1] >= 2)){
						unbalance = 1;
						time_slice_ns[state[0]][state[1]-2] += delta;
					}else if(state[1] < 2 && state[0]>=2){
						unbalance = 1;
						time_slice_ns[state[1]][state[0]-2] += delta;
					}
				}
			}
		}
		if( unbalance )
			unbalance_slice_ns += delta;
		period_time += delta;
	}

	pre_state[0]=per_cpu(lb_state, 0);
	pre_state[1]=per_cpu(lb_state, 1);

	if(rq_cnt != MT_LBPROF_UPDATE_STATE){
		per_cpu(lb_state, cpu) = rq_cnt;
	}

	for_each_cpu(tmp_cpu, cpu_possible_mask){
		if ( tmp_cpu == cpu && rq_cnt != MT_LBPROF_UPDATE_STATE )
			continue;
		switch(cpu_rq(tmp_cpu)->nr_running){
			case 1:
				per_cpu(lb_state, tmp_cpu) = MT_LBPROF_ONE_TASK_STATE;
				break;
			case 0:
				if( (per_cpu(lb_state, tmp_cpu) == MT_LBPROF_HOTPLUG_STATE ) || (per_cpu(lb_state, tmp_cpu) == MT_LBPROF_IDLE_STATE ) || (per_cpu(lb_state, tmp_cpu) == MT_LBPROF_NO_TASK_STATE) ){
					break;
				}
				per_cpu(lb_state, tmp_cpu) = MT_LBPROF_NO_TASK_STATE;
				break;
			default:
				if( (per_cpu(lb_state, tmp_cpu) == MT_LBPROF_N_TASK_STATE ) || (per_cpu(lb_state, tmp_cpu) == MT_LBPROF_AFFINITY_STATE) || (per_cpu(lb_state, tmp_cpu) == MT_LBPROF_FAILURE_STATE) || 
					(per_cpu(lb_state, tmp_cpu) == MT_LBPROF_BALANCE_FAIL_STATE) || (per_cpu(lb_state, tmp_cpu) == MT_LBPROF_ALLPINNED) ){
					break;
				}
				per_cpu(lb_state, tmp_cpu) = MT_LBPROF_N_TASK_STATE;
		}
	}

	post_state[0] = per_cpu(lb_state, 0);
	post_state[1] = per_cpu(lb_state, 1);

	if(delta >= 1000000 && unbalance){
		if( pre_state[0] < 2 )
			mt_lbprof_printf_with_rqinfo(MT_LBPROF_UPDATE, "%d:%d:%d:update:%d:%d:%d %lu %llu ", cpu, pre_state[0], pre_state[1], rq_cnt, post_state[0], post_state[1], time_slice_ns[pre_state[0]][pre_state[1]-2], delta );
		else
			mt_lbprof_printf_with_rqinfo(MT_LBPROF_UPDATE, "%d:%d:%d:update:%d:%d:%d %lu %llu ", cpu, pre_state[0], pre_state[1], rq_cnt, post_state[0], post_state[1], time_slice_ns[pre_state[1]][pre_state[0]-2], delta );
	}
	
	start = now;

	spin_unlock_irqrestore(&mt_lbprof_check_cpu_idle_spinlock, irq_flags);

	return;
}

void mt_lbprof_update_state(int cpu, int rq_cnt){
	struct rq *rq;
	unsigned long irq_flags;
	
	rq = cpu_rq(cpu);
	raw_spin_lock_irqsave(&rq->lock, irq_flags);
	mt_lbprof_update_state_has_lock(cpu, rq_cnt);
	raw_spin_unlock_irqrestore(&rq->lock, irq_flags);
}

#define TRIMz(x)  ((tz = (unsigned long long)(x)) < 0 ? 0 : tz)
void mt_lbprof_update_status(void)
{	
	int cpu, i, j;
	unsigned long long  now, delta;
	unsigned long irq_flags;	
	unsigned long long end_idle_time = 0;
	unsigned long lb_idle_time = 0 ;
	unsigned long cpu_load, period_time_32;
	char cpu_load_info[30]="", cpu_load_info_tmp[8];

	if( !mt_lbprof_start )
		return;

	if( !mt_lbprof_tst_flag(MT_LBPROF_LBSTATUS))
		return;
	
	spin_lock_irqsave(&mt_lbprof_output_spinlock, irq_flags);
	
	now = local_clock();
	delta = now - last_update;

	if( delta < 1000000000){
		spin_unlock_irqrestore(&mt_lbprof_output_spinlock, irq_flags);
		return;
	}

	last_update = now;
	spin_unlock_irqrestore(&mt_lbprof_output_spinlock, irq_flags);

	cpu = smp_processor_id();
	mt_lbprof_update_state(cpu, MT_LBPROF_UPDATE_STATE);

	spin_lock_irqsave(&mt_lbprof_check_cpu_idle_spinlock, irq_flags);
	do_div(period_time, 1000);
	period_time_32 = period_time;

	for_each_present_cpu (cpu) {
		end_idle_time = mtprof_get_cpu_idle(cpu);
		lb_idle_time = end_idle_time - per_cpu(start_idle_time, cpu);
		per_cpu(start_idle_time, cpu) = end_idle_time;
		cpu_load = 10000 - lb_idle_time * 2500/(period_time_32/4);
		snprintf(cpu_load_info_tmp, 8, "%3lu.%02lu ", SPLIT_PERCENT(cpu_load));
		strcat(cpu_load_info, cpu_load_info_tmp);
	}
	
	period_time_32/=10;

	mt_lbprof_printf(MT_LBPROF_LBSTATUS, "%lu %3lu.%02lu %s %3lu.%02lu %3lu.%02lu %3lu.%02lu %3lu.%02lu %3lu.%02lu %3lu.%02lu %3lu.%02lu\n",
		period_time_32, 
		SPLIT_PERCENT(10000 - (unbalance_slice_ns ) / period_time_32 ), 
		cpu_load_info,
		SPLIT_PERCENT(unbalance_slice_ns / period_time_32),
		SPLIT_PERCENT(time_slice_ns[0][0] / period_time_32),
		SPLIT_PERCENT(time_slice_ns[0][1] / period_time_32),
		SPLIT_PERCENT(time_slice_ns[0][2] / period_time_32),
		SPLIT_PERCENT(time_slice_ns[1][0] / period_time_32),
		SPLIT_PERCENT(time_slice_ns[1][1] / period_time_32),
		SPLIT_PERCENT(time_slice_ns[1][2] / period_time_32));

	for(i = 0; i < 2; i++){
		for(j = 0 ; j < 3 ; j++){
			time_slice_ns[i][j]=0;
		}
	}
	unbalance_slice_ns = 0;
	period_time=0;
	spin_unlock_irqrestore(&mt_lbprof_check_cpu_idle_spinlock, irq_flags);
}

void mt_lbprof_tick(void)
{
	if( mt_lbprof_tst_flag(MT_LBPROF_LBSTATUS))
		wake_up_interruptible(mt_lbprof_stats.log_wait);
	
	if( mt_lbprof_any_enable(MT_LBPROF_ALL_LOG_ENABLE))
		wake_up_interruptible(mt_lbprof_log.log_wait);
}

#else /* CONFIG_MT_LOAD_BALANCE_PROFILER */

void mt_lbprof_update_status(void)
{
}

void mt_lbprof_tick(void)
{
}

void mt_lbprof_update_state(int cpu, int rq_cnt)
{
}

void mt_lbprof_update_state_has_lock(int cpu, int rq_cnt)
{
}

#endif /* CONFIG_MT_LOAD_BALANCE_PROFILER */
