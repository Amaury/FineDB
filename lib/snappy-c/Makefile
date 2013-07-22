SONAME		= libsnappy.so
SRC		= snappy.c
INCLUDES	= snappy.h

CFLAGS := -Wall -g -O2 -DNDEBUG=1  -DSG=1 -fPIC
# Remove -DSG=1 if you don't need scather-gather support
# NDEBUG=1 is recommended for production

# Link options
LDFLAGS	= -shared -Wl,-soname,$(SONAME)

CC	= gcc
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

