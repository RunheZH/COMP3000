OPTIONS = -g -Wall
obj-m += blocker.o

all:	main.o util.o
	cc -o blocker main.o util.o	
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

main.o:	main.c
	cc -c main.c

util.o:	util.c
	cc -c util.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.o b

