intf:
	gcc -O0 -g -I/usr/include/libnl3/ -o intf intf.c -lnl-3

all: intf

clean:
	rm -f intf
