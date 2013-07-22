# ###################################
# #            MAKEFILE             #
# ###################################

# Executable's name
NAME	=	finedb
# Source files
SRC	=	main.c			\
		self_path.c		\
		finedb.c		\
		database.c		\
		server.c		\
		writer_thread.c		\
		connection_thread.c	\
		command_get.c		\
		command_put.c		\
		command_del.c		\
		command_list.c

# ###################################################################

# Paths to header files
IPATH	= -I. -I../include
# Path to libraries and lib's names
LDPATH	= -L. -L../lib -llmdb -lnanomsg -lsnappy -ly -lpthread -lrt -Wl,-rpath -Wl,'$$ORIGIN/../lib'
# Compiler options
EXEOPT	= -O3 # -g for debug

# ###################################################################

CC	= gcc
OBJS	= $(SRC:.c=.o)

# Objects compilation options
CFLAGS	= -ansi -std=c99 -pedantic-errors -Wall -Wextra -Wmissing-prototypes \
	  -Wno-long-long -Wno-unused-parameter -D_GNU_SOURCE -D_THREAD_SAFE \
	  $(IPATH) $(EXEOPT)

# Link options
LDFLAGS	= $(EXEOPT) $(LDPATH)

# ###################################################################

.PHONY: clean all

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	strip $(NAME)

clean:
	rm -f $(NAME) $(OBJS) *~

all: clean $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c $<

