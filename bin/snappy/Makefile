NAME	= scmd
IPATH = -I. -I../../include
CFLAGS := $(IPATH) -Wall -g -O2 -DNDEBUG=1 -DSG=1
# Remove -DSG=1 if you don't need scather-gather support
# NDEBUG=1 is recommended for production

.PHONY: all clean

all: clean $(NAME)

clean: 
	rm -f *.o $(NAME) ../$(NAME)

$(NAME): scmd.o map.o util.o ../../lib/libsnappy.a
	cp $(NAME) ../
