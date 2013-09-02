# #####################################################################
#                              Makefile
# #####################################################################

# Library's name (libNAME.so)
SONAME   =	liblinenoise.so

# Name of source files (names.c)
SRC      =	linenoise.c

# Name of header files (names.h)
INCLUDES =	linenoise.h

# #####################################################################

# Paths to header files
IPATH   =       -I.
# Path to libraries and lib's names
LDPATH  =       -L.
# Compiler options
EXEOPT  =       -O2 # -g for debug

# #####################################################################

CC      =	gcc
RM      =	/bin/rm -f
OBJS    =	$(SRC:.c=.o)

# Objects compilation options
CFLAGS  =	-pedantic -Wall -Wmissing-prototypes \
		  -Wno-long-long $(IPATH) -D_GNU_SOURCE \
		  -D_LARGEFILE_SOURCE -D_THREAD_SAFE -fPIC -std=c9x
CFLAGS_CYGWIN =	-Wall -Wmissing-prototypes -Wno-long-long $(IPATH) \
		  -D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_THREAD_SAFE
# Link options
LDFLAGS =	-shared $(EXEOPT) $(LDPATH) -Wl,-soname,$(SONAME)

# #####################################################################

.PHONY: clean all

$(SONAME): $(OBJS) $(SRC)
	$(CC) $(OBJS) $(LDFLAGS) -o $(SONAME)
	cp $(SONAME) ../
	cp $(INCLUDES) ../../include/

clean:
	$(RM) $(OBJS) $(SONAME) *~
	rm -f ../$(SONAME)
	cd ../../include ; rm -f $(INCLUDES)

all: clean $(SONAME)

.c.o:
	$(CC) $(CFLAGS) -c $<
