# #####################################################################
#                              Makefile
# #####################################################################

# Library's static name (libNAME.a)
NAME	=	liby.a

# Name of source files (names.c)
SRC       =	yvect.c		\
		ystr.c		\
		ylog.c		\
		ybin.c		\
		ydynabin.c	\
		ylist.c

# Name of header files (names.h)
HEADS     =	ydefs.h		\
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

# #####################################################################

.PHONY: clean all

$(NAME): $(OBJS) $(SRC)
	ar -r $(NAME) $(OBJS)
	ranlib $(NAME)
	cp $(NAME) ../
	cp $(HEADS) ../../include/

clean:
	$(RM) $(OBJS) $(NAME) *~
	rm -f ../$(NAME)
	for i in $(HEADS); do rm -f ../../include/$$i; done

all: clean $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c $<

