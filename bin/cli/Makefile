NAME	= finedb-cli
SRC	= finedb-cli.c

# ###################################################################

# Paths to header files
IPATH   = -I. -I../../include -I../../src
# Path to libraries and lib's names
LDPATH  = -L. -L../../lib -ly -Wl,-rpath -Wl,'$$ORIGIN/../lib'
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

.PHONY: all clean finedb-cli

all: clean $(NAME)

clean:
	rm -f $(NAME) ../$(NAME) $(OBJS)

finedb-cli: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	strip $(NAME)
	cp $(NAME) ../

.c.o:
	$(CC) $(CFLAGS) -c $<

