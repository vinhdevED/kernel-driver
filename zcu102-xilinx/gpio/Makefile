obj-m  += led_matrix_zcu102.o

export ARCH = arm64

export CROSS_COMPILE = aarch64-xilinx-linux-

KERNEL_DIR = /home/tdvinh/Desktop/PetalinuxProject/zcu102_driver_proj/build/tmp/work/zynqmp_generic-xilinx-linux/linux-xlnx/5.15.36+gitAUTOINC+19984dd147-r0/linux-zynqmp_generic-standard-build

PWD := $(shell pwd)

all: 
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean



