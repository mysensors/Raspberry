CC=g++
CCFLAGS=-Wall -Ofast -mfpu=vfp -lpthread -g -D__Raspberry_Pi -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

# define all programs
PROGRAMS = MyGateway MySensor MyMessage PiEEPROM MyMQTT
GATEWAY  = PiGateway
GATEWAY_SERIAL = PiGatewaySerial
GATEWAY_MOSQ = PiGatewayMosquitto

GATEWAY_SRCS = ${GATEWAY:=.cpp}
GATEWAY_SERIAL_SRCS = ${GATEWAY_SERIAL:=.cpp}
GATEWAY_MOSQ_SRCS = ${GATEWAY_MOSQ:=.cpp}
SOURCES = ${PROGRAMS:=.cpp}

GATEWAY_OBJS = ${GATEWAY:=.o}
GATEWAY_SERIAL_OBJS = ${GATEWAY_SERIAL:=.o}
GATEWAY_MOSQ_OBJS = ${GATEWAY_MOSQ:=.o}
OBJS = ${PROGRAMS:=.o}

GATEWAY_DEPS = ${GATEWAY:=.h}
GATEWAY_SERIAL_DEPS = ${GATEWAY_SERIAL:=.h}
GATEWAY_MOSQ_DEPS = ${GATEWAY_MOSQ:=.h}
DEPS = ${PROGRAMS:=.h}

RF24H = librf24-bcm/
CINCLUDE=-I. -I${RF24H}


all: ${GATEWAY} ${GATEWAY_SERIAL} ${GATEWAY_MOSQ}

%.o: %.cpp %.h ${DEPS}
	${CC} -c -o $@ $< ${CCFLAGS} ${CINCLUDE}

${GATEWAY}: ${OBJS} ${GATEWAY_OBJS}
	${CC} -o $@ ${OBJS} ${GATEWAY_OBJS} ${CCFLAGS} ${CINCLUDE} -lrf24-bcm

${GATEWAY_SERIAL}: ${OBJS} ${GATEWAY_SERIAL_OBJS}
	${CC} -o $@ ${OBJS} ${GATEWAY_SERIAL_OBJS} ${CCFLAGS} ${CINCLUDE} -lrf24-bcm -lutil


${GATEWAY_MOSQ}: ${OBJS} ${GATEWAY_MOSQ_OBJS}
	${CC} -o $@ ${OBJS} ${GATEWAY_MOSQ_OBJS} ${CCFLAGS} ${CINCLUDE} -lrf24-bcm -lmosquitto -lwiringPi

clean:
	rm -rf $(PROGRAMS) $(GATEWAY) $(GATEWAY_SERIAL) $(GATEWAY_MOSQ) $(BUILDDIR)/${OBJS}

