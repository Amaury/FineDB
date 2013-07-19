.PHONY: all clean finedb-cli

all: clean finedb-cli

clean:
	rm -f finedb-cli

finedb-cli: finedb-cli.c
	gcc finedb-cli.c -I../../include -I../../src ../../lib/liby.a -o finedb-cli

