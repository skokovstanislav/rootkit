
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build/
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/misc 
#EXTRA_CFLAGS = -O2 --no-warnings
CFLAGS-Y = -O2


TARGET1 = rootkit
obj-m   := $(TARGET1).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD)  modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
