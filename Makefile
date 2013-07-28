.PHONY: all clean finedb lib src test

finedb:
	cd lib; make
	cd src; make
	cd test; make

all:
	cd lib; make all
	cd src; make all
	cd test; make all

clean:
	cd lib; make clean
	cd src; make clean
	cd test; make clean

lib:
	cd lib; make

src:
	cd src; make

test:
	cd test; make
