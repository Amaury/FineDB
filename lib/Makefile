.PHONY: all clean lib

lib:
	cd lmdb; make
	cd nanomsg; make
	cd snappy-c; make

all:
	cd lmdb; make all
	cd nanomsg; make all
	cd snappy-c; make all

clean:
	cd lmdb; make clean
	cd nanomsg; make clean
	cd snappy-c; make clean

