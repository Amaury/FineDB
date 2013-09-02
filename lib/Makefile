SUBDIRS =	lmdb		\
		nanomsg		\
		snappy-c	\
		ylib		\
		linenoise

.PHONY: default clean all

default:
	for i in $(SUBDIRS); do cd $$i; make; cd ..; done

clean:
	for i in $(SUBDIRS); do cd $$i; make clean; cd ..; done
	rm -f *.a *~

all: clean default

