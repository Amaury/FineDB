CC	= gcc
W	= -W -Wall -Wno-unused-parameter -Wbad-function-cast
OPT = -O2 -g
CFLAGS	= -pthread $(OPT) $(W) $(XCFLAGS)
LDLIBS	=
SOLIBS	=
prefix	= /usr/local

IHDRS	= lmdb.h
ILIBS	= liblmdb.a liblmdb.so
#all:	$(ILIBS) $(PROGS)
all: clean liblmdb.a

install: $(ILIBS) $(IPROGS) $(IHDRS)
	cp $(ILIBS) $(DESTDIR)$(prefix)/lib
	cp $(IHDRS) $(DESTDIR)$(prefix)/include

clean:
	rm -rf *.[ao] *.so *~ testdb
	rm -f ../liblmdb.a
	rm -f ../../include/lmdb.h

liblmdb.a:	mdb.o midl.o
	ar rs $@ mdb.o midl.o
	cp $@ ../
	cp lmdb.h ../../include/

liblmdb.so:	mdb.o midl.o
	$(CC) $(LDFLAGS) -pthread -shared -o $@ mdb.o midl.o $(SOLIBS)

mdb.o: mdb.c lmdb.h midl.h
	$(CC) $(CFLAGS) -fPIC $(CPPFLAGS) -c mdb.c

midl.o: midl.c midl.h
	$(CC) $(CFLAGS) -fPIC $(CPPFLAGS) -c midl.c

%:	%.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o:	%.c lmdb.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
