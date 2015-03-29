##########################################################################
# Configurable options                                                   #
##########################################################################
# Install Base location
PREFIX=/usr/local
# Bin Dir
BINDIR=$(PREFIX)/sbin
# Set the name of predictable tty
TTY_NAME := /dev/ttyMySensorsGateway
# Set the group name for the raw tty
TTY_GROUPNAME := tty
##########################################################################
# Please do not change anything below this line                          #
##########################################################################
CC=g++
# get PI Revision from cpuinfo
PIREV := $(shell cat /proc/cpuinfo | grep Revision | cut -f 2 -d ":" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$$//')
CCFLAGS=-Wall -Ofast -mfpu=vfp -lpthread -g -D__Raspberry_Pi -mfloat-abi=hard -mtune=arm1176jzf-s -D_TTY_NAME=\"${TTY_NAME}\" -D_TTY_GROUPNAME=\"${TTY_GROUPNAME}\"

ifeq (${PIREV},$(filter ${PIREV},a01041 a21041))
	# a01041 and a21041 are PI 2 Model B and armv7
	CCFLAGS += -march=armv7-a 
else
	# anything else is armv6
	CCFLAGS += -march=armv6zk
endif

ifeq (${PIREV},$(filter ${PIREV},a01041 a21041 0010))
	# a01041 and a21041 are PI 2 Model B with BPLUS Layout and 0010 is Pi Model B+ with BPLUS Layout
	CCFLAGS += -D__PI_BPLUS
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

install: all install-gatewayserial install-gateway install-initscripts

install-gatewayserial: 
	@echo "Installing ${GATEWAY_SERIAL} to ${BINDIR}"
	@install -m 0755 ${GATEWAY_SERIAL} ${BINDIR}

install-gateway: 
	@echo "Installing ${GATEWAY} to ${BINDIR}"
	@install -m 0755 ${GATEWAY} ${BINDIR}
	
install-initscripts:
	@echo "Installing initscripts to /etc/init.d"
	@install -m 0755 initscripts/PiGatewaySerial /etc/init.d
	@install -m 0755 initscripts/PiGateway /etc/init.d
	@echo "Installing syslog config to /etc/rsyslog.d"
	@install -m 0755 initscripts/30-PiGatewaySerial.conf /etc/rsyslog.d
	@install -m 0755 initscripts/30-PiGateway.conf  /etc/rsyslog.d
	@service rsyslog restart

enable-gw: install
	@update-rc.d PiGateway defaults
	
enable-gwserial: install
	@update-rc.d PiGatewaySerial defaults
	
remove-gw:
	@update-rc.d -f PiGateway remove
	
remove-gwserial:
	@update-rc.d -f PiGatewaySerial remove
	
uninstall: remove-gw remove-gwserial
	@echo "Stopping daemon PiGatewaySerial (ignore errors)"
	-@service PiGatewaySerial stop
	@echo "Stopping daemon PiGateway (ignore errors)"
	-@service PiGateway stop
	@echo "removing files"
	rm ${BINDIR}/PiGatewaySerial ${BINDIR}/PiGateway /etc/init.d/PiGatewaySerial /etc/init.d/PiGateway /etc/rsyslog.d/30-PiGatewaySerial.conf /etc/rsyslog.d/30-PiGateway.conf
	
