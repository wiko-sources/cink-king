/*
 * drivers/gpu/tegra/tegra_ion.c
 *
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/ion.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include "ion_priv.h"
#include <linux/ion_drv.h>
#include <asm/cacheflush.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/mmprofile.h>

#define ION_FUNC_ENTER  //MMProfileLogMetaString(MMP_ION_DEBUG, MMProfileFlagStart, __func__);
#define ION_FUNC_LEAVE  //MMProfileLogMetaString(MMP_ION_DEBUG, MMProfileFlagEnd, __func__);

//#pragma GCC optimize ("O0")
#define DEFAULT_PAGE_SIZE 0x1000
#define PAGE_MASK 0xFFF
#define PAGE_ORDER 12

struct ion_device *g_ion_device;
struct ion_heap *g_ion_heaps[ION_HEAP_IDX_MAX];

EXPORT_SYMBOL(g_ion_device);
EXPORT_SYMBOL(ion_kernel_ioctl);

// Import from multimedia heap
extern struct ion_heap_ops sys_contig_heap_ops;
extern struct ion_heap_ops mm_heap_ops;
long ion_mm_ioctl(struct ion_client *client, unsigned int cmd, unsigned long arg, int from_kernel);

void smp_inner_dcache_flush_all(void);

static long ion_sys_cache_sync(struct ion_client *client, ion_sys_cache_sync_param_t* pParam)
{
    ION_FUNC_ENTER;
    if (pParam->sync_type < ION_CACHE_CLEAN_ALL)
    {
        // By range operation
        unsigned int start = (unsigned int) ion_map_kernel(client, pParam->handle);
        size_t size = ion_handle_buffer(pParam->handle)->size;
        unsigned int end;
        unsigned int page_num;
        unsigned int i;
        unsigned int page_start;
        struct page* ppage;
        phys_addr_t phys_addr;
        // Cache line align
        end = start + size;
        start = (void*)(start / L1_CACHE_BYTES * L1_CACHE_BYTES);
        size = (end - start + L1_CACHE_BYTES - 1) / L1_CACHE_BYTES * L1_CACHE_BYTES;
        page_num = ((start&PAGE_MASK)+size+PAGE_MASK)>>PAGE_ORDER;
        page_start = start & (~PAGE_MASK);
        // L1 cache sync
        if (pParam->sync_type == ION_CACHE_CLEAN_BY_RANGE)
        {
            printk("[ion_sys_cache_sync]: ION cache clean by range. start=0x%08X size=0x%08X\n", start, size);
            dmac_map_area((void*)start, size, DMA_TO_DEVICE);
        }
        else if (pParam->sync_type == ION_CACHE_INVALID_BY_RANGE)
        {
            printk("[ion_sys_cache_sync]: ION cache invalid by range. start=0x%08X size=0x%08X\n", start, size);
            dmac_unmap_area((void*)start, size, DMA_FROM_DEVICE);
        }
        else if (pParam->sync_type == ION_CACHE_FLUSH_BY_RANGE)
        {
            printk("[ion_sys_cache_sync]: ION cache flush by range. start=0x%08X size=0x%08X\n", start, size);
            dmac_flush_range((void*)start, start+size-1);
        }
        // L2 cache sync
        for (i=0; i<page_num; i++, page_start+=DEFAULT_PAGE_SIZE)
        {
            if (page_start>=VMALLOC_START && page_start<=VMALLOC_END)
            {
                ppage = vmalloc_to_page((void*)page_start);
                if (!ppage)
                {
                    ion_unmap_kernel(client, pParam->handle);
                    return -EFAULT;
                }
                phys_addr = page_to_phys(ppage);
            }
            else
                phys_addr = virt_to_phys((void*)page_start);
            if (pParam->sync_type == ION_CACHE_CLEAN_BY_RANGE)
                outer_clean_range(phys_addr, phys_addr+DEFAULT_PAGE_SIZE);
            else if (pParam->sync_type == ION_CACHE_INVALID_BY_RANGE)
                outer_inv_range(phys_addr, phys_addr+DEFAULT_PAGE_SIZE);
            else if (pParam->sync_type == ION_CACHE_FLUSH_BY_RANGE)
                outer_flush_range(phys_addr, phys_addr+DEFAULT_PAGE_SIZE);
        }
        ion_unmap_kernel(client, pParam->handle);
    }
    else
    {
        // All cache operation
        if (pParam->sync_type == ION_CACHE_CLEAN_ALL)
        {
            printk("[ion_sys_cache_sync]: ION cache clean all.\n");
            smp_inner_dcache_flush_all();
            outer_clean_all();
        }
        else if (pParam->sync_type == ION_CACHE_INVALID_ALL)
        {
            printk("[ion_sys_cache_sync]: ION cache invalid all.\n");
            smp_inner_dcache_flush_all();
            outer_inv_all();
            //outer_flush_all();
        }
        else if (pParam->sync_type == ION_CACHE_FLUSH_ALL)
        {
            printk("[ion_sys_cache_sync]: ION cache flush all.\n");
            smp_inner_dcache_flush_all();
            outer_flush_all();
        }
    }
    ION_FUNC_LEAVE;
    return 0;
}

static long ion_sys_ioctl(struct ion_client *client, unsigned int cmd, unsigned long arg, int from_kernel)
{
    ion_sys_data_t Param;
    long ret = 0;
    ION_FUNC_ENTER;
    if (from_kernel)
        Param = *(ion_sys_data_t*) arg;
    else
        copy_from_user(&Param, (void __user *)arg, sizeof(ion_sys_data_t));
    switch (Param.sys_cmd)
    {
    case ION_SYS_CACHE_SYNC:
        ret = ion_sys_cache_sync(client, &Param.cache_sync_param);
        break;
    case ION_SYS_GET_PHYS:
        if (ion_phys(client, Param.get_phys_param.handle, &(Param.get_phys_param.phy_addr), &(Param.get_phys_param.len)) < 0)
        {
            Param.get_phys_param.phy_addr = 0;
            Param.get_phys_param.len = 0;
            printk("[ion_sys_ioctl]: Error. Cannot get physical address.\n");
            ret = -EFAULT;
        }
        break;
    case ION_SYS_GET_CLIENT:
        Param.get_client_param.client = (unsigned int) client;
        break;
    default:
        printk("[ion_sys_ioctl]: Error. Invalid command.\n");
        ret = -EFAULT;
        break;
    }
    if (from_kernel)
        *(ion_sys_data_t*)arg = Param;
    else
        copy_to_user((void __user *)arg, &Param, sizeof(ion_sys_data_t));
    ION_FUNC_LEAVE;
    return ret;
}

static long _ion_ioctl(struct ion_client *client, unsigned int cmd, unsigned long arg, int from_kernel)
{
    long ret = 0;
    ION_FUNC_ENTER;
    switch (cmd)
    {
    case ION_CMD_SYSTEM:
        ret = ion_sys_ioctl(client, cmd, arg, from_kernel);
        break;
    case ION_CMD_MULTIMEDIA:
        ret = ion_mm_ioctl(client, cmd, arg, from_kernel);
        break;
    }
    ION_FUNC_LEAVE;
    return ret;
}

long ion_kernel_ioctl(struct ion_client *client, unsigned int cmd, unsigned long arg)
{
    return _ion_ioctl(client, cmd, arg, 1);
}

static long ion_custom_ioctl(struct ion_client *client, unsigned int cmd, unsigned long arg)
{
    return _ion_ioctl(client, cmd, arg, 0);
}

int ion_drv_probe(struct platform_device *pdev)
{
    printk("ion_drv_probe()\n");

	g_ion_device = ion_device_create(ion_custom_ioctl);
	if (IS_ERR_OR_NULL(g_ion_device))
    {
        printk("ion_device_create() error! device=%x\n", g_ion_device);
        return PTR_ERR(g_ion_device);
	}
    // Add system contig heap. (physical continuous)
    g_ion_heaps[ION_HEAP_IDX_SYSTEM_CONTIG] = kzalloc(sizeof(struct ion_heap), GFP_KERNEL);
    if (!g_ion_heaps[ION_HEAP_IDX_SYSTEM_CONTIG])
    {
        printk("Cannot allocate memory for g_ion_heaps[ION_HEAP_ID_SYSTEM_CONTIG]\n");
        return ERR_PTR(-ENOMEM);
    }
    g_ion_heaps[ION_HEAP_IDX_SYSTEM_CONTIG]->ops = &sys_contig_heap_ops;
    g_ion_heaps[ION_HEAP_IDX_SYSTEM_CONTIG]->type = ION_HEAP_TYPE_SYSTEM_CONTIG;
    g_ion_heaps[ION_HEAP_IDX_SYSTEM_CONTIG]->id = ION_HEAP_TYPE_SYSTEM_CONTIG;
    g_ion_heaps[ION_HEAP_IDX_SYSTEM_CONTIG]->name = "ION System Contiguous Heap";
    ion_device_add_heap(g_ion_device, g_ion_heaps[ION_HEAP_IDX_SYSTEM_CONTIG]);

    // Add multimedia heap. (using M4U MVA as physical address)
    g_ion_heaps[ION_HEAP_IDX_MULTIMEDIA] = kzalloc(sizeof(struct ion_heap), GFP_KERNEL);
    if (!g_ion_heaps[ION_HEAP_IDX_MULTIMEDIA])
    {
        printk("Cannot allocate memory for g_ion_heaps[ION_HEAP_ID_MULTIMEDIA]\n");
        return ERR_PTR(-ENOMEM);
    }
    g_ion_heaps[ION_HEAP_IDX_MULTIMEDIA]->ops = &mm_heap_ops;
    g_ion_heaps[ION_HEAP_IDX_MULTIMEDIA]->type = ION_HEAP_TYPE_MULTIMEDIA;
    g_ion_heaps[ION_HEAP_IDX_MULTIMEDIA]->name = "ION Multimedia Heap";
    g_ion_heaps[ION_HEAP_IDX_MULTIMEDIA]->id = ION_HEAP_TYPE_MULTIMEDIA;
	ion_device_add_heap(g_ion_device, g_ion_heaps[ION_HEAP_IDX_MULTIMEDIA]);

	platform_set_drvdata(pdev, g_ion_device);
	return 0;
}

int ion_drv_remove(struct platform_device *pdev)
{
    unsigned int i;
	struct ion_device *idev = platform_get_drvdata(pdev);
    for (i=0; i<ION_HEAP_IDX_MAX; i++)
	    ion_heap_destroy(g_ion_heaps[i]);
    ion_device_destroy(idev);
	return 0;
}

static struct platform_driver ion_driver = {
	.probe = ion_drv_probe,
	.remove = ion_drv_remove,
	.driver = { .name = "ion-drv" }
};

static struct platform_device ion_device = {
	.name = "ion-drv",
	.id   = 0,
};

static int __init ion_init(void)
{
    printk("ion_init()\n");
	if (platform_device_register(&ion_device))
	{
		return -ENODEV;
	}
	if (platform_driver_register(&ion_driver))
	{
		platform_device_unregister(&ion_device);
		return -ENODEV;
	}
	return 0;
}

static void __exit ion_exit(void)
{
	platform_driver_unregister(&ion_driver);
	platform_device_unregister(&ion_device);
}

module_init(ion_init);
module_exit(ion_exit);

