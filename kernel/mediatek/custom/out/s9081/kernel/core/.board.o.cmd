cmd_mediatek/custom/out/s9081/kernel/core/board.o := arm-linux-androideabi-gcc -Wp,-MD,mediatek/custom/out/s9081/kernel/core/.board.o.d  -nostdinc -isystem /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/arm-linux-androideabi-4.4.x/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include -I/home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -I../mediatek/custom//common -I../mediatek/platform/mt6577/kernel/core/include/ -I../mediatek/source/kernel/include/ -I../mediatek/custom/out/s9081/kernel/magnetometer/ -I../mediatek/custom/out/s9081/kernel/headset/ -I../mediatek/custom/out/s9081/kernel/kpd/ -I../mediatek/custom/out/s9081/kernel/accelerometer/ -I../mediatek/custom/out/s9081/kernel/touchpanel/ -I../mediatek/custom/out/s9081/kernel/rtc/ -I../mediatek/custom/out/s9081/kernel/core/ -I../mediatek/custom/out/s9081/kernel/leds/ -I../mediatek/custom/out/s9081/kernel/battery/ -I../mediatek/custom/out/s9081/kernel/camera/ -I../mediatek/custom/out/s9081/kernel/alsps/ -I../mediatek/custom/out/s9081/kernel/vibrator/ -I../mediatek/custom/out/s9081/kernel/imgsensor/ -I../mediatek/custom/out/s9081/kernel/flashlight/ -I../mediatek/custom/out/s9081/kernel/dct/ -I../mediatek/custom/out/s9081/kernel/usb/ -I../mediatek/custom/out/s9081/kernel/magnetometer/inc/ -I../mediatek/custom/out/s9081/kernel/gyroscope/inc/ -I../mediatek/custom/out/s9081/kernel/sound/inc/ -I../mediatek/custom/out/s9081/kernel/sound/ -I../mediatek/custom/out/s9081/kernel/lens/inc/ -I../mediatek/custom/out/s9081/kernel/lens/ -I../mediatek/custom/out/s9081/kernel/accelerometer/inc/ -I../mediatek/custom/out/s9081/kernel/eeprom/inc/ -I../mediatek/custom/out/s9081/kernel/eeprom/ -I../mediatek/custom/out/s9081/kernel/hdmi/inc/ -I../mediatek/custom/out/s9081/kernel/jogball/inc/ -I../mediatek/custom/out/s9081/kernel/leds/inc/ -I../mediatek/custom/out/s9081/kernel/barometer/inc/ -I../mediatek/custom/out/s9081/kernel/./ -I../mediatek/custom/out/s9081/kernel/lcm/inc/ -I../mediatek/custom/out/s9081/kernel/lcm/ -I../mediatek/custom/out/s9081/kernel/alsps/inc/ -I../mediatek/custom/out/s9081/kernel/imgsensor/inc/ -I../mediatek/custom/out/s9081/kernel/flashlight/inc/ -I../mediatek/custom/out/s9081/kernel/ccci/ -I../mediatek/custom/out/s9081/hal/sensors/ -I../mediatek/custom/out/s9081/hal/camera/ -I../mediatek/custom/out/s9081/hal/audioflinger/ -I../mediatek/custom/out/s9081/hal/combo/ -I../mediatek/custom/out/s9081/hal/inc/ -I../mediatek/custom/out/s9081/hal/ant/ -I../mediatek/custom/out/s9081/hal/fmradio/ -I../mediatek/custom/out/s9081/hal/lens/ -I../mediatek/custom/out/s9081/hal/eeprom/ -I../mediatek/custom/out/s9081/hal/vt/ -I../mediatek/custom/out/s9081/hal/bluetooth/ -I../mediatek/custom/out/s9081/hal/imgsensor/ -I../mediatek/custom/out/s9081/hal/flashlight/ -I../mediatek/custom/out/s9081/hal/matv/ -I../mediatek/custom/out/s9081/common -D__KERNEL__ -mlittle-endian -DMODEM_3G -Imediatek/platform/mt6577/kernel/core/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -fno-pic -marm -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -DHAVE_XLOG_FEATURE -DMTK_BT_PROFILE_MAPS -DMTK_BT_PROFILE_AVRCP -DMTK_WB_SPEECH_SUPPORT -DMTK_WIFI_HOTSPOT_SUPPORT -DMTK_GPS_SUPPORT -DMTK_BT_PROFILE_OPP -DMTK_BT_PROFILE_A2DP -DMTK_BT_PROFILE_MAPC -DMTK_FM_RECORDING_SUPPORT -DMTK_FM_SUPPORT -DMTK_AUTORAMA_SUPPORT -DMTK_FM_SHORT_ANTENNA_SUPPORT -DMTK_MULTI_STORAGE_SUPPORT -DCUSTOM_KERNEL_ALSPS -DMTK_BT_PROFILE_SPP -DMTK_BT_30_SUPPORT -DMTK_BT_PROFILE_MANAGER -DMTK_BT_PROFILE_HFP -DMTK_EAP_SIM_AKA -DMTK_BT_PROFILE_SIMAP -DMTK_DUAL_MIC_SUPPORT -DMTK_TVOUT_SUPPORT -DMTK_COMBO_SUPPORT -DCUSTOM_KERNEL_ACCELEROMETER -DMTK_WAPI_SUPPORT -DMTK_FD_SUPPORT -DHAVE_AACENCODE_FEATURE -DMTK_BT_21_SUPPORT -DMTK_IPV6_SUPPORT -DMTK_MASS_STORAGE -DMTK_FACEBEAUTY_SUPPORT -DMTK_MAV_SUPPORT -DMTK_BT_SUPPORT -DMTK_BT_PROFILE_PBAP -DMTK_CAMERA_BSP_SUPPORT -DMTK_FM_RX_SUPPORT -DMTK_BT_PROFILE_PAN -DMTK_BT_40_SUPPORT -DCUSTOM_KERNEL_MAGNETOMETER -DMTK_BT_PROFILE_FTP -DMTK_VT3G324M_SUPPORT -DHAVE_AWBENCODE_FEATURE -DMTK_BT_PROFILE_AVRCP14 -DMTK_BT_PROFILE_DUN -DMTK_CAMERA_APP_3DHW_SUPPORT -DMTK_BT_PROFILE_BIP -DMTK_BT_PROFILE_BPP -DMTK_WLAN_SUPPORT -DMTK_SENSOR_SUPPORT -DMTK_M4U_SUPPORT -DMTK_HDR_SUPPORT -DMTK_EMMC_SUPPORT -DMTK_AUTO_DETECT_ACCELEROMETER -DMTK_USES_HD_VIDEO -DMTK_COMBO_QUICK_SLEEP_SUPPORT -DMTK_COMBO_CORE_DUMP_SUPPORT -DCUSTOM_HAL_FMRADIO -DMTK_SD_REINIT_SUPPORT -DMTK_2SDCARD_SWAP -DMTK_SWAP_STATIC_MODE -DMT6577 -DOV8825AF -DOV8826AF -DDUMMY_LENS -DOV8826AF -DLT5002F_DSI -DR61408 -DNT35510_DSI -DMODEM_3G -DOV8825_MIPI_RAW -DMTK_MT6628 -DMT6628 -DOV8826_MIPI_RAW -DCONSTANT_FLASHLIGHT -DHI704_YUV -DDUMMY_LENS -DOV8825_MIPI_RAW -DOV8826_MIPI_RAW -DHI704_YUV -DFM_ANALOG_INPUT -DMT6628 -DMTK_GPS_MT6628 -DOV8825AF -DFM_ANALOG_OUTPUT -DMT6628_FM -DMTK_LCM_PHYSICAL_ROTATION=\"0\" -DLCM_WIDTH=\"480\" -DMTK_SHARE_MODEM_SUPPORT=\"2\" -DMTK_NEON_SUPPORT=\"yes\" -DMTK_SHARE_MODEM_CURRENT=\"2\" -DLCM_HEIGHT=\"854\" -DTINNO_PROJECT_NAME=\"generic\" -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(board)"  -D"KBUILD_MODNAME=KBUILD_STR(board)" -c -o mediatek/custom/out/s9081/kernel/core/board.o mediatek/custom/out/s9081/kernel/core/board.c

source_mediatek/custom/out/s9081/kernel/core/board.o := mediatek/custom/out/s9081/kernel/core/board.c

deps_mediatek/custom/out/s9081/kernel/core/board.o := \
    $(wildcard include/config/mtk/combo.h) \
    $(wildcard include/config/mtk/wcn/cmb/sdio/slot.h) \
    $(wildcard include/config/wlan.h) \
    $(wildcard include/config/mtk/mtd/nand.h) \
  include/linux/kernel.h \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/arm-linux-androideabi-4.4.x/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/posix_types.h \
  include/linux/bitops.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/smp.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/system.h \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/v6.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/preempt/monitor.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  include/linux/typecheck.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/irqflags.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/hwcap.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/outercache.h \
    $(wildcard include/config/outer/cache/sync.h) \
    $(wildcard include/config/outer/cache.h) \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/lock.h \
  include/asm-generic/bitops/le.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/printk.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/dynamic_debug.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/div64.h \
  include/linux/mm.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
  include/linux/errno.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/gfp.h \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/const.h \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/spinlock.h \
    $(wildcard include/config/thumb2/kernel.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/processor.h \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/arm/errata/754327.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/hw_breakpoint.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/inline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/atomic.h \
    $(wildcard include/config/generic/atomic64.h) \
  include/asm-generic/atomic-long.h \
  include/linux/wait.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/current.h \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/seqlock.h \
  include/linux/nodemask.h \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/string.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/generated/bounds.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v3.h) \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/glue.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/arm/patch/phys/virt/16bit.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/memory.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/sizes.h \
  include/asm-generic/sizes.h \
  include/asm-generic/memory_model.h \
  include/asm-generic/getorder.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mt/lock/debug.h) \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/smp.h \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/pfn.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/rbtree.h \
  include/linux/prio_tree.h \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
  include/linux/auxvec.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/auxvec.h \
  include/linux/completion.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/mmu.h \
    $(wildcard include/config/cpu/has/asid.h) \
  include/linux/range.h \
  include/linux/bit_spinlock.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/pgtable.h \
    $(wildcard include/config/highpte.h) \
  include/asm-generic/4level-fixup.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/proc-fns.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/glue-proc.h \
    $(wildcard include/config/cpu/arm610.h) \
    $(wildcard include/config/cpu/arm7tdmi.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/cpu/arm720t.h) \
    $(wildcard include/config/cpu/arm740t.h) \
    $(wildcard include/config/cpu/arm9tdmi.h) \
    $(wildcard include/config/cpu/arm920t.h) \
    $(wildcard include/config/cpu/arm922t.h) \
    $(wildcard include/config/cpu/arm925t.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/arm940t.h) \
    $(wildcard include/config/cpu/arm946e.h) \
    $(wildcard include/config/cpu/arm1020.h) \
    $(wildcard include/config/cpu/arm1020e.h) \
    $(wildcard include/config/cpu/arm1022.h) \
    $(wildcard include/config/cpu/arm1026.h) \
    $(wildcard include/config/cpu/mohawk.h) \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/cpu/v6k.h) \
    $(wildcard include/config/cpu/v7.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/vmalloc.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/pgtable-hwdef.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/s390.h) \
  include/linux/huge_mm.h \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  include/linux/vm_event_item.h \
  include/linux/interrupt.h \
    $(wildcard include/config/mtprof/irq/duration.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/irq/forced/threading.h) \
    $(wildcard include/config/generic/irq/probe.h) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/linux/hardirq.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tiny/preempt/rcu.h) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/hardirq.h \
    $(wildcard include/config/local/timers.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/irq.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/irqs.h \
    $(wildcard include/config/fiq/glue.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt6577_irq.h \
    $(wildcard include/config/mt6577/fpga.h) \
  include/linux/irq_cpustat.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/math64.h \
  include/linux/jiffies.h \
  include/linux/timex.h \
  include/linux/param.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/timex.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/timex.h \
  include/linux/timer.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/timerqueue.h \
  include/linux/kref.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  include/trace/events/irq.h \
  include/linux/tracepoint.h \
    $(wildcard include/config/tracepoints.h) \
  include/linux/rcupdate.h \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/preempt/rt.h) \
  include/linux/rcutree.h \
  include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  include/trace/define_trace.h \
    $(wildcard include/config/event/tracing.h) \
  include/linux/module.h \
    $(wildcard include/config/symbol/prefix.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/ftrace/module/support.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/debug/set/module/ronx.h) \
  include/linux/stat.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/stat.h \
  include/linux/kmod.h \
  include/linux/sysctl.h \
  include/linux/elf.h \
  include/linux/elf-em.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/elf.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/user.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ppc64.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/module.h \
    $(wildcard include/config/arm/unwind.h) \
  include/trace/events/module.h \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/pm/runtime.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/device.h \
    $(wildcard include/config/dmabounce.h) \
  include/linux/pm_wakeup.h \
  include/linux/delay.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/delay.h \
  include/linux/uaccess.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/uaccess.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
  include/linux/syscalls.h \
    $(wildcard include/config/ftrace/syscalls.h) \
    $(wildcard include/config/mips.h) \
    $(wildcard include/config/have/syscall/wrappers.h) \
  include/linux/aio_abi.h \
  include/linux/capability.h \
  include/linux/sem.h \
    $(wildcard include/config/sysvipc.h) \
  include/linux/ipc.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/ipcbuf.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/sembuf.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/signal.h \
  include/asm-generic/signal-defs.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/sigcontext.h \
  include/linux/unistd.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/unistd.h \
    $(wildcard include/config/oabi/compat.h) \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/percpu_counter.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/key.h \
    $(wildcard include/config/keys.h) \
  include/trace/syscall.h \
    $(wildcard include/config/perf/events.h) \
  include/linux/ftrace_event.h \
  include/linux/ring_buffer.h \
    $(wildcard include/config/ring/buffer/allow/swap.h) \
  include/linux/kmemcheck.h \
  include/linux/seq_file.h \
  include/linux/trace_seq.h \
  include/linux/fs.h \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/ioctl.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/blk_types.h \
    $(wildcard include/config/blk/dev/integrity.h) \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/path.h \
  include/linux/radix-tree.h \
  include/linux/pid.h \
  include/linux/semaphore.h \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/mtk/load/balance/profiler.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/mtprof/cputime.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/cgroup/sched.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/signal.h \
  include/linux/proportions.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
    $(wildcard include/config/user/ns.h) \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  ../mediatek/source/kernel/include/linux/rtpm_prio.h \
    $(wildcard include/config/mt/rt/monitor.h) \
  include/linux/aio.h \
  include/linux/uio.h \
  include/linux/ftrace.h \
    $(wildcard include/config/function/tracer.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
    $(wildcard include/config/stack/tracer.h) \
    $(wildcard include/config/frame/pointer.h) \
  include/linux/trace_clock.h \
  include/linux/kallsyms.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/ftrace.h \
    $(wildcard include/config/old/mcount.h) \
  include/linux/fiemap.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/inet.h \
  include/linux/fcntl.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/err.h \
  include/linux/perf_event.h \
    $(wildcard include/config/perf/use/vmalloc.h) \
    $(wildcard include/config/cgroup/perf.h) \
  include/linux/pid_namespace.h \
    $(wildcard include/config/pid/ns.h) \
  include/linux/nsproxy.h \
  include/linux/cpu.h \
    $(wildcard include/config/arch/cpu/probe/release.h) \
    $(wildcard include/config/pm/sleep/smp.h) \
  include/linux/sysdev.h \
  include/linux/node.h \
    $(wildcard include/config/memory/hotplug/sparse.h) \
  include/linux/irq_work.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/local.h \
  include/asm-generic/local.h \
  include/linux/mtd/nand.h \
  include/linux/mtd/mtd.h \
    $(wildcard include/config/mtd/debug.h) \
    $(wildcard include/config/mtd/debug/verbose.h) \
  include/mtd/mtd-abi.h \
  include/linux/mtd/flashchip.h \
  include/linux/mtd/bbm.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/io.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/io.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/mach-types.h \
  include/generated/mach-types.h \
    $(wildcard include/config/arch/ebsa110.h) \
    $(wildcard include/config/arch/rpc.h) \
    $(wildcard include/config/arch/ebsa285.h) \
    $(wildcard include/config/arch/netwinder.h) \
    $(wildcard include/config/arch/cats.h) \
    $(wildcard include/config/arch/shark.h) \
    $(wildcard include/config/sa1100/brutus.h) \
    $(wildcard include/config/arch/personal/server.h) \
    $(wildcard include/config/arch/l7200.h) \
    $(wildcard include/config/sa1100/pleb.h) \
    $(wildcard include/config/arch/integrator.h) \
    $(wildcard include/config/sa1100/h3600.h) \
    $(wildcard include/config/arch/p720t.h) \
    $(wildcard include/config/sa1100/assabet.h) \
    $(wildcard include/config/sa1100/lart.h) \
    $(wildcard include/config/sa1100/graphicsclient.h) \
    $(wildcard include/config/sa1100/xp860.h) \
    $(wildcard include/config/sa1100/cerf.h) \
    $(wildcard include/config/sa1100/nanoengine.h) \
    $(wildcard include/config/sa1100/jornada720.h) \
    $(wildcard include/config/arch/edb7211.h) \
    $(wildcard include/config/sa1100/pfs168.h) \
    $(wildcard include/config/sa1100/flexanet.h) \
    $(wildcard include/config/sa1100/simpad.h) \
    $(wildcard include/config/arch/lubbock.h) \
    $(wildcard include/config/arch/clep7212.h) \
    $(wildcard include/config/sa1100/shannon.h) \
    $(wildcard include/config/sa1100/consus.h) \
    $(wildcard include/config/arch/aaed2000.h) \
    $(wildcard include/config/arch/cdb89712.h) \
    $(wildcard include/config/sa1100/graphicsmaster.h) \
    $(wildcard include/config/sa1100/adsbitsy.h) \
    $(wildcard include/config/arch/pxa/idp.h) \
    $(wildcard include/config/sa1100/pt/system3.h) \
    $(wildcard include/config/arch/autcpu12.h) \
    $(wildcard include/config/sa1100/h3100.h) \
    $(wildcard include/config/sa1100/collie.h) \
    $(wildcard include/config/sa1100/badge4.h) \
    $(wildcard include/config/arch/fortunet.h) \
    $(wildcard include/config/arch/mx1ads.h) \
    $(wildcard include/config/arch/h7201.h) \
    $(wildcard include/config/arch/h7202.h) \
    $(wildcard include/config/arch/iq80321.h) \
    $(wildcard include/config/arch/ks8695.h) \
    $(wildcard include/config/arch/smdk2410.h) \
    $(wildcard include/config/arch/ceiva.h) \
    $(wildcard include/config/mach/voiceblue.h) \
    $(wildcard include/config/arch/h5400.h) \
    $(wildcard include/config/mach/omap/innovator.h) \
    $(wildcard include/config/arch/ixdp2400.h) \
    $(wildcard include/config/arch/ixdp2800.h) \
    $(wildcard include/config/arch/ixdp425.h) \
    $(wildcard include/config/sa1100/hackkit.h) \
    $(wildcard include/config/arch/ixcdp1100.h) \
    $(wildcard include/config/arch/at91rm9200dk.h) \
    $(wildcard include/config/arch/cintegrator.h) \
    $(wildcard include/config/arch/viper.h) \
    $(wildcard include/config/arch/adi/coyote.h) \
    $(wildcard include/config/arch/ixdp2401.h) \
    $(wildcard include/config/arch/ixdp2801.h) \
    $(wildcard include/config/arch/iq31244.h) \
    $(wildcard include/config/arch/bast.h) \
    $(wildcard include/config/arch/h1940.h) \
    $(wildcard include/config/arch/enp2611.h) \
    $(wildcard include/config/arch/s3c2440.h) \
    $(wildcard include/config/arch/gumstix.h) \
    $(wildcard include/config/mach/omap/h2.h) \
    $(wildcard include/config/mach/e740.h) \
    $(wildcard include/config/arch/iq80331.h) \
    $(wildcard include/config/arch/versatile/pb.h) \
    $(wildcard include/config/mach/kev7a400.h) \
    $(wildcard include/config/mach/lpd7a400.h) \
    $(wildcard include/config/mach/lpd7a404.h) \
    $(wildcard include/config/mach/csb337.h) \
    $(wildcard include/config/mach/mainstone.h) \
    $(wildcard include/config/mach/xcep.h) \
    $(wildcard include/config/mach/arcom/vulcan.h) \
    $(wildcard include/config/mach/nomadik.h) \
    $(wildcard include/config/mach/corgi.h) \
    $(wildcard include/config/mach/poodle.h) \
    $(wildcard include/config/mach/armcore.h) \
    $(wildcard include/config/mach/mx31ads.h) \
    $(wildcard include/config/mach/himalaya.h) \
    $(wildcard include/config/mach/edb9312.h) \
    $(wildcard include/config/mach/omap/generic.h) \
    $(wildcard include/config/mach/edb9301.h) \
    $(wildcard include/config/mach/edb9315.h) \
    $(wildcard include/config/mach/vr1000.h) \
    $(wildcard include/config/mach/omap/perseus2.h) \
    $(wildcard include/config/mach/e800.h) \
    $(wildcard include/config/mach/e750.h) \
    $(wildcard include/config/mach/scb9328.h) \
    $(wildcard include/config/mach/omap/h3.h) \
    $(wildcard include/config/mach/omap/h4.h) \
    $(wildcard include/config/mach/omap/osk.h) \
    $(wildcard include/config/mach/tosa.h) \
    $(wildcard include/config/mach/avila.h) \
    $(wildcard include/config/mach/edb9302.h) \
    $(wildcard include/config/mach/husky.h) \
    $(wildcard include/config/mach/shepherd.h) \
    $(wildcard include/config/mach/h4700.h) \
    $(wildcard include/config/mach/rx3715.h) \
    $(wildcard include/config/mach/nslu2.h) \
    $(wildcard include/config/mach/e400.h) \
    $(wildcard include/config/mach/ixdpg425.h) \
    $(wildcard include/config/mach/versatile/ab.h) \
    $(wildcard include/config/mach/edb9307.h) \
    $(wildcard include/config/mach/kb9200.h) \
    $(wildcard include/config/mach/sx1.h) \
    $(wildcard include/config/mach/ixdp465.h) \
    $(wildcard include/config/mach/ixdp2351.h) \
    $(wildcard include/config/mach/iq80332.h) \
    $(wildcard include/config/mach/gtwx5715.h) \
    $(wildcard include/config/mach/csb637.h) \
    $(wildcard include/config/mach/n30.h) \
    $(wildcard include/config/mach/nec/mp900.h) \
    $(wildcard include/config/mach/kafa.h) \
    $(wildcard include/config/mach/ts72xx.h) \
    $(wildcard include/config/mach/otom.h) \
    $(wildcard include/config/mach/nexcoder/2440.h) \
    $(wildcard include/config/mach/eco920.h) \
    $(wildcard include/config/mach/roadrunner.h) \
    $(wildcard include/config/mach/at91rm9200ek.h) \
    $(wildcard include/config/mach/spitz.h) \
    $(wildcard include/config/mach/adssphere.h) \
    $(wildcard include/config/mach/colibri.h) \
    $(wildcard include/config/mach/gateway7001.h) \
    $(wildcard include/config/mach/pcm027.h) \
    $(wildcard include/config/mach/anubis.h) \
    $(wildcard include/config/mach/akita.h) \
    $(wildcard include/config/mach/e330.h) \
    $(wildcard include/config/mach/nokia770.h) \
    $(wildcard include/config/mach/carmeva.h) \
    $(wildcard include/config/mach/edb9315a.h) \
    $(wildcard include/config/mach/stargate2.h) \
    $(wildcard include/config/mach/intelmote2.h) \
    $(wildcard include/config/mach/trizeps4.h) \
    $(wildcard include/config/mach/pnx4008.h) \
    $(wildcard include/config/mach/cpuat91.h) \
    $(wildcard include/config/mach/iq81340sc.h) \
    $(wildcard include/config/mach/iq81340mc.h) \
    $(wildcard include/config/mach/micro9.h) \
    $(wildcard include/config/mach/micro9l.h) \
    $(wildcard include/config/mach/omap/palmte.h) \
    $(wildcard include/config/mach/realview/eb.h) \
    $(wildcard include/config/mach/borzoi.h) \
    $(wildcard include/config/mach/palmld.h) \
    $(wildcard include/config/mach/ixdp28x5.h) \
    $(wildcard include/config/mach/omap/palmtt.h) \
    $(wildcard include/config/mach/arcom/zeus.h) \
    $(wildcard include/config/mach/osiris.h) \
    $(wildcard include/config/mach/palmte2.h) \
    $(wildcard include/config/mach/mx27ads.h) \
    $(wildcard include/config/mach/at91sam9261ek.h) \
    $(wildcard include/config/mach/loft.h) \
    $(wildcard include/config/mach/mx21ads.h) \
    $(wildcard include/config/mach/ams/delta.h) \
    $(wildcard include/config/mach/nas100d.h) \
    $(wildcard include/config/mach/magician.h) \
    $(wildcard include/config/mach/nxdkn.h) \
    $(wildcard include/config/mach/palmtx.h) \
    $(wildcard include/config/mach/s3c2413.h) \
    $(wildcard include/config/mach/wg302v2.h) \
    $(wildcard include/config/mach/omap/2430sdp.h) \
    $(wildcard include/config/mach/davinci/evm.h) \
    $(wildcard include/config/mach/palmz72.h) \
    $(wildcard include/config/mach/nxdb500.h) \
    $(wildcard include/config/mach/palmt5.h) \
    $(wildcard include/config/mach/palmtc.h) \
    $(wildcard include/config/mach/omap/apollon.h) \
    $(wildcard include/config/mach/ateb9200.h) \
    $(wildcard include/config/mach/n35.h) \
    $(wildcard include/config/mach/logicpd/pxa270.h) \
    $(wildcard include/config/mach/nxeb500hmi.h) \
    $(wildcard include/config/mach/espresso.h) \
    $(wildcard include/config/mach/rx1950.h) \
    $(wildcard include/config/mach/gesbc9312.h) \
    $(wildcard include/config/mach/picotux2xx.h) \
    $(wildcard include/config/mach/dsmg600.h) \
    $(wildcard include/config/mach/omap/fsample.h) \
    $(wildcard include/config/mach/snapper/cl15.h) \
    $(wildcard include/config/mach/omap/palmz71.h) \
    $(wildcard include/config/mach/smdk2412.h) \
    $(wildcard include/config/mach/smdk2413.h) \
    $(wildcard include/config/mach/aml/m5900.h) \
    $(wildcard include/config/mach/balloon3.h) \
    $(wildcard include/config/mach/ecbat91.h) \
    $(wildcard include/config/mach/onearm.h) \
    $(wildcard include/config/mach/smdk2443.h) \
    $(wildcard include/config/mach/fsg.h) \
    $(wildcard include/config/mach/at91sam9260ek.h) \
    $(wildcard include/config/mach/glantank.h) \
    $(wildcard include/config/mach/n2100.h) \
    $(wildcard include/config/mach/qt2410.h) \
    $(wildcard include/config/mach/kixrp435.h) \
    $(wildcard include/config/mach/cc9p9360dev.h) \
    $(wildcard include/config/mach/edb9302a.h) \
    $(wildcard include/config/mach/edb9307a.h) \
    $(wildcard include/config/mach/omap/3430sdp.h) \
    $(wildcard include/config/mach/vstms.h) \
    $(wildcard include/config/mach/micro9m.h) \
    $(wildcard include/config/mach/bug.h) \
    $(wildcard include/config/mach/at91sam9263ek.h) \
    $(wildcard include/config/mach/em7210.h) \
    $(wildcard include/config/mach/vpac270.h) \
    $(wildcard include/config/mach/treo680.h) \
    $(wildcard include/config/mach/zylonite.h) \
    $(wildcard include/config/mach/mx31lite.h) \
    $(wildcard include/config/mach/mioa701.h) \
    $(wildcard include/config/mach/armadillo5x0.h) \
    $(wildcard include/config/mach/cc9p9360js.h) \
    $(wildcard include/config/mach/smdk6400.h) \
    $(wildcard include/config/mach/nokia/n800.h) \
    $(wildcard include/config/mach/ep80219.h) \
    $(wildcard include/config/mach/goramo/mlr.h) \
    $(wildcard include/config/mach/em/x270.h) \
    $(wildcard include/config/mach/neo1973/gta02.h) \
    $(wildcard include/config/mach/at91sam9rlek.h) \
    $(wildcard include/config/mach/colibri320.h) \
    $(wildcard include/config/mach/cam60.h) \
    $(wildcard include/config/mach/at91eb01.h) \
    $(wildcard include/config/mach/db88f5281.h) \
    $(wildcard include/config/mach/csb726.h) \
    $(wildcard include/config/mach/davinci/dm6467/evm.h) \
    $(wildcard include/config/mach/davinci/dm355/evm.h) \
    $(wildcard include/config/mach/littleton.h) \
    $(wildcard include/config/mach/realview/pb11mp.h) \
    $(wildcard include/config/mach/mx27/3ds.h) \
    $(wildcard include/config/mach/halibut.h) \
    $(wildcard include/config/mach/trout.h) \
    $(wildcard include/config/mach/tct/hammer.h) \
    $(wildcard include/config/mach/herald.h) \
    $(wildcard include/config/mach/sim/one.h) \
    $(wildcard include/config/mach/jive.h) \
    $(wildcard include/config/mach/sam9/l9260.h) \
    $(wildcard include/config/mach/realview/pb1176.h) \
    $(wildcard include/config/mach/yl9200.h) \
    $(wildcard include/config/mach/rd88f5182.h) \
    $(wildcard include/config/mach/kurobox/pro.h) \
    $(wildcard include/config/mach/mx31/3ds.h) \
    $(wildcard include/config/mach/qong.h) \
    $(wildcard include/config/mach/omap2evm.h) \
    $(wildcard include/config/mach/omap3evm.h) \
    $(wildcard include/config/mach/dns323.h) \
    $(wildcard include/config/mach/omap3/beagle.h) \
    $(wildcard include/config/mach/nokia/n810.h) \
    $(wildcard include/config/mach/pcm038.h) \
    $(wildcard include/config/mach/ts209.h) \
    $(wildcard include/config/mach/at91cap9adk.h) \
    $(wildcard include/config/mach/mx31moboard.h) \
    $(wildcard include/config/mach/terastation/pro2.h) \
    $(wildcard include/config/mach/linkstation/pro.h) \
    $(wildcard include/config/mach/e350.h) \
    $(wildcard include/config/mach/ts409.h) \
    $(wildcard include/config/mach/cm/x300.h) \
    $(wildcard include/config/mach/at91sam9g20ek.h) \
    $(wildcard include/config/mach/smdk6410.h) \
    $(wildcard include/config/mach/u300.h) \
    $(wildcard include/config/mach/wrt350n/v2.h) \
    $(wildcard include/config/mach/omap/ldp.h) \
    $(wildcard include/config/mach/mx35/3ds.h) \
    $(wildcard include/config/mach/neuros/osd2.h) \
    $(wildcard include/config/mach/trizeps4wl.h) \
    $(wildcard include/config/mach/ts78xx.h) \
    $(wildcard include/config/mach/sffsdr.h) \
    $(wildcard include/config/mach/pcm037.h) \
    $(wildcard include/config/mach/db88f6281/bp.h) \
    $(wildcard include/config/mach/rd88f6192/nas.h) \
    $(wildcard include/config/mach/rd88f6281.h) \
    $(wildcard include/config/mach/db78x00/bp.h) \
    $(wildcard include/config/mach/smdk2416.h) \
    $(wildcard include/config/mach/wbd111.h) \
    $(wildcard include/config/mach/mv2120.h) \
    $(wildcard include/config/mach/mx51/3ds.h) \
    $(wildcard include/config/mach/imx27lite.h) \
    $(wildcard include/config/mach/usb/a9260.h) \
    $(wildcard include/config/mach/usb/a9263.h) \
    $(wildcard include/config/mach/qil/a9260.h) \
    $(wildcard include/config/mach/kzm/arm11/01.h) \
    $(wildcard include/config/mach/nokia/n810/wimax.h) \
    $(wildcard include/config/mach/sapphire.h) \
    $(wildcard include/config/mach/stmp37xx.h) \
    $(wildcard include/config/mach/stmp378x.h) \
    $(wildcard include/config/mach/ezx/a780.h) \
    $(wildcard include/config/mach/ezx/e680.h) \
    $(wildcard include/config/mach/ezx/a1200.h) \
    $(wildcard include/config/mach/ezx/e6.h) \
    $(wildcard include/config/mach/ezx/e2.h) \
    $(wildcard include/config/mach/ezx/a910.h) \
    $(wildcard include/config/mach/edmini/v2.h) \
    $(wildcard include/config/mach/zipit2.h) \
    $(wildcard include/config/mach/omap3/pandora.h) \
    $(wildcard include/config/mach/mss2.h) \
    $(wildcard include/config/mach/lb88rc8480.h) \
    $(wildcard include/config/mach/mx25/3ds.h) \
    $(wildcard include/config/mach/omap3530/lv/som.h) \
    $(wildcard include/config/mach/davinci/da830/evm.h) \
    $(wildcard include/config/mach/at572d940hfeb.h) \
    $(wildcard include/config/mach/dove/db.h) \
    $(wildcard include/config/mach/overo.h) \
    $(wildcard include/config/mach/at2440evb.h) \
    $(wildcard include/config/mach/neocore926.h) \
    $(wildcard include/config/mach/wnr854t.h) \
    $(wildcard include/config/mach/rd88f5181l/ge.h) \
    $(wildcard include/config/mach/rd88f5181l/fxo.h) \
    $(wildcard include/config/mach/stamp9g20.h) \
    $(wildcard include/config/mach/smdkc100.h) \
    $(wildcard include/config/mach/tavorevb.h) \
    $(wildcard include/config/mach/saar.h) \
    $(wildcard include/config/mach/at91sam9m10g45ek.h) \
    $(wildcard include/config/mach/mxlads.h) \
    $(wildcard include/config/mach/linkstation/mini.h) \
    $(wildcard include/config/mach/afeb9260.h) \
    $(wildcard include/config/mach/imx27ipcam.h) \
    $(wildcard include/config/mach/rd88f6183ap/ge.h) \
    $(wildcard include/config/mach/realview/pba8.h) \
    $(wildcard include/config/mach/realview/pbx.h) \
    $(wildcard include/config/mach/micro9s.h) \
    $(wildcard include/config/mach/rut100.h) \
    $(wildcard include/config/mach/g3evm.h) \
    $(wildcard include/config/mach/w90p910evb.h) \
    $(wildcard include/config/mach/w90p950evb.h) \
    $(wildcard include/config/mach/w90n960evb.h) \
    $(wildcard include/config/mach/mv88f6281gtw/ge.h) \
    $(wildcard include/config/mach/ncp.h) \
    $(wildcard include/config/mach/davinci/dm365/evm.h) \
    $(wildcard include/config/mach/centro.h) \
    $(wildcard include/config/mach/nokia/rx51.h) \
    $(wildcard include/config/mach/omap/zoom2.h) \
    $(wildcard include/config/mach/cpuat9260.h) \
    $(wildcard include/config/mach/cpuimx27.h) \
    $(wildcard include/config/mach/acs5k.h) \
    $(wildcard include/config/mach/snapper/9260.h) \
    $(wildcard include/config/mach/dsm320.h) \
    $(wildcard include/config/mach/exeda.h) \
    $(wildcard include/config/mach/mini2440.h) \
    $(wildcard include/config/mach/colibri300.h) \
    $(wildcard include/config/mach/linkstation/ls/hgl.h) \
    $(wildcard include/config/mach/cpuat9g20.h) \
    $(wildcard include/config/mach/smdk6440.h) \
    $(wildcard include/config/mach/nas4220b.h) \
    $(wildcard include/config/mach/zylonite2.h) \
    $(wildcard include/config/mach/aspenite.h) \
    $(wildcard include/config/mach/ttc/dkb.h) \
    $(wildcard include/config/mach/pcm043.h) \
    $(wildcard include/config/mach/sheevaplug.h) \
    $(wildcard include/config/mach/avengers/lite.h) \
    $(wildcard include/config/mach/mx51/babbage.h) \
    $(wildcard include/config/mach/rd78x00/masa.h) \
    $(wildcard include/config/mach/dm355/leopard.h) \
    $(wildcard include/config/mach/ts219.h) \
    $(wildcard include/config/mach/pca100.h) \
    $(wildcard include/config/mach/davinci/da850/evm.h) \
    $(wildcard include/config/mach/at91sam9g10ek.h) \
    $(wildcard include/config/mach/omap/4430sdp.h) \
    $(wildcard include/config/mach/magx/zn5.h) \
    $(wildcard include/config/mach/btmavb101.h) \
    $(wildcard include/config/mach/btmawb101.h) \
    $(wildcard include/config/mach/omap3/torpedo.h) \
    $(wildcard include/config/mach/anw6410.h) \
    $(wildcard include/config/mach/imx27/visstrim/m10.h) \
    $(wildcard include/config/mach/portuxg20.h) \
    $(wildcard include/config/mach/smdkc110.h) \
    $(wildcard include/config/mach/omap3517evm.h) \
    $(wildcard include/config/mach/netspace/v2.h) \
    $(wildcard include/config/mach/netspace/max/v2.h) \
    $(wildcard include/config/mach/d2net/v2.h) \
    $(wildcard include/config/mach/net2big/v2.h) \
    $(wildcard include/config/mach/net5big/v2.h) \
    $(wildcard include/config/mach/inetspace/v2.h) \
    $(wildcard include/config/mach/at91sam9g45ekes.h) \
    $(wildcard include/config/mach/pc7302.h) \
    $(wildcard include/config/mach/spear600.h) \
    $(wildcard include/config/mach/spear300.h) \
    $(wildcard include/config/mach/lilly1131.h) \
    $(wildcard include/config/mach/hmt.h) \
    $(wildcard include/config/mach/vexpress.h) \
    $(wildcard include/config/mach/d2net.h) \
    $(wildcard include/config/mach/bigdisk.h) \
    $(wildcard include/config/mach/at91sam9g20ek/2mmc.h) \
    $(wildcard include/config/mach/bcmring.h) \
    $(wildcard include/config/mach/dp6xx.h) \
    $(wildcard include/config/mach/mahimahi.h) \
    $(wildcard include/config/mach/smdk6442.h) \
    $(wildcard include/config/mach/openrd/base.h) \
    $(wildcard include/config/mach/devkit8000.h) \
    $(wildcard include/config/mach/mx51/efikamx.h) \
    $(wildcard include/config/mach/cm/t35.h) \
    $(wildcard include/config/mach/net2big.h) \
    $(wildcard include/config/mach/igep0020.h) \
    $(wildcard include/config/mach/nuc932evb.h) \
    $(wildcard include/config/mach/openrd/client.h) \
    $(wildcard include/config/mach/u8500.h) \
    $(wildcard include/config/mach/mx51/efikasb.h) \
    $(wildcard include/config/mach/marvell/jasper.h) \
    $(wildcard include/config/mach/flint.h) \
    $(wildcard include/config/mach/tavorevb3.h) \
    $(wildcard include/config/mach/touchbook.h) \
    $(wildcard include/config/mach/raumfeld/rc.h) \
    $(wildcard include/config/mach/raumfeld/connector.h) \
    $(wildcard include/config/mach/raumfeld/speaker.h) \
    $(wildcard include/config/mach/tnetv107x.h) \
    $(wildcard include/config/mach/smdkv210.h) \
    $(wildcard include/config/mach/omap/zoom3.h) \
    $(wildcard include/config/mach/omap/3630sdp.h) \
    $(wildcard include/config/mach/smartq7.h) \
    $(wildcard include/config/mach/watson/efm/plugin.h) \
    $(wildcard include/config/mach/g4evm.h) \
    $(wildcard include/config/mach/omapl138/hawkboard.h) \
    $(wildcard include/config/mach/ts41x.h) \
    $(wildcard include/config/mach/phy3250.h) \
    $(wildcard include/config/mach/mini6410.h) \
    $(wildcard include/config/mach/mx28evk.h) \
    $(wildcard include/config/mach/smartq5.h) \
    $(wildcard include/config/mach/davinci/dm6467tevm.h) \
    $(wildcard include/config/mach/mxt/td60.h) \
    $(wildcard include/config/mach/riot/bei2.h) \
    $(wildcard include/config/mach/riot/x37.h) \
    $(wildcard include/config/mach/capc7117.h) \
    $(wildcard include/config/mach/icontrol.h) \
    $(wildcard include/config/mach/qsd8x50a/st1/5.h) \
    $(wildcard include/config/mach/mx23evk.h) \
    $(wildcard include/config/mach/ap4evb.h) \
    $(wildcard include/config/mach/mityomapl138.h) \
    $(wildcard include/config/mach/guruplug.h) \
    $(wildcard include/config/mach/spear310.h) \
    $(wildcard include/config/mach/spear320.h) \
    $(wildcard include/config/mach/aquila.h) \
    $(wildcard include/config/mach/esata/sheevaplug.h) \
    $(wildcard include/config/mach/msm7x30/surf.h) \
    $(wildcard include/config/mach/ea2478devkit.h) \
    $(wildcard include/config/mach/terastation/wxl.h) \
    $(wildcard include/config/mach/msm7x25/surf.h) \
    $(wildcard include/config/mach/msm7x25/ffa.h) \
    $(wildcard include/config/mach/msm7x27/surf.h) \
    $(wildcard include/config/mach/msm7x27/ffa.h) \
    $(wildcard include/config/mach/msm7x30/ffa.h) \
    $(wildcard include/config/mach/qsd8x50/surf.h) \
    $(wildcard include/config/mach/mx53/evk.h) \
    $(wildcard include/config/mach/igep0030.h) \
    $(wildcard include/config/mach/sbc3530.h) \
    $(wildcard include/config/mach/saarb.h) \
    $(wildcard include/config/mach/harmony.h) \
    $(wildcard include/config/mach/msm7x30/fluid.h) \
    $(wildcard include/config/mach/cm/t3517.h) \
    $(wildcard include/config/mach/wbd222.h) \
    $(wildcard include/config/mach/mt65xx.h) \
    $(wildcard include/config/mach/msm8x60/surf.h) \
    $(wildcard include/config/mach/msm8x60/sim.h) \
    $(wildcard include/config/mach/tcc8000/sdk.h) \
    $(wildcard include/config/mach/nanos.h) \
    $(wildcard include/config/mach/stamp9g45.h) \
    $(wildcard include/config/mach/cns3420vb.h) \
    $(wildcard include/config/mach/omap4/panda.h) \
    $(wildcard include/config/mach/ti8168evm.h) \
    $(wildcard include/config/mach/teton/bga.h) \
    $(wildcard include/config/mach/eukrea/cpuimx25.h) \
    $(wildcard include/config/mach/eukrea/cpuimx35.h) \
    $(wildcard include/config/mach/eukrea/cpuimx51sd.h) \
    $(wildcard include/config/mach/eukrea/cpuimx51.h) \
    $(wildcard include/config/mach/smdkc210.h) \
    $(wildcard include/config/mach/omap3/braillo.h) \
    $(wildcard include/config/mach/spyplug.h) \
    $(wildcard include/config/mach/ginger.h) \
    $(wildcard include/config/mach/tny/t3530.h) \
    $(wildcard include/config/mach/pca102.h) \
    $(wildcard include/config/mach/spade.h) \
    $(wildcard include/config/mach/mxc25/topaz.h) \
    $(wildcard include/config/mach/t5325.h) \
    $(wildcard include/config/mach/gw2361.h) \
    $(wildcard include/config/mach/elog.h) \
    $(wildcard include/config/mach/income.h) \
    $(wildcard include/config/mach/bcm589x.h) \
    $(wildcard include/config/mach/etna.h) \
    $(wildcard include/config/mach/hawks.h) \
    $(wildcard include/config/mach/meson.h) \
    $(wildcard include/config/mach/xsbase255.h) \
    $(wildcard include/config/mach/pvm2030.h) \
    $(wildcard include/config/mach/mioa502.h) \
    $(wildcard include/config/mach/vvbox/sdorig2.h) \
    $(wildcard include/config/mach/vvbox/sdlite2.h) \
    $(wildcard include/config/mach/vvbox/sdpro4.h) \
    $(wildcard include/config/mach/htc/spv/m700.h) \
    $(wildcard include/config/mach/mx257sx.h) \
    $(wildcard include/config/mach/goni.h) \
    $(wildcard include/config/mach/msm8x55/svlte/ffa.h) \
    $(wildcard include/config/mach/msm8x55/svlte/surf.h) \
    $(wildcard include/config/mach/quickstep.h) \
    $(wildcard include/config/mach/dmw96.h) \
    $(wildcard include/config/mach/hammerhead.h) \
    $(wildcard include/config/mach/trident.h) \
    $(wildcard include/config/mach/lightning.h) \
    $(wildcard include/config/mach/iconnect.h) \
    $(wildcard include/config/mach/autobot.h) \
    $(wildcard include/config/mach/coconut.h) \
    $(wildcard include/config/mach/durian.h) \
    $(wildcard include/config/mach/cayenne.h) \
    $(wildcard include/config/mach/fuji.h) \
    $(wildcard include/config/mach/synology/6282.h) \
    $(wildcard include/config/mach/em1sy.h) \
    $(wildcard include/config/mach/m502.h) \
    $(wildcard include/config/mach/matrix518.h) \
    $(wildcard include/config/mach/tiny/gurnard.h) \
    $(wildcard include/config/mach/spear1310.h) \
    $(wildcard include/config/mach/bv07.h) \
    $(wildcard include/config/mach/mxt/td61.h) \
    $(wildcard include/config/mach/openrd/ultimate.h) \
    $(wildcard include/config/mach/devixp.h) \
    $(wildcard include/config/mach/miccpt.h) \
    $(wildcard include/config/mach/mic256.h) \
    $(wildcard include/config/mach/as1167.h) \
    $(wildcard include/config/mach/omap3/ibiza.h) \
    $(wildcard include/config/mach/u5500.h) \
    $(wildcard include/config/mach/davinci/picto.h) \
    $(wildcard include/config/mach/mecha.h) \
    $(wildcard include/config/mach/bubba3.h) \
    $(wildcard include/config/mach/pupitre.h) \
    $(wildcard include/config/mach/tegra/vogue.h) \
    $(wildcard include/config/mach/tegra/e1165.h) \
    $(wildcard include/config/mach/simplenet.h) \
    $(wildcard include/config/mach/ec4350tbm.h) \
    $(wildcard include/config/mach/pec/tc.h) \
    $(wildcard include/config/mach/pec/hc2.h) \
    $(wildcard include/config/mach/esl/mobilis/a.h) \
    $(wildcard include/config/mach/esl/mobilis/b.h) \
    $(wildcard include/config/mach/esl/wave/a.h) \
    $(wildcard include/config/mach/esl/wave/b.h) \
    $(wildcard include/config/mach/unisense/mmm.h) \
    $(wildcard include/config/mach/blueshark.h) \
    $(wildcard include/config/mach/e10.h) \
    $(wildcard include/config/mach/app3k/robin.h) \
    $(wildcard include/config/mach/pov15hd.h) \
    $(wildcard include/config/mach/stella.h) \
    $(wildcard include/config/mach/linkstation/lschl.h) \
    $(wildcard include/config/mach/netwalker.h) \
    $(wildcard include/config/mach/acsx106.h) \
    $(wildcard include/config/mach/atlas5/c1.h) \
    $(wildcard include/config/mach/nsb3ast.h) \
    $(wildcard include/config/mach/gnet/slc.h) \
    $(wildcard include/config/mach/af4000.h) \
    $(wildcard include/config/mach/ark9431.h) \
    $(wildcard include/config/mach/fs/s5pc100.h) \
    $(wildcard include/config/mach/omap3505nova8.h) \
    $(wildcard include/config/mach/omap3621/edp1.h) \
    $(wildcard include/config/mach/oratisaes.h) \
    $(wildcard include/config/mach/smdkv310.h) \
    $(wildcard include/config/mach/siemens/l0.h) \
    $(wildcard include/config/mach/ventana.h) \
    $(wildcard include/config/mach/wm8505/7in/netbook.h) \
    $(wildcard include/config/mach/ec4350sdb.h) \
    $(wildcard include/config/mach/mimas.h) \
    $(wildcard include/config/mach/titan.h) \
    $(wildcard include/config/mach/craneboard.h) \
    $(wildcard include/config/mach/es2440.h) \
    $(wildcard include/config/mach/najay/a9263.h) \
    $(wildcard include/config/mach/htctornado.h) \
    $(wildcard include/config/mach/dimm/mx257.h) \
    $(wildcard include/config/mach/jigen.h) \
    $(wildcard include/config/mach/smdk6450.h) \
    $(wildcard include/config/mach/meno/qng.h) \
    $(wildcard include/config/mach/ns2416.h) \
    $(wildcard include/config/mach/rpc353.h) \
    $(wildcard include/config/mach/tq6410.h) \
    $(wildcard include/config/mach/sky6410.h) \
    $(wildcard include/config/mach/dynasty.h) \
    $(wildcard include/config/mach/vivo.h) \
    $(wildcard include/config/mach/bury/bl7582.h) \
    $(wildcard include/config/mach/bury/bps5270.h) \
    $(wildcard include/config/mach/basi.h) \
    $(wildcard include/config/mach/tn200.h) \
    $(wildcard include/config/mach/c2mmi.h) \
    $(wildcard include/config/mach/meson/6236m.h) \
    $(wildcard include/config/mach/meson/8626m.h) \
    $(wildcard include/config/mach/tube.h) \
    $(wildcard include/config/mach/messina.h) \
    $(wildcard include/config/mach/mx50/arm2.h) \
    $(wildcard include/config/mach/cetus9263.h) \
    $(wildcard include/config/mach/brownstone.h) \
    $(wildcard include/config/mach/vmx25.h) \
    $(wildcard include/config/mach/vmx51.h) \
    $(wildcard include/config/mach/abacus.h) \
    $(wildcard include/config/mach/cm4745.h) \
    $(wildcard include/config/mach/oratislink.h) \
    $(wildcard include/config/mach/davinci/dm365/dvr.h) \
    $(wildcard include/config/mach/netviz.h) \
    $(wildcard include/config/mach/flexibity.h) \
    $(wildcard include/config/mach/wlan/computer.h) \
    $(wildcard include/config/mach/lpc24xx.h) \
    $(wildcard include/config/mach/spica.h) \
    $(wildcard include/config/mach/gpsdisplay.h) \
    $(wildcard include/config/mach/bipnet.h) \
    $(wildcard include/config/mach/overo/ctu/inertial.h) \
    $(wildcard include/config/mach/davinci/dm355/mmm.h) \
    $(wildcard include/config/mach/pc9260/v2.h) \
    $(wildcard include/config/mach/ptx7545.h) \
    $(wildcard include/config/mach/tm/efdc.h) \
    $(wildcard include/config/mach/omap3/waldo1.h) \
    $(wildcard include/config/mach/flyer.h) \
    $(wildcard include/config/mach/tornado3240.h) \
    $(wildcard include/config/mach/soli/01.h) \
    $(wildcard include/config/mach/omapl138/europalc.h) \
    $(wildcard include/config/mach/helios/v1.h) \
    $(wildcard include/config/mach/netspace/lite/v2.h) \
    $(wildcard include/config/mach/ssc.h) \
    $(wildcard include/config/mach/premierwave/en.h) \
    $(wildcard include/config/mach/wasabi.h) \
    $(wildcard include/config/mach/mx50/rdp.h) \
    $(wildcard include/config/mach/universal/c210.h) \
    $(wildcard include/config/mach/real6410.h) \
    $(wildcard include/config/mach/spx/sakura.h) \
    $(wildcard include/config/mach/ij3k/2440.h) \
    $(wildcard include/config/mach/omap3/bc10.h) \
    $(wildcard include/config/mach/thebe.h) \
    $(wildcard include/config/mach/rv082.h) \
    $(wildcard include/config/mach/armlguest.h) \
    $(wildcard include/config/mach/tjinc1000.h) \
    $(wildcard include/config/mach/dockstar.h) \
    $(wildcard include/config/mach/ax8008.h) \
    $(wildcard include/config/mach/gnet/sgce.h) \
    $(wildcard include/config/mach/pxwnas/500/1000.h) \
    $(wildcard include/config/mach/ea20.h) \
    $(wildcard include/config/mach/awm2.h) \
    $(wildcard include/config/mach/ti8148evm.h) \
    $(wildcard include/config/mach/seaboard.h) \
    $(wildcard include/config/mach/linkstation/chlv2.h) \
    $(wildcard include/config/mach/tera/pro2/rack.h) \
    $(wildcard include/config/mach/rubys.h) \
    $(wildcard include/config/mach/aquarius.h) \
    $(wildcard include/config/mach/mx53/ard.h) \
    $(wildcard include/config/mach/mx53/smd.h) \
    $(wildcard include/config/mach/lswxl.h) \
    $(wildcard include/config/mach/dove/avng/v3.h) \
    $(wildcard include/config/mach/sdi/ess/9263.h) \
    $(wildcard include/config/mach/jocpu550.h) \
    $(wildcard include/config/mach/msm8x60/rumi3.h) \
    $(wildcard include/config/mach/msm8x60/ffa.h) \
    $(wildcard include/config/mach/yanomami.h) \
    $(wildcard include/config/mach/gta04.h) \
    $(wildcard include/config/mach/cm/a510.h) \
    $(wildcard include/config/mach/omap3/rfs200.h) \
    $(wildcard include/config/mach/kx33xx.h) \
    $(wildcard include/config/mach/ptx7510.h) \
    $(wildcard include/config/mach/top9000.h) \
    $(wildcard include/config/mach/teenote.h) \
    $(wildcard include/config/mach/ts3.h) \
    $(wildcard include/config/mach/a0.h) \
    $(wildcard include/config/mach/fsm9xxx/surf.h) \
    $(wildcard include/config/mach/fsm9xxx/ffa.h) \
    $(wildcard include/config/mach/frrhwcdma60w.h) \
    $(wildcard include/config/mach/remus.h) \
    $(wildcard include/config/mach/at91cap7xdk.h) \
    $(wildcard include/config/mach/at91cap7stk.h) \
    $(wildcard include/config/mach/kt/sbc/sam9/1.h) \
    $(wildcard include/config/mach/armada/xp/db.h) \
    $(wildcard include/config/mach/spdm.h) \
    $(wildcard include/config/mach/gtib.h) \
    $(wildcard include/config/mach/dgm3240.h) \
    $(wildcard include/config/mach/htcmega.h) \
    $(wildcard include/config/mach/tricorder.h) \
    $(wildcard include/config/mach/tx28.h) \
    $(wildcard include/config/mach/bstbrd.h) \
    $(wildcard include/config/mach/pwb3090.h) \
    $(wildcard include/config/mach/idea6410.h) \
    $(wildcard include/config/mach/qbc9263.h) \
    $(wildcard include/config/mach/borabora.h) \
    $(wildcard include/config/mach/valdez.h) \
    $(wildcard include/config/mach/ls9g20.h) \
    $(wildcard include/config/mach/mios/v1.h) \
    $(wildcard include/config/mach/s5pc110/crespo.h) \
    $(wildcard include/config/mach/controltek9g20.h) \
    $(wildcard include/config/mach/tin307.h) \
    $(wildcard include/config/mach/tin510.h) \
    $(wildcard include/config/mach/bluecheese.h) \
    $(wildcard include/config/mach/tem3x30.h) \
    $(wildcard include/config/mach/harvest/desoto.h) \
    $(wildcard include/config/mach/msm8x60/qrdc.h) \
    $(wildcard include/config/mach/spear900.h) \
    $(wildcard include/config/mach/pcontrol/g20.h) \
    $(wildcard include/config/mach/rdstor.h) \
    $(wildcard include/config/mach/usdloader.h) \
    $(wildcard include/config/mach/tsoploader.h) \
    $(wildcard include/config/mach/kronos.h) \
    $(wildcard include/config/mach/ffcore.h) \
    $(wildcard include/config/mach/mone.h) \
    $(wildcard include/config/mach/unit2s.h) \
    $(wildcard include/config/mach/acer/a5.h) \
    $(wildcard include/config/mach/etherpro/isp.h) \
    $(wildcard include/config/mach/stretchs7000.h) \
    $(wildcard include/config/mach/p87/smartsim.h) \
    $(wildcard include/config/mach/tulip.h) \
    $(wildcard include/config/mach/sunflower.h) \
    $(wildcard include/config/mach/rib.h) \
    $(wildcard include/config/mach/clod.h) \
    $(wildcard include/config/mach/rump.h) \
    $(wildcard include/config/mach/tenderloin.h) \
    $(wildcard include/config/mach/shortloin.h) \
    $(wildcard include/config/mach/antares.h) \
    $(wildcard include/config/mach/wb40n.h) \
    $(wildcard include/config/mach/herring.h) \
    $(wildcard include/config/mach/naxy400.h) \
    $(wildcard include/config/mach/naxy1200.h) \
    $(wildcard include/config/mach/vpr200.h) \
    $(wildcard include/config/mach/bug20.h) \
    $(wildcard include/config/mach/goflexnet.h) \
    $(wildcard include/config/mach/torbreck.h) \
    $(wildcard include/config/mach/saarb/mg1.h) \
    $(wildcard include/config/mach/callisto.h) \
    $(wildcard include/config/mach/multhsu.h) \
    $(wildcard include/config/mach/saluda.h) \
    $(wildcard include/config/mach/pemp/omap3/apollo.h) \
    $(wildcard include/config/mach/vc0718.h) \
    $(wildcard include/config/mach/mvblx.h) \
    $(wildcard include/config/mach/inhand/apeiron.h) \
    $(wildcard include/config/mach/inhand/fury.h) \
    $(wildcard include/config/mach/inhand/siren.h) \
    $(wildcard include/config/mach/hdnvp.h) \
    $(wildcard include/config/mach/softwinner.h) \
    $(wildcard include/config/mach/prima2/evb.h) \
    $(wildcard include/config/mach/nas6210.h) \
    $(wildcard include/config/mach/unisdev.h) \
    $(wildcard include/config/mach/sbca11.h) \
    $(wildcard include/config/mach/saga.h) \
    $(wildcard include/config/mach/ns/k330.h) \
    $(wildcard include/config/mach/tanna.h) \
    $(wildcard include/config/mach/imate8502.h) \
    $(wildcard include/config/mach/aspen.h) \
    $(wildcard include/config/mach/daintree/cwac.h) \
    $(wildcard include/config/mach/zmx25.h) \
    $(wildcard include/config/mach/maple1.h) \
    $(wildcard include/config/mach/qsd8x72/surf.h) \
    $(wildcard include/config/mach/qsd8x72/ffa.h) \
    $(wildcard include/config/mach/abilene.h) \
    $(wildcard include/config/mach/eigen/ttr.h) \
    $(wildcard include/config/mach/iomega/ix2/200.h) \
    $(wildcard include/config/mach/coretec/vcx7400.h) \
    $(wildcard include/config/mach/santiago.h) \
    $(wildcard include/config/mach/mx257sol.h) \
    $(wildcard include/config/mach/strasbourg.h) \
    $(wildcard include/config/mach/msm8x60/fluid.h) \
    $(wildcard include/config/mach/smartqv5.h) \
    $(wildcard include/config/mach/smartqv3.h) \
    $(wildcard include/config/mach/smartqv7.h) \
    $(wildcard include/config/mach/paz00.h) \
    $(wildcard include/config/mach/acmenetusfoxg20.h) \
    $(wildcard include/config/mach/fwbd/0404.h) \
    $(wildcard include/config/mach/hdgu.h) \
    $(wildcard include/config/mach/pyramid.h) \
    $(wildcard include/config/mach/epiphan.h) \
    $(wildcard include/config/mach/omap/bender.h) \
    $(wildcard include/config/mach/gurnard.h) \
    $(wildcard include/config/mach/gtl/it5100.h) \
    $(wildcard include/config/mach/bcm2708.h) \
    $(wildcard include/config/mach/mx51/ggc.h) \
    $(wildcard include/config/mach/sharespace.h) \
    $(wildcard include/config/mach/haba/knx/explorer.h) \
    $(wildcard include/config/mach/simtec/kirkmod.h) \
    $(wildcard include/config/mach/crux.h) \
    $(wildcard include/config/mach/mx51/bravo.h) \
    $(wildcard include/config/mach/charon.h) \
    $(wildcard include/config/mach/picocom3.h) \
    $(wildcard include/config/mach/picocom4.h) \
    $(wildcard include/config/mach/serrano.h) \
    $(wildcard include/config/mach/doubleshot.h) \
    $(wildcard include/config/mach/evsy.h) \
    $(wildcard include/config/mach/huashan.h) \
    $(wildcard include/config/mach/lausanne.h) \
    $(wildcard include/config/mach/emerald.h) \
    $(wildcard include/config/mach/tqma35.h) \
    $(wildcard include/config/mach/marvel.h) \
    $(wildcard include/config/mach/manuae.h) \
    $(wildcard include/config/mach/chacha.h) \
    $(wildcard include/config/mach/lemon.h) \
    $(wildcard include/config/mach/csc.h) \
    $(wildcard include/config/mach/gira/knxip/router.h) \
    $(wildcard include/config/mach/t20.h) \
    $(wildcard include/config/mach/hdmini.h) \
    $(wildcard include/config/mach/sciphone/g2.h) \
    $(wildcard include/config/mach/express.h) \
    $(wildcard include/config/mach/express/kt.h) \
    $(wildcard include/config/mach/maximasp.h) \
    $(wildcard include/config/mach/nitrogen/imx51.h) \
    $(wildcard include/config/mach/nitrogen/imx53.h) \
    $(wildcard include/config/mach/sunfire.h) \
    $(wildcard include/config/mach/arowana.h) \
    $(wildcard include/config/mach/tegra/daytona.h) \
    $(wildcard include/config/mach/tegra/swordfish.h) \
    $(wildcard include/config/mach/edison.h) \
    $(wildcard include/config/mach/svp8500v1.h) \
    $(wildcard include/config/mach/svp8500v2.h) \
    $(wildcard include/config/mach/svp5500.h) \
    $(wildcard include/config/mach/b5500.h) \
    $(wildcard include/config/mach/s5500.h) \
    $(wildcard include/config/mach/icon.h) \
    $(wildcard include/config/mach/elephant.h) \
    $(wildcard include/config/mach/shooter.h) \
    $(wildcard include/config/mach/spade/lte.h) \
    $(wildcard include/config/mach/philhwani.h) \
    $(wildcard include/config/mach/gsncomm.h) \
    $(wildcard include/config/mach/strasbourg/a2.h) \
    $(wildcard include/config/mach/mmm.h) \
    $(wildcard include/config/mach/davinci/dm365/bv.h) \
    $(wildcard include/config/mach/ag5evm.h) \
    $(wildcard include/config/mach/sc575plc.h) \
    $(wildcard include/config/mach/sc575ipc.h) \
    $(wildcard include/config/mach/omap3/tdm3730.h) \
    $(wildcard include/config/mach/top9000/eval.h) \
    $(wildcard include/config/mach/top9000/su.h) \
    $(wildcard include/config/mach/utm300.h) \
    $(wildcard include/config/mach/tsunagi.h) \
    $(wildcard include/config/mach/ts75xx.h) \
    $(wildcard include/config/mach/ts47xx.h) \
    $(wildcard include/config/mach/da850/k5.h) \
    $(wildcard include/config/mach/ax502.h) \
    $(wildcard include/config/mach/igep0032.h) \
    $(wildcard include/config/mach/antero.h) \
    $(wildcard include/config/mach/synergy.h) \
    $(wildcard include/config/mach/ics/if/voip.h) \
    $(wildcard include/config/mach/wlf/cragg/6410.h) \
    $(wildcard include/config/mach/punica.h) \
    $(wildcard include/config/mach/trimslice.h) \
    $(wildcard include/config/mach/mx27/wmultra.h) \
    $(wildcard include/config/mach/mackerel.h) \
    $(wildcard include/config/mach/fa9x27.h) \
    $(wildcard include/config/mach/ns2816tb.h) \
    $(wildcard include/config/mach/ns2816/ntpad.h) \
    $(wildcard include/config/mach/ns2816/ntnb.h) \
    $(wildcard include/config/mach/kaen.h) \
    $(wildcard include/config/mach/nv1000.h) \
    $(wildcard include/config/mach/nuc950ts.h) \
    $(wildcard include/config/mach/nokia/rm680.h) \
    $(wildcard include/config/mach/ast2200.h) \
    $(wildcard include/config/mach/lead.h) \
    $(wildcard include/config/mach/unino1.h) \
    $(wildcard include/config/mach/greeco.h) \
    $(wildcard include/config/mach/verdi.h) \
    $(wildcard include/config/mach/dm6446/adbox.h) \
    $(wildcard include/config/mach/quad/salsa.h) \
    $(wildcard include/config/mach/abb/gma/1/1.h) \
    $(wildcard include/config/mach/svcid.h) \
    $(wildcard include/config/mach/msm8960/sim.h) \
    $(wildcard include/config/mach/msm8960/rumi3.h) \
    $(wildcard include/config/mach/icon/g.h) \
    $(wildcard include/config/mach/mb3.h) \
    $(wildcard include/config/mach/gsia18s.h) \
    $(wildcard include/config/mach/pivicc.h) \
    $(wildcard include/config/mach/pcm048.h) \
    $(wildcard include/config/mach/dds.h) \
    $(wildcard include/config/mach/chalten/xa1.h) \
    $(wildcard include/config/mach/ts48xx.h) \
    $(wildcard include/config/mach/tonga2/tfttimer.h) \
    $(wildcard include/config/mach/whistler.h) \
    $(wildcard include/config/mach/asl/phoenix.h) \
    $(wildcard include/config/mach/at91sam9263otlite.h) \
    $(wildcard include/config/mach/ddplug.h) \
    $(wildcard include/config/mach/d2plug.h) \
    $(wildcard include/config/mach/kzm9d.h) \
    $(wildcard include/config/mach/verdi/lte.h) \
    $(wildcard include/config/mach/nanozoom.h) \
    $(wildcard include/config/mach/dm3730/som/lv.h) \
    $(wildcard include/config/mach/dm3730/torpedo.h) \
    $(wildcard include/config/mach/anchovy.h) \
    $(wildcard include/config/mach/re2rev20.h) \
    $(wildcard include/config/mach/re2rev21.h) \
    $(wildcard include/config/mach/cns21xx.h) \
    $(wildcard include/config/mach/rider.h) \
    $(wildcard include/config/mach/nsk330.h) \
    $(wildcard include/config/mach/cns2133evb.h) \
    $(wildcard include/config/mach/z3/816x/mod.h) \
    $(wildcard include/config/mach/z3/814x/mod.h) \
    $(wildcard include/config/mach/beect.h) \
    $(wildcard include/config/mach/dma/thunderbug.h) \
    $(wildcard include/config/mach/omn/at91sam9g20.h) \
    $(wildcard include/config/mach/mx25/e2s/uc.h) \
    $(wildcard include/config/mach/mione.h) \
    $(wildcard include/config/mach/top9000/tcu.h) \
    $(wildcard include/config/mach/top9000/bsl.h) \
    $(wildcard include/config/mach/kingdom.h) \
    $(wildcard include/config/mach/armadillo460.h) \
    $(wildcard include/config/mach/lq2.h) \
    $(wildcard include/config/mach/sweda/tms2.h) \
    $(wildcard include/config/mach/mx53/loco.h) \
    $(wildcard include/config/mach/acer/a8.h) \
    $(wildcard include/config/mach/acer/gauguin.h) \
    $(wildcard include/config/mach/guppy.h) \
    $(wildcard include/config/mach/mx61/ard.h) \
    $(wildcard include/config/mach/tx53.h) \
    $(wildcard include/config/mach/omapl138/case/a3.h) \
    $(wildcard include/config/mach/uemd.h) \
    $(wildcard include/config/mach/ccwmx51mut.h) \
    $(wildcard include/config/mach/rockhopper.h) \
    $(wildcard include/config/mach/nookcolor.h) \
    $(wildcard include/config/mach/hkdkc100.h) \
    $(wildcard include/config/mach/ts42xx.h) \
    $(wildcard include/config/mach/aebl.h) \
    $(wildcard include/config/mach/wario.h) \
    $(wildcard include/config/mach/gfs/spm.h) \
    $(wildcard include/config/mach/cm/t3730.h) \
    $(wildcard include/config/mach/isc3.h) \
    $(wildcard include/config/mach/rascal.h) \
    $(wildcard include/config/mach/hrefv60.h) \
    $(wildcard include/config/mach/tpt/2/0.h) \
    $(wildcard include/config/mach/splendor.h) \
    $(wildcard include/config/mach/msm8x60/qt.h) \
    $(wildcard include/config/mach/htc/hd/mini.h) \
    $(wildcard include/config/mach/athene.h) \
    $(wildcard include/config/mach/deep/r/ek/1.h) \
    $(wildcard include/config/mach/vivow/ct.h) \
    $(wildcard include/config/mach/nery/1000.h) \
    $(wildcard include/config/mach/rfl109145/ssrv.h) \
    $(wildcard include/config/mach/nmh.h) \
    $(wildcard include/config/mach/wn802t.h) \
    $(wildcard include/config/mach/dragonet.h) \
    $(wildcard include/config/mach/at91sam9263desk16l.h) \
    $(wildcard include/config/mach/bcmhana/sv.h) \
    $(wildcard include/config/mach/bcmhana/tablet.h) \
    $(wildcard include/config/mach/koi.h) \
    $(wildcard include/config/mach/ts4800.h) \
    $(wildcard include/config/mach/tqma9263.h) \
    $(wildcard include/config/mach/holiday.h) \
    $(wildcard include/config/mach/dma6410.h) \
    $(wildcard include/config/mach/pcats/overlay.h) \
    $(wildcard include/config/mach/hwgw6410.h) \
    $(wildcard include/config/mach/shenzhou.h) \
    $(wildcard include/config/mach/cwme9210.h) \
    $(wildcard include/config/mach/cwme9210js.h) \
    $(wildcard include/config/mach/pgs/sitara.h) \
    $(wildcard include/config/mach/colibri/tegra2.h) \
    $(wildcard include/config/mach/w21.h) \
    $(wildcard include/config/mach/polysat1.h) \
    $(wildcard include/config/mach/dataway.h) \
    $(wildcard include/config/mach/cobral138.h) \
    $(wildcard include/config/mach/roverpcs8.h) \
    $(wildcard include/config/mach/marvelc.h) \
    $(wildcard include/config/mach/navefihid.h) \
    $(wildcard include/config/mach/dm365/cv100.h) \
    $(wildcard include/config/mach/able.h) \
    $(wildcard include/config/mach/legacy.h) \
    $(wildcard include/config/mach/icong.h) \
    $(wildcard include/config/mach/rover/g8.h) \
    $(wildcard include/config/mach/t5388p.h) \
    $(wildcard include/config/mach/dingo.h) \
    $(wildcard include/config/mach/goflexhome.h) \
    $(wildcard include/config/mach/lanreadyfn511.h) \
    $(wildcard include/config/mach/omap3/baia.h) \
    $(wildcard include/config/mach/omap3smartdisplay.h) \
    $(wildcard include/config/mach/xilinx.h) \
    $(wildcard include/config/mach/a2f.h) \
    $(wildcard include/config/mach/sky25.h) \
    $(wildcard include/config/mach/ccmx53.h) \
    $(wildcard include/config/mach/ccmx53js.h) \
    $(wildcard include/config/mach/ccwmx53.h) \
    $(wildcard include/config/mach/ccwmx53js.h) \
    $(wildcard include/config/mach/frisms.h) \
    $(wildcard include/config/mach/msm7x27a/ffa.h) \
    $(wildcard include/config/mach/msm7x27a/surf.h) \
    $(wildcard include/config/mach/msm7x27a/rumi3.h) \
    $(wildcard include/config/mach/dimmsam9g20.h) \
    $(wildcard include/config/mach/dimm/imx28.h) \
    $(wildcard include/config/mach/amk/a4.h) \
    $(wildcard include/config/mach/gnet/sgme.h) \
    $(wildcard include/config/mach/shooter/u.h) \
    $(wildcard include/config/mach/vmx53.h) \
    $(wildcard include/config/mach/rhino.h) \
    $(wildcard include/config/mach/armlex4210.h) \
    $(wildcard include/config/mach/swarcoextmodem.h) \
    $(wildcard include/config/mach/snowball.h) \
    $(wildcard include/config/mach/pcm049.h) \
    $(wildcard include/config/mach/vigor.h) \
    $(wildcard include/config/mach/oslo/amundsen.h) \
    $(wildcard include/config/mach/gsl/diamond.h) \
    $(wildcard include/config/mach/cv2201.h) \
    $(wildcard include/config/mach/cv2202.h) \
    $(wildcard include/config/mach/cv2203.h) \
    $(wildcard include/config/mach/vit/ibox.h) \
    $(wildcard include/config/mach/dm6441/esp.h) \
    $(wildcard include/config/mach/at91sam9x5ek.h) \
    $(wildcard include/config/mach/libra.h) \
    $(wildcard include/config/mach/easycrrh.h) \
    $(wildcard include/config/mach/tripel.h) \
    $(wildcard include/config/mach/endian/mini.h) \
    $(wildcard include/config/mach/xilinx/ep107.h) \
    $(wildcard include/config/mach/nuri.h) \
    $(wildcard include/config/mach/janus.h) \
    $(wildcard include/config/mach/ddnas.h) \
    $(wildcard include/config/mach/tag.h) \
    $(wildcard include/config/mach/tagw.h) \
    $(wildcard include/config/mach/nitrogen/vm/imx51.h) \
    $(wildcard include/config/mach/viprinet.h) \
    $(wildcard include/config/mach/bockw.h) \
    $(wildcard include/config/mach/eva2000.h) \
    $(wildcard include/config/mach/steelyard.h) \
    $(wildcard include/config/mach/mach/sdh001.h) \
    $(wildcard include/config/mach/nsslsboard.h) \
    $(wildcard include/config/mach/geneva/b5.h) \
    $(wildcard include/config/mach/spear1340.h) \
    $(wildcard include/config/mach/rexmas.h) \
    $(wildcard include/config/mach/msm8960/cdp.h) \
    $(wildcard include/config/mach/msm8960/mdp.h) \
    $(wildcard include/config/mach/msm8960/fluid.h) \
    $(wildcard include/config/mach/msm8960/apq.h) \
    $(wildcard include/config/mach/helios/v2.h) \
    $(wildcard include/config/mach/mif10p.h) \
    $(wildcard include/config/mach/iam28.h) \
    $(wildcard include/config/mach/picasso.h) \
    $(wildcard include/config/mach/mr301a.h) \
    $(wildcard include/config/mach/notle.h) \
    $(wildcard include/config/mach/eelx2.h) \
    $(wildcard include/config/mach/moon.h) \
    $(wildcard include/config/mach/ruby.h) \
    $(wildcard include/config/mach/goldengate.h) \
    $(wildcard include/config/mach/ctbu/gen2.h) \
    $(wildcard include/config/mach/kmp/am17/01.h) \
    $(wildcard include/config/mach/wtplug.h) \
    $(wildcard include/config/mach/mx27su2.h) \
    $(wildcard include/config/mach/nb31.h) \
    $(wildcard include/config/mach/hjsdu.h) \
    $(wildcard include/config/mach/td3/rev1.h) \
    $(wildcard include/config/mach/eag/ci4000.h) \
    $(wildcard include/config/mach/net5big/nand/v2.h) \
    $(wildcard include/config/mach/cpx2.h) \
    $(wildcard include/config/mach/net2big/nand/v2.h) \
    $(wildcard include/config/mach/ecuv5.h) \
    $(wildcard include/config/mach/hsgx6d.h) \
    $(wildcard include/config/mach/dawad7.h) \
    $(wildcard include/config/mach/sam9repeater.h) \
    $(wildcard include/config/mach/gt/i5700.h) \
    $(wildcard include/config/mach/ctera/plug/c2.h) \
    $(wildcard include/config/mach/marvelct.h) \
    $(wildcard include/config/mach/ag11005.h) \
    $(wildcard include/config/mach/vangogh.h) \
    $(wildcard include/config/mach/matrix505.h) \
    $(wildcard include/config/mach/oce/nigma.h) \
    $(wildcard include/config/mach/t55.h) \
    $(wildcard include/config/mach/bio3k.h) \
    $(wildcard include/config/mach/expressct.h) \
    $(wildcard include/config/mach/cardhu.h) \
    $(wildcard include/config/mach/aruba.h) \
    $(wildcard include/config/mach/bonaire.h) \
    $(wildcard include/config/mach/nuc700evb.h) \
    $(wildcard include/config/mach/nuc710evb.h) \
    $(wildcard include/config/mach/nuc740evb.h) \
    $(wildcard include/config/mach/nuc745evb.h) \
    $(wildcard include/config/mach/transcede.h) \
    $(wildcard include/config/mach/mora.h) \
    $(wildcard include/config/mach/nda/evm.h) \
    $(wildcard include/config/mach/timu.h) \
    $(wildcard include/config/mach/expressh.h) \
    $(wildcard include/config/mach/veridis/a300.h) \
    $(wildcard include/config/mach/dm368/leopard.h) \
    $(wildcard include/config/mach/omap/mcop.h) \
    $(wildcard include/config/mach/tritip.h) \
    $(wildcard include/config/mach/sm1k.h) \
    $(wildcard include/config/mach/monch.h) \
    $(wildcard include/config/mach/curacao.h) \
    $(wildcard include/config/mach/origen.h) \
    $(wildcard include/config/mach/epc10.h) \
    $(wildcard include/config/mach/sgh/i740.h) \
    $(wildcard include/config/mach/tuna.h) \
    $(wildcard include/config/mach/mx51/tulip.h) \
    $(wildcard include/config/mach/mx51/aster7.h) \
    $(wildcard include/config/mach/acro37xbrd.h) \
    $(wildcard include/config/mach/elke.h) \
    $(wildcard include/config/mach/sbc6000x.h) \
    $(wildcard include/config/mach/r1801e.h) \
    $(wildcard include/config/mach/h1600.h) \
    $(wildcard include/config/mach/mini210.h) \
    $(wildcard include/config/mach/mini8168.h) \
    $(wildcard include/config/mach/pc7308.h) \
    $(wildcard include/config/mach/kmm2m01.h) \
    $(wildcard include/config/mach/mx51erebus.h) \
    $(wildcard include/config/mach/wm8650refboard.h) \
    $(wildcard include/config/mach/tuxrail.h) \
    $(wildcard include/config/mach/arthur.h) \
    $(wildcard include/config/mach/doorboy.h) \
    $(wildcard include/config/mach/xarina.h) \
    $(wildcard include/config/mach/roverx7.h) \
    $(wildcard include/config/mach/sdvr.h) \
    $(wildcard include/config/mach/acer/maya.h) \
    $(wildcard include/config/mach/pico.h) \
    $(wildcard include/config/mach/cwmx233.h) \
    $(wildcard include/config/mach/cwam1808.h) \
    $(wildcard include/config/mach/cwdm365.h) \
    $(wildcard include/config/mach/mx51/moray.h) \
    $(wildcard include/config/mach/thales/cbc.h) \
    $(wildcard include/config/mach/bluepoint.h) \
    $(wildcard include/config/mach/dir665.h) \
    $(wildcard include/config/mach/acmerover1.h) \
    $(wildcard include/config/mach/shooter/ct.h) \
    $(wildcard include/config/mach/bliss.h) \
    $(wildcard include/config/mach/blissc.h) \
    $(wildcard include/config/mach/thales/adc.h) \
    $(wildcard include/config/mach/ubisys/p9d/evp.h) \
    $(wildcard include/config/mach/atdgp318.h) \
    $(wildcard include/config/mach/mt6573.h) \
    $(wildcard include/config/mach/mt6575.h) \
    $(wildcard include/config/mach/mt6583.h) \
    $(wildcard include/config/mach/mt8320.h) \
    $(wildcard include/config/mach/mt6577.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/mach/arch.h \
    $(wildcard include/config/multi/irq/handler.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/mach/irq.h \
  include/linux/irq.h \
    $(wildcard include/config/irq/release/method.h) \
    $(wildcard include/config/generic/pending/irq.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/irq_regs.h \
  include/asm-generic/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/irq/preflow/fasteoi.h) \
    $(wildcard include/config/sparse/irq.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/hw_irq.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/mach/map.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/mach/time.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/setup.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/system.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/board.h \
  ../mediatek/custom/out/s9081/kernel/core/board-custom.h \
  include/linux/autoconf.h \
    $(wildcard include/config/mtk/usb/gadget.h) \
    $(wildcard include/config/scsi/dma.h) \
    $(wildcard include/config/kernel/gzip.h) \
    $(wildcard include/config/input/keyboard.h) \
    $(wildcard include/config/rfs/accel.h) \
    $(wildcard include/config/ip/nf/target/redirect.h) \
    $(wildcard include/config/crc32.h) \
    $(wildcard include/config/i2c/boardinfo.h) \
    $(wildcard include/config/have/aout.h) \
    $(wildcard include/config/vfp.h) \
    $(wildcard include/config/ext3/fs/xattr.h) \
    $(wildcard include/config/fb/tileblitting.h) \
    $(wildcard include/config/blk/dev/dm.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/ip/multiple/tables.h) \
    $(wildcard include/config/flatmem/manual.h) \
    $(wildcard include/config/mtk/aee/kdump.h) \
    $(wildcard include/config/network/filesystems.h) \
    $(wildcard include/config/cpu/freq/gov/ondemand.h) \
    $(wildcard include/config/experimental.h) \
    $(wildcard include/config/ppp/sync/tty.h) \
    $(wildcard include/config/arch/suspend/possible.h) \
    $(wildcard include/config/mtk/g2d.h) \
    $(wildcard include/config/ssb/possible.h) \
    $(wildcard include/config/nf/nat/sip.h) \
    $(wildcard include/config/netfilter/xt/match/statistic.h) \
    $(wildcard include/config/stp.h) \
    $(wildcard include/config/inet6/tunnel.h) \
    $(wildcard include/config/nf/conntrack/sip.h) \
    $(wildcard include/config/crypto/manager/disable/tests.h) \
    $(wildcard include/config/have/kernel/lzma.h) \
    $(wildcard include/config/default/security/dac.h) \
    $(wildcard include/config/fib/rules.h) \
    $(wildcard include/config/ip6/nf/mangle.h) \
    $(wildcard include/config/netfilter/xt/match/realm.h) \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/crypto/aead.h) \
    $(wildcard include/config/input/mousedev/psaux.h) \
    $(wildcard include/config/default/tcp/cong.h) \
    $(wildcard include/config/uevent/helper/path.h) \
    $(wildcard include/config/usb/storage/usbat.h) \
    $(wildcard include/config/mtk/serial.h) \
    $(wildcard include/config/nf/nat/proto/gre.h) \
    $(wildcard include/config/android/binder/ipc.h) \
    $(wildcard include/config/ip6/nf/target/reject.h) \
    $(wildcard include/config/keyboard/hid.h) \
    $(wildcard include/config/inet6/xfrm/mode/routeoptimization.h) \
    $(wildcard include/config/default/message/loglevel.h) \
    $(wildcard include/config/kgdb/serial/console.h) \
    $(wildcard include/config/have/arm/scu.h) \
    $(wildcard include/config/netfilter/xt/match/quota2/log.h) \
    $(wildcard include/config/connector.h) \
    $(wildcard include/config/legacy/ptys.h) \
    $(wildcard include/config/xfrm/ipcomp.h) \
    $(wildcard include/config/crypto/rng2.h) \
    $(wildcard include/config/netfilter/netlink/queue.h) \
    $(wildcard include/config/mtk/combo/bt.h) \
    $(wildcard include/config/msdos/fs.h) \
    $(wildcard include/config/tun.h) \
    $(wildcard include/config/ipv6/pimsm/v2.h) \
    $(wildcard include/config/cfg80211.h) \
    $(wildcard include/config/fiq/debugger/console.h) \
    $(wildcard include/config/jbd/debug.h) \
    $(wildcard include/config/dm/crypt.h) \
    $(wildcard include/config/have/proc/cpu.h) \
    $(wildcard include/config/hid/belkin.h) \
    $(wildcard include/config/isr/monitor.h) \
    $(wildcard include/config/mtk/net/ccmni.h) \
    $(wildcard include/config/wireless/ext/sysfs.h) \
    $(wildcard include/config/usb.h) \
    $(wildcard include/config/crypto/hmac.h) \
    $(wildcard include/config/branch/profile/none.h) \
    $(wildcard include/config/mt/sched/monitor.h) \
    $(wildcard include/config/scsi/scan/async.h) \
    $(wildcard include/config/ip6/nf/target/log.h) \
    $(wildcard include/config/ext4/debug.h) \
    $(wildcard include/config/ip/nf/arptables.h) \
    $(wildcard include/config/hid/cherry.h) \
    $(wildcard include/config/mtk/gps.h) \
    $(wildcard include/config/hid/sunplus.h) \
    $(wildcard include/config/bcma/possible.h) \
    $(wildcard include/config/nf/conntrack/proc/compat.h) \
    $(wildcard include/config/hid/thrustmaster.h) \
    $(wildcard include/config/mtd/cfi/i2.h) \
    $(wildcard include/config/crypto/authenc.h) \
    $(wildcard include/config/bounce.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/mtd.h) \
    $(wildcard include/config/mmc/block/minors.h) \
    $(wildcard include/config/mtk/combo/gps.h) \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/input/mousedev.h) \
    $(wildcard include/config/crypto/des.h) \
    $(wildcard include/config/nls/codepage/437.h) \
    $(wildcard include/config/mtd/nand/ids.h) \
    $(wildcard include/config/cfg80211/internal/regdb.h) \
    $(wildcard include/config/fiq/debugger.h) \
    $(wildcard include/config/arm/gic.h) \
    $(wildcard include/config/serio.h) \
    $(wildcard include/config/input/mouse.h) \
    $(wildcard include/config/rtc/intf/sysfs.h) \
    $(wildcard include/config/blk/dev/initrd.h) \
    $(wildcard include/config/leds/trigger/timer.h) \
    $(wildcard include/config/mtd/nand/platform.h) \
    $(wildcard include/config/zlib/inflate.h) \
    $(wildcard include/config/arm/errata/764369.h) \
    $(wildcard include/config/crypto/twofish/common.h) \
    $(wildcard include/config/logo/linux/clut224.h) \
    $(wildcard include/config/ip/pnp.h) \
    $(wildcard include/config/mt/eng/build.h) \
    $(wildcard include/config/cpu/freq/gov/hybrid.h) \
    $(wildcard include/config/rtc/intf/proc.h) \
    $(wildcard include/config/stacktrace/support.h) \
    $(wildcard include/config/usb/device/class.h) \
    $(wildcard include/config/arm.h) \
    $(wildcard include/config/netfilter/xt/match/string.h) \
    $(wildcard include/config/ip/nf/target/log.h) \
    $(wildcard include/config/has/wakelock.h) \
    $(wildcard include/config/logo.h) \
    $(wildcard include/config/usb/storage.h) \
    $(wildcard include/config/standalone.h) \
    $(wildcard include/config/cpu/freq/gov/performance.h) \
    $(wildcard include/config/arch/has/cpufreq.h) \
    $(wildcard include/config/ashmem.h) \
    $(wildcard include/config/have/ide.h) \
    $(wildcard include/config/hid/apple.h) \
    $(wildcard include/config/init/env/arg/limit.h) \
    $(wildcard include/config/nf/conntrack/pptp.h) \
    $(wildcard include/config/context/switch/tracer.h) \
    $(wildcard include/config/switch.h) \
    $(wildcard include/config/devkmem.h) \
    $(wildcard include/config/ppp/deflate.h) \
    $(wildcard include/config/textsearch/kmp.h) \
    $(wildcard include/config/serio/ambakmi.h) \
    $(wildcard include/config/vt.h) \
    $(wildcard include/config/mtk/pmic.h) \
    $(wildcard include/config/netfilter/xt/target/classify.h) \
    $(wildcard include/config/power/supply.h) \
    $(wildcard include/config/cpu/cache/vipt.h) \
    $(wildcard include/config/netfilter/xt/target/nfqueue.h) \
    $(wildcard include/config/wext/core.h) \
    $(wildcard include/config/nls.h) \
    $(wildcard include/config/pvr/sgx/mt6577.h) \
    $(wildcard include/config/mfd/support.h) \
    $(wildcard include/config/pppoe.h) \
    $(wildcard include/config/xfrm/migrate.h) \
    $(wildcard include/config/syn/cookies.h) \
    $(wildcard include/config/ip/advanced/router.h) \
    $(wildcard include/config/usb/storage/alauda.h) \
    $(wildcard include/config/mtk/devinfo/driver.h) \
    $(wildcard include/config/ip6/nf/iptables.h) \
    $(wildcard include/config/nl80211/testmode.h) \
    $(wildcard include/config/cpu/freq/gov/userspace.h) \
    $(wildcard include/config/inet/ipcomp.h) \
    $(wildcard include/config/hid/cypress.h) \
    $(wildcard include/config/nls/iso8859/1.h) \
    $(wildcard include/config/android/ram/console/early/size.h) \
    $(wildcard include/config/crypto/workqueue.h) \
    $(wildcard include/config/hid/kensington.h) \
    $(wildcard include/config/cpu/freq/table.h) \
    $(wildcard include/config/textsearch/bm.h) \
    $(wildcard include/config/ppp/mppe.h) \
    $(wildcard include/config/rfkill.h) \
    $(wildcard include/config/netdevices.h) \
    $(wildcard include/config/net/key.h) \
    $(wildcard include/config/ip6/nf/target/hl.h) \
    $(wildcard include/config/iosched/deadline.h) \
    $(wildcard include/config/cpu/tlb/v7.h) \
    $(wildcard include/config/eventfd.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/xfrm.h) \
    $(wildcard include/config/defconfig/list.h) \
    $(wildcard include/config/list.h) \
    $(wildcard include/config/ipv6/multiple/tables.h) \
    $(wildcard include/config/ip/nf/target/masquerade.h) \
    $(wildcard include/config/proc/page/monitor.h) \
    $(wildcard include/config/android/low/memory/killer.h) \
    $(wildcard include/config/arch/has/cpu/idle/wait.h) \
    $(wildcard include/config/android/ram/console/enable/verbose.h) \
    $(wildcard include/config/cfg80211/developer/warnings.h) \
    $(wildcard include/config/scsi/wait/scan.h) \
    $(wildcard include/config/mtk/bt/net/dev.h) \
    $(wildcard include/config/nf/defrag/ipv4.h) \
    $(wildcard include/config/select/memory/model.h) \
    $(wildcard include/config/inet/xfrm/mode/beet.h) \
    $(wildcard include/config/mmc/unsafe/resume.h) \
    $(wildcard include/config/ipv6/route/info.h) \
    $(wildcard include/config/netfilter/advanced.h) \
    $(wildcard include/config/crypto/deflate.h) \
    $(wildcard include/config/ipv6/router/pref.h) \
    $(wildcard include/config/netfilter/netlink/log.h) \
    $(wildcard include/config/have/dynamic/ftrace.h) \
    $(wildcard include/config/local/wdt.h) \
    $(wildcard include/config/magic/sysrq.h) \
    $(wildcard include/config/ipv6/sit/6rd.h) \
    $(wildcard include/config/netfilter/xt/match/mark.h) \
    $(wildcard include/config/have/arm/twd.h) \
    $(wildcard include/config/ip/nf/mangle.h) \
    $(wildcard include/config/mtk/sched/tracers.h) \
    $(wildcard include/config/default/cfq.h) \
    $(wildcard include/config/inet6/xfrm/mode/tunnel.h) \
    $(wildcard include/config/ip/nf/filter.h) \
    $(wildcard include/config/hid/zeroplus.h) \
    $(wildcard include/config/ext3/fs.h) \
    $(wildcard include/config/netfilter/xt/match/length.h) \
    $(wildcard include/config/fat/fs.h) \
    $(wildcard include/config/textsearch/fsm.h) \
    $(wildcard include/config/ip6/nf/raw.h) \
    $(wildcard include/config/inet/tunnel.h) \
    $(wildcard include/config/eeprom/93cx6.h) \
    $(wildcard include/config/mmc/block/bounce.h) \
    $(wildcard include/config/generic/clockevents.h) \
    $(wildcard include/config/iosched/cfq.h) \
    $(wildcard include/config/lib80211.h) \
    $(wildcard include/config/cpu/cp15/mmu.h) \
    $(wildcard include/config/rfkill/leds.h) \
    $(wildcard include/config/console/translations.h) \
    $(wildcard include/config/stop/machine.h) \
    $(wildcard include/config/cpu/freq.h) \
    $(wildcard include/config/dummy/console.h) \
    $(wildcard include/config/nls/ascii.h) \
    $(wildcard include/config/mediatek/solution.h) \
    $(wildcard include/config/qfmt/v2.h) \
    $(wildcard include/config/leds/triggers.h) \
    $(wildcard include/config/rd/gzip.h) \
    $(wildcard include/config/have/regs/and/stack/access/api.h) \
    $(wildcard include/config/usb/storage/cypress/atacb.h) \
    $(wildcard include/config/inet/xfrm/mode/transport.h) \
    $(wildcard include/config/crypto/md5.h) \
    $(wildcard include/config/have/generic/hardirqs.h) \
    $(wildcard include/config/usb/android/adb.h) \
    $(wildcard include/config/binfmt/elf.h) \
    $(wildcard include/config/scsi/proc/fs.h) \
    $(wildcard include/config/ip/pimsm/v1.h) \
    $(wildcard include/config/inet6/ah.h) \
    $(wildcard include/config/cpu/cp15.h) \
    $(wildcard include/config/usb/android.h) \
    $(wildcard include/config/inet/xfrm/tunnel.h) \
    $(wildcard include/config/netfilter/xt/mark.h) \
    $(wildcard include/config/netfilter/xtables.h) \
    $(wildcard include/config/mtk/mmc.h) \
    $(wildcard include/config/usb/storage/datafab.h) \
    $(wildcard include/config/usb/storage/karma.h) \
    $(wildcard include/config/gps.h) \
    $(wildcard include/config/hid/greenasia.h) \
    $(wildcard include/config/logo/linux/mono.h) \
    $(wildcard include/config/hardirqs/sw/resend.h) \
    $(wildcard include/config/vt/hw/console/binding.h) \
    $(wildcard include/config/hid/gyration.h) \
    $(wildcard include/config/mtk/i2c.h) \
    $(wildcard include/config/earlysuspend.h) \
    $(wildcard include/config/crc16.h) \
    $(wildcard include/config/generic/calibrate/delay.h) \
    $(wildcard include/config/cpu/has/pmu.h) \
    $(wildcard include/config/tmpfs.h) \
    $(wildcard include/config/anon/inodes.h) \
    $(wildcard include/config/ip/pnp/dhcp.h) \
    $(wildcard include/config/netfilter/xt/target/hl.h) \
    $(wildcard include/config/vmsplit/3g.h) \
    $(wildcard include/config/rtc/hctosys.h) \
    $(wildcard include/config/serial/core/console.h) \
    $(wildcard include/config/usb/hid.h) \
    $(wildcard include/config/usb/mtk/hdrc/gadget.h) \
    $(wildcard include/config/android.h) \
    $(wildcard include/config/lib80211/debug.h) \
    $(wildcard include/config/nf/conntrack/events.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/mtk/fb/support/assertion/layer.h) \
    $(wildcard include/config/crypto/pcomp2.h) \
    $(wildcard include/config/nf/conntrack/ftp.h) \
    $(wildcard include/config/usb/gadget.h) \
    $(wildcard include/config/event/power/tracing/deprecated.h) \
    $(wildcard include/config/mtk/smart/battery.h) \
    $(wildcard include/config/unix.h) \
    $(wildcard include/config/yaffs/yaffs1.h) \
    $(wildcard include/config/crypto/hash2.h) \
    $(wildcard include/config/default/hostname.h) \
    $(wildcard include/config/ampc/cdev/num.h) \
    $(wildcard include/config/usb/storage/isd200.h) \
    $(wildcard include/config/cpu/freq/gov/powersave.h) \
    $(wildcard include/config/xps.h) \
    $(wildcard include/config/inet/esp.h) \
    $(wildcard include/config/nf/conntrack/ipv6.h) \
    $(wildcard include/config/md.h) \
    $(wildcard include/config/crypto/algapi.h) \
    $(wildcard include/config/bridge.h) \
    $(wildcard include/config/misc/devices.h) \
    $(wildcard include/config/input/uinput.h) \
    $(wildcard include/config/mtd/cfi/i1.h) \
    $(wildcard include/config/nf/nat.h) \
    $(wildcard include/config/crypto/hash.h) \
    $(wildcard include/config/log/buf/shift.h) \
    $(wildcard include/config/extra/firmware.h) \
    $(wildcard include/config/cache/l2x0.h) \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/proc/events.h) \
    $(wildcard include/config/virt/to/bus.h) \
    $(wildcard include/config/vfat/fs.h) \
    $(wildcard include/config/cpu/rmap.h) \
    $(wildcard include/config/blk/dev/loop.h) \
    $(wildcard include/config/wakelock.h) \
    $(wildcard include/config/input/misc.h) \
    $(wildcard include/config/cpu/pabrt/v7.h) \
    $(wildcard include/config/suspend.h) \
    $(wildcard include/config/mtd/nand/ecc.h) \
    $(wildcard include/config/crypto/cbc.h) \
    $(wildcard include/config/kprobes.h) \
    $(wildcard include/config/fs/mbcache.h) \
    $(wildcard include/config/rtc/class.h) \
    $(wildcard include/config/arm/amba.h) \
    $(wildcard include/config/android/ram/console/early/init.h) \
    $(wildcard include/config/generic/tracer.h) \
    $(wildcard include/config/yaffs/auto/yaffs2.h) \
    $(wildcard include/config/mtk/sound.h) \
    $(wildcard include/config/have/function/tracer.h) \
    $(wildcard include/config/nf/nat/tftp.h) \
    $(wildcard include/config/netfilter/xt/match/multiport.h) \
    $(wildcard include/config/cpu/cache/v7.h) \
    $(wildcard include/config/crypto/manager2.h) \
    $(wildcard include/config/usb/gadget/vbus/draw.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/i2c.h) \
    $(wildcard include/config/ppp/multilink.h) \
    $(wildcard include/config/cpu/abrt/ev7.h) \
    $(wildcard include/config/crypto/ecb.h) \
    $(wildcard include/config/usb/libusual.h) \
    $(wildcard include/config/debug/fs.h) \
    $(wildcard include/config/mtk/combo/fm.h) \
    $(wildcard include/config/base/full.h) \
    $(wildcard include/config/fb/cfb/imageblit.h) \
    $(wildcard include/config/zlib/deflate.h) \
    $(wildcard include/config/yaffs/fs.h) \
    $(wildcard include/config/mtprof/applaunch.h) \
    $(wildcard include/config/fw/loader.h) \
    $(wildcard include/config/rtc/hctosys/device.h) \
    $(wildcard include/config/netfilter/xt/match/recent.h) \
    $(wildcard include/config/signalfd.h) \
    $(wildcard include/config/ext4/fs.h) \
    $(wildcard include/config/kretprobes.h) \
    $(wildcard include/config/crypto/sha1.h) \
    $(wildcard include/config/ipv6/privacy.h) \
    $(wildcard include/config/usb/gadget/dualspeed.h) \
    $(wildcard include/config/has/iomem.h) \
    $(wildcard include/config/pppopns.h) \
    $(wildcard include/config/usb/android/mass/storage.h) \
    $(wildcard include/config/usb/storage/sddr09.h) \
    $(wildcard include/config/mtd/map/bank/width/1.h) \
    $(wildcard include/config/ip6/nf/match/frag.h) \
    $(wildcard include/config/yaffs/xattr.h) \
    $(wildcard include/config/net.h) \
    $(wildcard include/config/input/evdev.h) \
    $(wildcard include/config/have/sparse/irq.h) \
    $(wildcard include/config/hid/dragonrise.h) \
    $(wildcard include/config/quota/tree.h) \
    $(wildcard include/config/packet.h) \
    $(wildcard include/config/netfilter/xt/match/iprange.h) \
    $(wildcard include/config/nf/conntrack/tftp.h) \
    $(wildcard include/config/nop/tracer.h) \
    $(wildcard include/config/inet.h) \
    $(wildcard include/config/mtk/aee/aed.h) \
    $(wildcard include/config/ip/pnp/bootp.h) \
    $(wildcard include/config/prevent/firmware/build.h) \
    $(wildcard include/config/crypto/twofish.h) \
    $(wildcard include/config/rfkill/pm.h) \
    $(wildcard include/config/mtk/aee/ipanic.h) \
    $(wildcard include/config/rtc/lib.h) \
    $(wildcard include/config/usb/android/acm.h) \
    $(wildcard include/config/netfilter/xt/match/policy.h) \
    $(wildcard include/config/have/kprobes.h) \
    $(wildcard include/config/ip/route/classid.h) \
    $(wildcard include/config/crypto/aes.h) \
    $(wildcard include/config/ext4/use/for/ext23.h) \
    $(wildcard include/config/ip6/nf/target/reject/skerr.h) \
    $(wildcard include/config/nf/conntrack/mark.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/eagle77v1.h) \
    $(wildcard include/config/nls/codepage/950.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/inet/xfrm/mode/tunnel.h) \
    $(wildcard include/config/fiq/debugger/console/default/enable.h) \
    $(wildcard include/config/nf/nat/needed.h) \
    $(wildcard include/config/lockdep/support.h) \
    $(wildcard include/config/rtc/intf/alarm.h) \
    $(wildcard include/config/cpu/freq/stat.h) \
    $(wildcard include/config/cpu/freq/default/gov/hybrid.h) \
    $(wildcard include/config/mtd/blkdevs.h) \
    $(wildcard include/config/mtk/touchpanel.h) \
    $(wildcard include/config/inet6/esp.h) \
    $(wildcard include/config/mt65xx/tracer.h) \
    $(wildcard include/config/sysctl/syscall.h) \
    $(wildcard include/config/mtk/leds.h) \
    $(wildcard include/config/mtk/accdet.h) \
    $(wildcard include/config/ip6/nf/filter.h) \
    $(wildcard include/config/input/mousedev/screen/x.h) \
    $(wildcard include/config/need/dma/map/state.h) \
    $(wildcard include/config/serio/libps2.h) \
    $(wildcard include/config/netfilter/xt/match/connbytes.h) \
    $(wildcard include/config/android/paranoid/network.h) \
    $(wildcard include/config/hid/twinhan.h) \
    $(wildcard include/config/panic/timeout.h) \
    $(wildcard include/config/zboot/rom/bss.h) \
    $(wildcard include/config/usb/storage/sddr55.h) \
    $(wildcard include/config/netfilter/xt/match/time.h) \
    $(wildcard include/config/mtk/fb.h) \
    $(wildcard include/config/have/kernel/gzip.h) \
    $(wildcard include/config/dm/uevent.h) \
    $(wildcard include/config/netfilter/xt/match/mac.h) \
    $(wildcard include/config/netfilter/xt/target/nflog.h) \
    $(wildcard include/config/android/timed/output.h) \
    $(wildcard include/config/libcrc32c.h) \
    $(wildcard include/config/crypto/sha256.h) \
    $(wildcard include/config/have/ftrace/mcount/record.h) \
    $(wildcard include/config/inet/tcp/diag.h) \
    $(wildcard include/config/hid/sony.h) \
    $(wildcard include/config/hw/console.h) \
    $(wildcard include/config/dm/debug.h) \
    $(wildcard include/config/devmem.h) \
    $(wildcard include/config/max/dram/size/support.h) \
    $(wildcard include/config/hid/monterey.h) \
    $(wildcard include/config/hid/ezkey.h) \
    $(wildcard include/config/iosched/noop.h) \
    $(wildcard include/config/quotactl.h) \
    $(wildcard include/config/neon.h) \
    $(wildcard include/config/debug/kernel.h) \
    $(wildcard include/config/localversion.h) \
    $(wildcard include/config/mach/no/westbridge.h) \
    $(wildcard include/config/crypto.h) \
    $(wildcard include/config/usb/trancevibrator.h) \
    $(wildcard include/config/default/mmap/min/addr.h) \
    $(wildcard include/config/ip/nf/iptables.h) \
    $(wildcard include/config/cmdline.h) \
    $(wildcard include/config/have/dma/api/debug.h) \
    $(wildcard include/config/hid/samsung.h) \
    $(wildcard include/config/ip/pimsm/v2.h) \
    $(wildcard include/config/usb/arch/has/hcd.h) \
    $(wildcard include/config/generic/irq/show.h) \
    $(wildcard include/config/mtk/combo/wifi.h) \
    $(wildcard include/config/mtk/wd/kicker.h) \
    $(wildcard include/config/ipv6/optimistic/dad.h) \
    $(wildcard include/config/alignment/trap.h) \
    $(wildcard include/config/mtk/aee/feature.h) \
    $(wildcard include/config/scsi/mod.h) \
    $(wildcard include/config/oprofile.h) \
    $(wildcard include/config/crypto/crc32c.h) \
    $(wildcard include/config/serial/core.h) \
    $(wildcard include/config/fuse/fs.h) \
    $(wildcard include/config/embedded.h) \
    $(wildcard include/config/hid/microsoft.h) \
    $(wildcard include/config/have/kretprobes.h) \
    $(wildcard include/config/nf/defrag/ipv6.h) \
    $(wildcard include/config/ppp/filter.h) \
    $(wildcard include/config/android/ram/console/early/addr.h) \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/scsi.h) \
    $(wildcard include/config/fb/cfb/fillrect.h) \
    $(wildcard include/config/nf/nat/pptp.h) \
    $(wildcard include/config/hid/chicony.h) \
    $(wildcard include/config/hid.h) \
    $(wildcard include/config/font/8x16.h) \
    $(wildcard include/config/jbd2.h) \
    $(wildcard include/config/inet6/ipcomp.h) \
    $(wildcard include/config/android/ram/console.h) \
    $(wildcard include/config/mtk/jpeg.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/printk/process/info.h) \
    $(wildcard include/config/misc/filesystems.h) \
    $(wildcard include/config/ftrace.h) \
    $(wildcard include/config/netfilter/xt/match/socket.h) \
    $(wildcard include/config/hid/topseed.h) \
    $(wildcard include/config/hid/a4tech.h) \
    $(wildcard include/config/print/quota/warning.h) \
    $(wildcard include/config/ip/nf/target/netmap.h) \
    $(wildcard include/config/rcu/cpu/stall/timeout.h) \
    $(wildcard include/config/cfg80211/debugfs.h) \
    $(wildcard include/config/mtk/keypad.h) \
    $(wildcard include/config/yaffs/yaffs2.h) \
    $(wildcard include/config/ip6/nf/match/ipv6header.h) \
    $(wildcard include/config/cache/pl310.h) \
    $(wildcard include/config/profiling.h) \
    $(wildcard include/config/inet6/xfrm/mode/transport.h) \
    $(wildcard include/config/crypto/arc4.h) \
    $(wildcard include/config/mtk/rtc.h) \
    $(wildcard include/config/slhc.h) \
    $(wildcard include/config/scsi/tgt.h) \
    $(wildcard include/config/crypto/manager.h) \
    $(wildcard include/config/ppp/bsdcomp.h) \
    $(wildcard include/config/mtd/nand.h) \
    $(wildcard include/config/vectors/base.h) \
    $(wildcard include/config/netfilter/xt/target/mark.h) \
    $(wildcard include/config/ipv6/subtrees.h) \
    $(wildcard include/config/mmc/block.h) \
    $(wildcard include/config/mtk/serial/console.h) \
    $(wildcard include/config/arch/mt6577.h) \
    $(wildcard include/config/expert.h) \
    $(wildcard include/config/wireless.h) \
    $(wildcard include/config/wext/proc.h) \
    $(wildcard include/config/kgdb.h) \
    $(wildcard include/config/fat/default/iocharset.h) \
    $(wildcard include/config/frame/warn.h) \
    $(wildcard include/config/rcu/cpu/stall/verbose.h) \
    $(wildcard include/config/generic/hweight.h) \
    $(wildcard include/config/initramfs/source.h) \
    $(wildcard include/config/mmc.h) \
    $(wildcard include/config/hid/logitech.h) \
    $(wildcard include/config/stacktrace.h) \
    $(wildcard include/config/pppolac.h) \
    $(wildcard include/config/usb/mtk/debug/fs.h) \
    $(wildcard include/config/usb/debug.h) \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/kgdb/kdb.h) \
    $(wildcard include/config/cgroup/cpuacct.h) \
    $(wildcard include/config/fb/earlysuspend.h) \
    $(wildcard include/config/i2c/helper/auto.h) \
    $(wildcard include/config/inet/ah.h) \
    $(wildcard include/config/default/iosched.h) \
    $(wildcard include/config/ipv6/mip6.h) \
    $(wildcard include/config/nlattr.h) \
    $(wildcard include/config/tcp/cong/cubic.h) \
    $(wildcard include/config/suspend/freezer.h) \
    $(wildcard include/config/netfilter/xt/connmark.h) \
    $(wildcard include/config/hid/kye.h) \
    $(wildcard include/config/firmware/in/kernel.h) \
    $(wildcard include/config/ip/mroute/multiple/tables.h) \
    $(wildcard include/config/input/touchscreen.h) \
    $(wildcard include/config/arpd.h) \
    $(wildcard include/config/netfilter/xt/match/limit.h) \
    $(wildcard include/config/ip6/nf/queue.h) \
    $(wildcard include/config/arm/errata/754322.h) \
    $(wildcard include/config/fb.h) \
    $(wildcard include/config/i2c/compat.h) \
    $(wildcard include/config/cpu/32v7.h) \
    $(wildcard include/config/msdos/partition.h) \
    $(wildcard include/config/have/oprofile.h) \
    $(wildcard include/config/have/generic/dma/coherent.h) \
    $(wildcard include/config/thermal.h) \
    $(wildcard include/config/logo/linux/vga16.h) \
    $(wildcard include/config/hid/petalynx.h) \
    $(wildcard include/config/have/arch/kgdb.h) \
    $(wildcard include/config/nf/conntrack/ipv4.h) \
    $(wildcard include/config/smp/on/up.h) \
    $(wildcard include/config/zone/dma/flag.h) \
    $(wildcard include/config/rps.h) \
    $(wildcard include/config/inet6/xfrm/tunnel.h) \
    $(wildcard include/config/usb/mtk/hdrc.h) \
    $(wildcard include/config/legacy/pty/count.h) \
    $(wildcard include/config/mtd/map/bank/width/2.h) \
    $(wildcard include/config/usb/mtk/debug.h) \
    $(wildcard include/config/ip/multicast.h) \
    $(wildcard include/config/wakelock/stat.h) \
    $(wildcard include/config/has/earlysuspend.h) \
    $(wildcard include/config/default/security.h) \
    $(wildcard include/config/tick/oneshot.h) \
    $(wildcard include/config/nf/nat/proto/udplite.h) \
    $(wildcard include/config/mtk/ccci/driver.h) \
    $(wildcard include/config/mouse/avagotech/a320.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/android/logger.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
    $(wildcard include/config/have/function/graph/tracer.h) \
    $(wildcard include/config/mtk/videocodec/driver.h) \
    $(wildcard include/config/crypto/blkcipher2.h) \
    $(wildcard include/config/mtd/block.h) \
    $(wildcard include/config/wext/priv.h) \
    $(wildcard include/config/hid/pantherlord.h) \
    $(wildcard include/config/mtk/devapc/driver.h) \
    $(wildcard include/config/rfkill/input.h) \
    $(wildcard include/config/ikconfig.h) \
    $(wildcard include/config/bridge/igmp/snooping.h) \
    $(wildcard include/config/have/c/recordmcount.h) \
    $(wildcard include/config/have/perf/events.h) \
    $(wildcard include/config/ppp/async.h) \
    $(wildcard include/config/uid/stat.h) \
    $(wildcard include/config/sys/supports/apm/emulation.h) \
    $(wildcard include/config/rtc/intf/alarm/dev.h) \
    $(wildcard include/config/netfilter/xt/match/quota2.h) \
    $(wildcard include/config/fat/default/codepage.h) \
    $(wildcard include/config/blk/dev.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/mtk/vibrator.h) \
    $(wildcard include/config/tracing/support.h) \
    $(wildcard include/config/unix98/ptys.h) \
    $(wildcard include/config/jbd.h) \
    $(wildcard include/config/printk/time.h) \
    $(wildcard include/config/input/mousedev/screen/y.h) \
    $(wildcard include/config/ppp.h) \
    $(wildcard include/config/netfilter/xt/match/quota.h) \
    $(wildcard include/config/have/kernel/lzo.h) \
    $(wildcard include/config/inet/diag.h) \
    $(wildcard include/config/nf/nat/ftp.h) \
    $(wildcard include/config/nf/ct/proto/udplite.h) \
    $(wildcard include/config/ikconfig/proc.h) \
    $(wildcard include/config/proc.h) \
    $(wildcard include/config/elf/core.h) \
    $(wildcard include/config/textsearch.h) \
    $(wildcard include/config/usb/support.h) \
    $(wildcard include/config/arm/flush/console/on/restart.h) \
    $(wildcard include/config/netfilter/xt/match/qtaguid.h) \
    $(wildcard include/config/staging.h) \
    $(wildcard include/config/mtd/char.h) \
    $(wildcard include/config/mmprofile.h) \
    $(wildcard include/config/netfilter/xt/match/physdev.h) \
    $(wildcard include/config/netfilter/xt/match/state.h) \
    $(wildcard include/config/usb/storage/freecom.h) \
    $(wildcard include/config/inet6/xfrm/mode/beet.h) \
    $(wildcard include/config/mtk/ccci/devices.h) \
    $(wildcard include/config/fb/cfb/copyarea.h) \
    $(wildcard include/config/usb/storage/jumpshot.h) \
    $(wildcard include/config/cpu/freq/stat/details.h) \
    $(wildcard include/config/generic/clockevents/build.h) \
    $(wildcard include/config/usb/storage/debug.h) \
    $(wildcard include/config/hid/ntrig.h) \
    $(wildcard include/config/decompress/gzip.h) \
    $(wildcard include/config/yaffs/disable/tags/ecc.h) \
    $(wildcard include/config/llc.h) \
    $(wildcard include/config/cross/compile.h) \
    $(wildcard include/config/usb/gadget/selected.h) \
    $(wildcard include/config/generic/clockevents/broadcast.h) \
    $(wildcard include/config/nls/utf8.h) \
    $(wildcard include/config/netfilter/tproxy.h) \
    $(wildcard include/config/scsi/multi/lun.h) \
    $(wildcard include/config/hid/smartjoyplus.h) \
    $(wildcard include/config/new/leds.h) \
    $(wildcard include/config/crc/ccitt.h) \
    $(wildcard include/config/ext3/fs/posix/acl.h) \
    $(wildcard include/config/android/pmem.h) \
    $(wildcard include/config/blk/dev/sd.h) \
    $(wildcard include/config/cmdline/from/bootloader.h) \
    $(wildcard include/config/netfilter/netlink.h) \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/bitreverse.h) \
    $(wildcard include/config/console/poll.h) \
    $(wildcard include/config/fb/mode/helpers.h) \
    $(wildcard include/config/crypto/blkcipher.h) \
    $(wildcard include/config/usb/storage/onetouch.h) \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/pvr/build/release/mt6577.h) \
    $(wildcard include/config/fiq.h) \
    $(wildcard include/config/ip/nf/target/reject.h) \
    $(wildcard include/config/leds/class.h) \
    $(wildcard include/config/rtc/intf/dev.h) \
    $(wildcard include/config/mtd/map/bank/width/4.h) \
    $(wildcard include/config/hid/support.h) \
    $(wildcard include/config/ext4/fs/xattr.h) \
    $(wildcard include/config/net/activity/stats.h) \
    $(wildcard include/config/nls/default.h) \
    $(wildcard include/config/nf/ct/proto/gre.h) \
    $(wildcard include/config/nf/ct/netlink.h) \
    $(wildcard include/config/crypto/aead2.h) \
    $(wildcard include/config/debug/info.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/crypto/algapi2.h) \
    $(wildcard include/config/zboot/rom/text.h) \
    $(wildcard include/config/have/memblock.h) \
    $(wildcard include/config/input.h) \
    $(wildcard include/config/proc/sysctl.h) \
    $(wildcard include/config/have/irq/work.h) \
    $(wildcard include/config/user/wakelock.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/hardware.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt6577_gpio.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt_gpio.h \
    $(wildcard include/config/mt6575/fpga.h) \
  ../mediatek/custom/out/s9081/kernel/dct/cust_gpio_usage.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt_bt.h \
  include/linux/rfkill.h \
  include/linux/leds.h \
    $(wildcard include/config/leds/trigger/ide/disk.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/eint.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mtk_rtc.h \
  include/linux/rtc.h \
    $(wildcard include/config/rtc/intf/dev/uie/emul.h) \
  include/linux/cdev.h \
  include/linux/poll.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/poll.h \
  include/asm-generic/poll.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt6577_typedefs.h \
  include/linux/bug.h \
  ../mediatek/custom/out/s9081/kernel/dct/cust_eint.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt_combo.h \

mediatek/custom/out/s9081/kernel/core/board.o: $(deps_mediatek/custom/out/s9081/kernel/core/board.o)

$(deps_mediatek/custom/out/s9081/kernel/core/board.o):
