CC=g++
CCFLAGS=-Wall -Ofast -mfpu=vfp -lpthread -g -D__Raspberry_Pi -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

# define all programs
PROGRAMS = MyGateway MySensor MyMessage PiGateway
GATEWAY  = PiGateway
SOURCES = ${PROGRAMS:=.cpp}
OBJS = ${PROGRAMS:=.o}
DEPS = ${PROGRAMS:=.h}
RF24H = librf24-bcm/

all: ${GATEWAY} 

%.o: %.cpp ${DEPS}
	${CC} -c -o $@ $< ${CCFLAGS} -I${RF24H}

${GATEWAY}: ${OBJS}
	${CC} -o $@ ${OBJS} ${CCFLAGS} -I${RF24H} -lrf24-bcm

clean:
	rm -rf $(PROGRAMS) $(BUILDDIR)/${OBJS}

