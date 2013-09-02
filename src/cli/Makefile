NAME	= finedb-cli
SRC	= finedb-cli.c

# ###################################################################

# Paths to header files
IPATH   = -I. -I../../include -I../server
# Path to libraries and lib's names
LDPATH  = -L. -L../../lib -lsnappy -ly -lfinedb -llinenoise -lrt -Wl,-rpath -Wl,'$$ORIGIN/../lib'
# Compiler options
EXEOPT  = -O2 # -g for debug

# ###################################################################

OBJS	= $(SRC:.c=.o)

# Objects compilation options
CFLAGS  = -ansi -std=c99 -pedantic-errors -Wall -Wextra -Wmissing-prototypes \
          -Wno-long-long -Wno-unused-parameter -D_GNU_SOURCE -D_THREAD_SAFE \
          $(IPATH) $(EXEOPT)

# Link options
LDFLAGS = $(EXEOPT) $(LDPATH)

# ###################################################################

.PHONY: all clean

all: clean $(NAME)

clean:
	rm -f $(NAME) ../$(NAME) $(OBJS) ../../bin/$(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	strip $(NAME)
	mv $(NAME) ../../bin/

.c.o:
	$(CC) $(CFLAGS) -c $<

