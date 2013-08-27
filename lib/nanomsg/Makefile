HEADERS	=	src/nn.h \
		src/pair.h \
		src/reqrep.h \
		src/pubsub.h \
		src/survey.h \
		src/pipeline.h \
		src/bus.h \
		src/inproc.h \
		src/ipc.h \
		src/tcp.h

.PHONY: all clean lib

lib:
	cd build; cmake ..; make
	mv build/libnanomsg.so* ../
	if [ ! -d ../../include/nanomsg ]; then mkdir ../../include/nanomsg; fi
	cp ${HEADERS} ../../include/nanomsg

all: clean lib

clean:
	cd build; rm -rf *
	rm -f ../libnanomsg.so*
	rm -rf ../../include/nanomsg

