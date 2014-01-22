#############################################################################
#
# Compiles Mysensors Gateway for Raspberry Pi. Uses the RF24 library
# Install the RF24 library by source:
#
# git clone https://github.com/stanleyseow/RF24.git
# cd RF24
# cd librf24-rpi/librf24
# make
# make PiGatewayPipe
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
PROGRAMS = Gateway Relay Sensor PiGateway
GATEWAY  = PiGateway
SOURCES = ${PROGRAMS:=.cpp}
OBJS = ${PROGRAMS:=.o}
DEPS = ${PROGRAMS:=.h}
RF24H = /home/pi/RF24/librf24-rpi/librf24/

all: ${OBJS} 


%.o: %.cpp ${DEPS}
	${CC} -c -g -o $@ $< ${CCFLAGS} -I${RF24H}

${GATEWAY}: ${OBJS}
	${CC} -o $@ ${OBJS} ${CCFLAGS} -I${RF24H} -lrf24

clean:
	rm -rf $(PROGRAMS)

