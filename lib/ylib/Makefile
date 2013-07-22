# #####################################################################
#                              Makefile
# #####################################################################

# Library's name
SONAME =	liby.so

# Name of source files
SRC =		yvect.c		\
		ystr.c		\
		ylog.c		\
		ybin.c		\
		ydynabin.c	\
		ylist.c

# Name of header files
INCLUDES =	ydefs.h		\
		yerror.h	\
		ystr.h		\
		ylog.h		\
		yvect.h		\
		ybin.h		\
		ydynabin.h	\
		ylist.h

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
CFLAGS  =	-ansi -std=c99 -pedantic -Wall -Wextra -Wmissing-prototypes \
		  -Wno-long-long -Wno-pointer-arith $(IPATH) -D_GNU_SOURCE \
		  -D_LARGEFILE_SOURCE -D_THREAD_SAFE -fPIC
# Link options
LDFLAGS = -shared -Wl,-soname,$(SONAME)

# #####################################################################

.PHONY: clean all

$(SONAME): $(OBJS) $(SRC)
	$(CC) $(OBJS) $(LDFLAGS) -o $(SONAME)
	cp $(SONAME) ../
	cp $(INCLUDES) ../../include/

clean:
	$(RM) $(OBJS) $(SONAME) *~
	$(RM) ../$(SONAME)
	cd ../../include ; $(RM) $(INCLUDES)

all: clean $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c $<

