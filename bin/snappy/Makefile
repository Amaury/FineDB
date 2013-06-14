IPATH = -I. -I../../include
CFLAGS := $(IPATH) -Wall -g -O2 -DNDEBUG=1 -DSG=1
# Remove -DSG=1 if you don't need scather-gather support
# NDEBUG=1 is recommended for production

.PHONY: all clean

scmd: scmd.o map.o util.o
	cc scmd.o map.o util.o ../../lib/libsnappy.a   -o scmd
	cp scmd ../

all: clean scmd

clean: 
	rm -f *.o scmd ../scmd

