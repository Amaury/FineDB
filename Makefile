.PHONY: all clean finedb lib bin src

finedb:
	cd lib; make
	cd bin; make
	cd src; make

all:
	cd lib; make all
	cd bin; make all
	cd src; make all

clean:
	cd lib; make clean
	cd bin; make clean
	cd src; make clean

lib:
	cd lib; make

bin:
	cd bin; make

src:
	cd src; make

