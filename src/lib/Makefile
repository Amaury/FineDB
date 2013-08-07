SONAME		= libfinedb.so
SRC		= libfinedb.c
INCLUDES	= libfinedb.h

# ###################################################################

# Paths to header files
IPATH   = -I. -I../../include -I../server
# Path to libraries and lib's names
LDPATH  = -L. -L../../lib -ly -l snappy
# Compiler options
EXEOPT  = -O2 # -g for debug

# ###################################################################

OBJS	= $(SRC:.c=.o)

# Objects compilation options
CFLAGS  = -ansi -std=c99 -pedantic-errors -Wall -Wextra -Wmissing-prototypes \
          -Wno-long-long -Wno-unused-parameter -D_GNU_SOURCE -D_THREAD_SAFE \
          -fPIC $(IPATH) $(EXEOPT)

# Link options
LDFLAGS = $(EXEOPT) $(LDPATH) -shared -Wl,-soname,$(SONAME)

# ###################################################################

.PHONY: all clean

$(SONAME): $(OBJS) $(SRC)
	$(CC) $(OBJS) $(LDFLAGS) -o $(SONAME)
	mv $(SONAME) ../../lib/
	cp $(INCLUDES) ../../include/

all: clean $(SONAME)

clean:
	rm -f $(SONAME) ../../lib/$(SONAME) $(OBJS)
	rm -f ../../include/$(INCLUDES)

.c.o:
	$(CC) $(CFLAGS) -c $<

