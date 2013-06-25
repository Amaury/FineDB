.PHONY: all clean test_put test_get

all: clean test_put test_get

clean:
	rm -f test_put test_get

test_put: test_put.c
	gcc test_put.c -o test_put

test_get: test_get.c
	gcc test_get.c -o test_get

