#ifneq	($(KERNELRELEASE),)
#	obj-m	:=	hello.o

#else
#	KERNELDIR	?=	/lib/modules/$(shell uname -r)/build
#	PWD	:=	$(shell pwd)

#default:
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

#clean:
#	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

#endif

obj-m += hello.o

KDIR = /usr/src/linux-headers-5.3.0-28-generic

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
