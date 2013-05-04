#ccflags-y += -I$(src)/core/include
ccflags-y += -I$(src)/linux/include -I$(src)/linux/pri/include -I$(src)/core/include -I$(src)/include -I$(src)/ -I$(MTK_PATH_PLATFORM)/core/include/

ifeq ($(MTK_COMBO_CHIP), MT6628)
EXT_FLAG =_mt6620
else
EXT_FLAG=
endif

COMMON_SRC_PATH := $(src)
COMMON_OBJ_PATH := $(src)

# WMT DRIVER
obj-$(CONFIG_MTK_COMBO) += mtk_stp_wmt$(EXT_FLAG).o
# WMT DRIVER-core part
mtk_stp_wmt$(EXT_FLAG)-objs	:= core/wmt_core.o core/wmt_ctrl.o  core/wmt_func.o core/wmt_ic_6620.o core/wmt_lib.o core/wmt_conf.o


# WMT DRIVER-linux private part
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/wmt_dev.o linux/pri/wmt_exp.o
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/wmt_tm.o

# WMT DRIVER-OSAL
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/osal.o
# WMT DRIVER-platform implementation
ccflags-y += -D WMT_PLAT_ALPS
mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/wmt_plat_alps.o
# FIXME: select desired hw according to project configuration
mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/mtk_wcn_cmb_hw_6620.o 
#mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/mtk_wcn_cmb_hw_6628.o 
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/stp_exp.o core/stp_core.o core/psm_core.o core/btm_core.o linux/pri/stp_dbg.o
ifneq ($(MTK_COMBO_CHIP), MT6628)
# WMT stub part (built-in kernel image)
obj-y			+= platform/alps/mtk_wcn_cmb_stub_alps.o
endif

obj-$(CONFIG_MTK_COMBO) += mtk_stp_uart$(EXT_FLAG).o
mtk_stp_uart$(EXT_FLAG)-objs	:= linux/pri/stp_uart.o


obj-$(CONFIG_MTK_COMBO) += mtk_hif_sdio$(EXT_FLAG).o
mtk_hif_sdio$(EXT_FLAG)-objs	:= linux/hif_sdio.o

# obj-$(CONFIG_MTK_COMBO) += mtk_stp_sdio$(EXT_FLAG).o
# mtk_stp_sdio$(EXT_FLAG)-objs	:= linux/pri/stp_sdio.o
# mtk_stp_sdio$(EXT_FLAG)-objs	+= linux/osal.o

obj-$(CONFIG_MTK_COMBO_BT) += mtk_stp_bt$(EXT_FLAG).o
mtk_stp_bt$(EXT_FLAG)-objs		:= linux/stp_chrdev_bt.o

obj-$(CONFIG_MTK_COMBO_GPS) += mtk_stp_gps$(EXT_FLAG).o
mtk_stp_gps$(EXT_FLAG)-objs	:= linux/stp_chrdev_gps.o


obj-$(CONFIG_MTK_COMBO_WIFI) += mtk_wmt_wifi$(EXT_FLAG).o
mtk_wmt_wifi$(EXT_FLAG)-objs       := linux/wmt_chrdev_wifi.o 
