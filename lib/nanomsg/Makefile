HEADERS	=	src/nn.h \
		src/pair.h \
		src/reqrep.h \
		src/pubsub.h \
		src/survey.h \
		src/fanin.h \
		src/fanout.h \
		src/bus.h \
		src/inproc.h \
		src/ipc.h \
		src/tcp.h

.PHONY: all clean lib

lib:
	cd build; cmake ..; make
	cp build/libnanomsg.a ../
	mkdir ../../include/nanomsg
	cp ${HEADERS} ../../include/nanomsg

all: clean lib

clean:
	cd build; rm -rf *
	rm -f ../libnanomsg.a
	rm -rf ../../include/nanomsg

