CC	= gcc
W	= -W -Wall -Wno-unused-parameter -Wbad-function-cast
OPT = -O2 -g
CFLAGS	= -pthread $(OPT) $(W) $(XCFLAGS)
LDFLAGS = -L../../lib -llmdb
IPATH   = -I../../include
LDLIBS	=
SOLIBS	=

PROGS	= mdb_stat mdb_copy

.PHONY: all clean bin

all:	clean bin

clean:
	rm -rf $(PROGS) *.[ao] *~
	rm -f ../mdb_stat ../mdb_copy

bin: $(PROGS)
	cp mdb_stat ../
	cp mdb_copy ../

mdb_stat: mdb_stat.o ../../lib/liblmdb.a

mdb_copy: mdb_copy.o ../../lib/liblmdb.a

%:	%.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o:	%.c
	$(CC) $(CFLAGS) $(IPATH) -c $<
