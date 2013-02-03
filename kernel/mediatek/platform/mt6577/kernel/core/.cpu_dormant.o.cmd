cmd_mediatek/platform/mt6577/kernel/core/cpu_dormant.o := arm-linux-androideabi-gcc -Wp,-MD,mediatek/platform/mt6577/kernel/core/.cpu_dormant.o.d  -nostdinc -isystem /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/arm-linux-androideabi-4.4.x/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include -I/home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -I../mediatek/custom//common -I../mediatek/platform/mt6577/kernel/core/include/ -I../mediatek/source/kernel/include/ -I../mediatek/custom/out/s9081/kernel/magnetometer/ -I../mediatek/custom/out/s9081/kernel/headset/ -I../mediatek/custom/out/s9081/kernel/kpd/ -I../mediatek/custom/out/s9081/kernel/accelerometer/ -I../mediatek/custom/out/s9081/kernel/touchpanel/ -I../mediatek/custom/out/s9081/kernel/rtc/ -I../mediatek/custom/out/s9081/kernel/core/ -I../mediatek/custom/out/s9081/kernel/leds/ -I../mediatek/custom/out/s9081/kernel/battery/ -I../mediatek/custom/out/s9081/kernel/camera/ -I../mediatek/custom/out/s9081/kernel/alsps/ -I../mediatek/custom/out/s9081/kernel/vibrator/ -I../mediatek/custom/out/s9081/kernel/imgsensor/ -I../mediatek/custom/out/s9081/kernel/flashlight/ -I../mediatek/custom/out/s9081/kernel/dct/ -I../mediatek/custom/out/s9081/kernel/usb/ -I../mediatek/custom/out/s9081/kernel/magnetometer/inc/ -I../mediatek/custom/out/s9081/kernel/gyroscope/inc/ -I../mediatek/custom/out/s9081/kernel/sound/inc/ -I../mediatek/custom/out/s9081/kernel/sound/ -I../mediatek/custom/out/s9081/kernel/lens/inc/ -I../mediatek/custom/out/s9081/kernel/lens/ -I../mediatek/custom/out/s9081/kernel/accelerometer/inc/ -I../mediatek/custom/out/s9081/kernel/eeprom/inc/ -I../mediatek/custom/out/s9081/kernel/eeprom/ -I../mediatek/custom/out/s9081/kernel/hdmi/inc/ -I../mediatek/custom/out/s9081/kernel/jogball/inc/ -I../mediatek/custom/out/s9081/kernel/leds/inc/ -I../mediatek/custom/out/s9081/kernel/barometer/inc/ -I../mediatek/custom/out/s9081/kernel/./ -I../mediatek/custom/out/s9081/kernel/lcm/inc/ -I../mediatek/custom/out/s9081/kernel/lcm/ -I../mediatek/custom/out/s9081/kernel/alsps/inc/ -I../mediatek/custom/out/s9081/kernel/imgsensor/inc/ -I../mediatek/custom/out/s9081/kernel/flashlight/inc/ -I../mediatek/custom/out/s9081/kernel/ccci/ -I../mediatek/custom/out/s9081/hal/sensors/ -I../mediatek/custom/out/s9081/hal/camera/ -I../mediatek/custom/out/s9081/hal/audioflinger/ -I../mediatek/custom/out/s9081/hal/combo/ -I../mediatek/custom/out/s9081/hal/inc/ -I../mediatek/custom/out/s9081/hal/ant/ -I../mediatek/custom/out/s9081/hal/fmradio/ -I../mediatek/custom/out/s9081/hal/lens/ -I../mediatek/custom/out/s9081/hal/eeprom/ -I../mediatek/custom/out/s9081/hal/vt/ -I../mediatek/custom/out/s9081/hal/bluetooth/ -I../mediatek/custom/out/s9081/hal/imgsensor/ -I../mediatek/custom/out/s9081/hal/flashlight/ -I../mediatek/custom/out/s9081/hal/matv/ -I../mediatek/custom/out/s9081/common -D__KERNEL__   -mlittle-endian -DMODEM_3G -Imediatek/platform/mt6577/kernel/core/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -include asm/unified.h -Wa,-mfpu=neon -gdwarf-2        -c -o mediatek/platform/mt6577/kernel/core/cpu_dormant.o mediatek/platform/mt6577/kernel/core/cpu_dormant.S

source_mediatek/platform/mt6577/kernel/core/cpu_dormant.o := mediatek/platform/mt6577/kernel/core/cpu_dormant.S

deps_mediatek/platform/mt6577/kernel/core/cpu_dormant.o := \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/memory.h \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/arm/patch/phys/virt/16bit.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/const.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/memory.h \
  /home/xiuyongtang/OrderTinno/A_GPL/C_GPL_S9081/buildable/kernel/arch/arm/include/asm/sizes.h \
  include/asm-generic/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt6577_reg_base.h \
    $(wildcard include/config/base.h) \

mediatek/platform/mt6577/kernel/core/cpu_dormant.o: $(deps_mediatek/platform/mt6577/kernel/core/cpu_dormant.o)

$(deps_mediatek/platform/mt6577/kernel/core/cpu_dormant.o):
