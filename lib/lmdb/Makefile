SONAME		= liblmdb.so
SRC		= mdb.c \
		  midl.c
INCLUDES	= lmdb.h

CC	= gcc
W	= -W -Wall -Wno-unused-parameter -Wbad-function-cast
OPT	= -O2 -g
CFLAGS	= -pthread $(OPT) $(W) $(XCFLAGS) -fPIC
# link options
LDFLAGS	= -shared -Wl,-soname,$(SONAME)

OBJS	= $(SRC:.c=.o)

.PHONY: all clean lib

lib: $(OBJS) $(SRC)
	$(CC) $(OBJS) $(LDFLAGS) -o $(SONAME)
	cp $(SONAME) ../
	cp $(INCLUDES) ../../include/

all: clean lib

clean:
	rm -f $(OBJS) $(SONAME) *~
	rm -f ../$(SONAME)
	cd ../../include ; rm -f $(INCLUDES)

.c.o:
	$(CC) $(CFLAGS) -c $<

