#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include "aed.h"

#ifndef PARTIAL_BUILD
static int proc_read_generate_oops(char *page, char **start,
			     off_t off, int count,
			     int *eof, void *data)
{
	int len;

#if 0	
	aee_bug("AED-12345678901234567890", "Test from generate oops");
#endif
	BUG();
	len = sprintf(page, "Oops Generated\n");

	return len;
}

extern void aed_md_exception2(const int *log, int log_size, const int *phy, 
                                int phy_size, const char* detail);

static int proc_read_generate_ee(char *page, char **start,
				 off_t off, int count,
				 int *eof, void *data)
{
#define TEST_PHY_SIZE 0x10000

	int log[16], i;
	char *ptr;

	memset(log, 0, sizeof(log));

	ptr = kmalloc(TEST_PHY_SIZE, GFP_KERNEL);
	for (i = 0; i < TEST_PHY_SIZE; i++) {
		ptr[i] = (i % 26) + 'A';
	}
	aed_md_exception2(log, 0, (int *)ptr, TEST_PHY_SIZE, __func__);
	kfree(ptr);

	return sprintf(page, "ME Generated\n");
}

static int proc_read_generate_kernel_notify(char *page, char **start,
					     off_t off, int count,
					     int *eof, void *data)
{
	return sprintf(page, "Usage: write message with format \"R|W|E:Tag:You Message\" into this file to generate kernel warning\n");
}

static int proc_write_generate_kernel_notify(struct file* file,
					      const char __user *buf, unsigned long count,
					      void *data)
{
	char msg[164], *colon_ptr;

	if (count == 0)	{
		return -EINVAL;
	}

	if ((count < 5) || (count >= sizeof(msg))) {
		xlog_printk(ANDROID_LOG_WARN, AEK_LOG_TAG, "aed: %s count sould be >= 5 and <= %d bytes.\n", __func__, sizeof(msg));
		return -EINVAL;
	}

	if (copy_from_user(msg, buf, count)) {
		xlog_printk(ANDROID_LOG_WARN, AEK_LOG_TAG, "aed: %s unable to read message\n", __func__);
		return -EFAULT;
	}
	/* Be safe */
	msg[count] = 0;

        if (msg[1] != ':') {
		return -EINVAL;
        }
	colon_ptr = strchr(&msg[2], ':');
	if ((colon_ptr == NULL) || ((colon_ptr - msg) > 32)) {
		xlog_printk(ANDROID_LOG_WARN, AEK_LOG_TAG, "aed: %s cannot find valid module name\n", __func__); 
		return -EINVAL;
	}
	*colon_ptr = 0;
	
	switch (msg[0]) {
	case 'R':
		aee_kernel_reminding(&msg[2], colon_ptr + 1);
		break;

	case 'W':
		aee_kernel_warning(&msg[2], colon_ptr + 1);
		break;

	case 'E':
		aee_kernel_exception(&msg[2], colon_ptr + 1);
		break;

	default:
		return -EINVAL;
	}

	return count;
}

int aed_proc_debug_init(struct proc_dir_entry *aed_proc_dir)
{
	struct proc_dir_entry *aed_proc_generate_oops_file;
	struct proc_dir_entry *aed_proc_generate_ee_file;
	struct proc_dir_entry *aed_proc_generate_ke_file;

	aed_proc_generate_oops_file = create_proc_read_entry("generate-oops", 
							     0400, aed_proc_dir, 
							     proc_read_generate_oops,
							     NULL);
	if (aed_proc_generate_oops_file == NULL) {
	  xlog_printk(ANDROID_LOG_ERROR, AEK_LOG_TAG, "aed create_proc_read_entry failed at generate-oops\n");
	  return -ENOMEM;
	}

	aed_proc_generate_ke_file = create_proc_entry("generate-kernel-notify",
						      S_IFREG | 0600, aed_proc_dir);
	if (aed_proc_generate_ke_file == NULL) {
	  xlog_printk(ANDROID_LOG_ERROR, AEK_LOG_TAG, "aed create_proc_read_entry failed at generate-kernel-notify\n");
	  return -ENOMEM;
	}
	aed_proc_generate_ke_file->write_proc = proc_write_generate_kernel_notify;
	aed_proc_generate_ke_file->read_proc = proc_read_generate_kernel_notify;
	
	aed_proc_generate_ee_file = create_proc_read_entry("generate-ee", 
							   0400, aed_proc_dir, 
							   proc_read_generate_ee,
							   NULL);
	if(aed_proc_generate_ee_file == NULL) {
	  xlog_printk(ANDROID_LOG_ERROR, AEK_LOG_TAG, "aed create_proc_read_entry failed at generate-ee\n");
	  return -ENOMEM;
	}
	return 0;
}

int aed_proc_debug_done(struct proc_dir_entry *aed_proc_dir)
{
	remove_proc_entry("generate-oops", aed_proc_dir);
	remove_proc_entry("generate-kernel-notify", aed_proc_dir);
	remove_proc_entry("generate-ee", aed_proc_dir);
	return 0;
}

#else

int aed_proc_debug_init(struct proc_dir_entry *aed_proc_dir)
{
	return 0;
}

int aed_proc_debug_done(struct proc_dir_entry *aed_proc_dir)
{
	return 0;
}

#endif
