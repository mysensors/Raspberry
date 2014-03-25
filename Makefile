#############################################################################
#
# Compiles Mysensors Gateway for Raspberry Pi. 
#
# make
# make RadioGateway
# sudo make install 
#
# The valiable RF24H below should point to the directory for the RF24 header
# files
#
# License: GPL (General Public License)
#
#
prefix := /opt/mysensors

CC=g++
CCFLAGS=-Wall -Ofast -mfpu=vfp -DDEBUG -DRPI -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

# define all programs
PROGRAMS = Gateway Relay Sensor RadioGateway
GATEWAY  = RadioGateway
SOURCES = ${PROGRAMS:=.cpp}
OBJS = ${PROGRAMS:=.o}
DEPS = ${PROGRAMS:=.h}
RF24H=librf24

all: ${GATEWAY} 

%.o: %.cpp ${DEPS}
	${CC} -c -g -o $@ $< ${CCFLAGS} -I${RF24H}

${GATEWAY}: RF24 MYSENSORS ${OBJS}
	${CC} -o $@ ${OBJS} ${CCFLAGS} -I${RF24H} librf24/librf24.so.1

RF24:	
	$(MAKE) -C librf24
MYSENSORS:
	$(MAKE) -C ${MYSENSORSH}
clean:
	rm -rf $(PROGRAMS)

