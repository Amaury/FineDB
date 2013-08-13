# ###################################
# #            MAKEFILE             #
# ###################################

# Executable's name
NAME	=	finedb-server
# Source files
SRC	=	main.c				\
		self_path.c			\
		finedb.c			\
		database.c			\
		server.c			\
		writer_thread.c			\
		connection_thread.c		\
		command_setdb.c			\
		command_get.c			\
		command_del.c			\
		command_put.c			\
		command_list.c			\
		command_drop.c			\
		command_start_commit_rollback.c	\
		command_ping.c

# ###################################################################

# Paths to header files
IPATH	= -I. -I../../include
# Path to libraries and lib's names
LDPATH	= -L. -L../../lib -llmdb -lnanomsg -lsnappy -ly -lpthread -lrt -Wl,-rpath -Wl,'$$ORIGIN/../lib'
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
	mv $(NAME) ../../bin/

clean:
	rm -f $(NAME) $(OBJS) *~ ../../bin/$(NAME)

all: clean $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c $<

