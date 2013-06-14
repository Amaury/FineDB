CFLAGS := -Wall -g -O2 -DNDEBUG=1  -DSG=1
# Remove -DSG=1 if you don't need scather-gather support
# NDEBUG=1 is recommended for production

.PHONY: all clean lib

lib: snappy.o
	ar -r libsnappy.a snappy.o
	ranlib libsnappy.a
	cp libsnappy.a ../
	cp snappy.h ../../include/

all: clean lib

snappy.o: snappy.c

clean: 
	rm -f snappy.o libsnappy.a
	rm -f ../libsnappy.a
	rm -f ../../include/snappy.h

