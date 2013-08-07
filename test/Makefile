SRC	= test_lmdb.c

IPATH   = -I. -I../include
LDPATH  = -L. -L../lib -llmdb -lnanomsg -lsnappy -ly -lpthread -lrt -Wl,-rpath -Wl,'$$ORIGIN/../lib'

CFLAGS	= -ansi -std=c99 -pedantic-errors -Wall -Wextra -Wmissing-prototypes \
	  -Wno-long-long -Wno-unused-parameter -D_GNU_SOURCE -D_THREAD_SAFE \
	  $(IPATH) $(EXEOPT)

LDFLAGS = $(EXEOPT) $(LDPATH)

OBJS    = $(SRC:.c=.o)

.PHONY: all clean test_put test_get

all: clean test_lmdb

clean:
	rm -f test_lmdb $(OBJS)

test_lmdb: $(OBJS)
	gcc $(OBJS) $(LDFLAGS) -o test_lmdb

.c.o:
	gcc $(CFLAGS) -c $<

