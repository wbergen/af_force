obj-m += af_force4.o

# CFLAGS=-Wnounused-function

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc scram.c -o scrambler
	gcc af_interface1.c -o af_interface

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	sudo insmod ./af_force4.ko