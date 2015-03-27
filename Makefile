CC=g++
PIREV := $(shell cat /proc/cpuinfo | grep Revision | cut -f 2 -d ":" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$$//')
CCFLAGS=-Wall -Ofast -mfpu=vfp -lpthread -g -D__Raspberry_Pi -mfloat-abi=hard -mtune=arm1176jzf-s
ifeq (${PIREV},a01041)
	CCFLAGS += -march=armv7-a -D__PI_2
else
	CCFLAGS += -march=armv6zk
endif

# define all programs
PROGRAMS = MyGateway MySensor MyMessage PiEEPROM
GATEWAY  = PiGateway
GATEWAY_SERIAL = PiGatewaySerial

GATEWAY_SRCS = ${GATEWAY:=.cpp}
GATEWAY_SERIAL_SRCS = ${GATEWAY_SERIAL:=.cpp}
SOURCES = ${PROGRAMS:=.cpp}

GATEWAY_OBJS = ${GATEWAY:=.o}
GATEWAY_SERIAL_OBJS = ${GATEWAY_SERIAL:=.o}
OBJS = ${PROGRAMS:=.o}

GATEWAY_DEPS = ${GATEWAY:=.h}
GATEWAY_SERIAL_DEPS = ${GATEWAY_SERIAL:=.h}
DEPS = ${PROGRAMS:=.h}

RF24H = librf24-bcm/
CINCLUDE=-I. -I${RF24H}


all: ${GATEWAY} ${GATEWAY_SERIAL}

%.o: %.cpp %.h ${DEPS}
	${CC} -c -o $@ $< ${CCFLAGS} ${CINCLUDE}

${GATEWAY}: ${OBJS} ${GATEWAY_OBJS}
	${CC} -o $@ ${OBJS} ${GATEWAY_OBJS} ${CCFLAGS} ${CINCLUDE} -lrf24-bcm

${GATEWAY_SERIAL}: ${OBJS} ${GATEWAY_SERIAL_OBJS}
	${CC} -o $@ ${OBJS} ${GATEWAY_SERIAL_OBJS} ${CCFLAGS} ${CINCLUDE} -lrf24-bcm -lutil

clean:
	rm -rf $(PROGRAMS) $(GATEWAY) $(GATEWAY_SERIAL) ${OBJS} $(GATEWAY_OBJS) $(GATEWAY_SERIAL_OBJS)

