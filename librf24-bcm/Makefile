#############################################################################
#
# Makefile for librf24-bcm on Raspberry Pi
#
# License: GPL (General Public License)
# Author:  Charles-Henri Hallard 
# Date:    2013/03/13 
#
# Description:
# ------------
# use make all and mak install to install the library 
# You can change the install directory by editing the LIBDIR line
#
PREFIX=/usr/local

IOBASE := $(shell cat /proc/iomem | grep bcm2708_vcio | cut -f 1 -d "-")
PIREV := $(shell cat /proc/cpuinfo | grep Revision | cut -f 2 -d ":" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$$//')

# Library parameters
# where to put the lib
LIBDIR=$(PREFIX)/lib
# lib name 
LIB=librf24-bcm
# shared library name
LIBNAME=$(LIB).so.1.0

# Where to put the header files
HEADER_DIR=${PREFIX}/include/RF24

# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -mtune=arm1176jzf-s
ifeq (${PIREV},$(filter ${PIREV},a01041 a21041))
	CCFLAGS += -march=armv7-a
else
	CCFLAGS += -march=armv6zk
endif
CCFLAGS += -D BCM2835_PERI_BASE=0x${IOBASE}

# make all
# reinstall the library after each recompilation
all: librf24-bcm

# Make the library
librf24-bcm: RF24.o bcm2835.o 
	g++ -shared -Wl,-soname,$@.so.1 ${CCFLAGS} -o ${LIBNAME} $^
	
# Library parts
RF24.o: RF24.cpp
	g++ -Wall -fPIC ${CCFLAGS} -c $^

bcm2835.o: bcm2835.c
	gcc -Wall -fPIC ${CCFLAGS} -c $^

# clear build files
clean:
	rm -rf *.o ${LIB}.*

install: all install-libs install-headers

# Install the library to LIBPATH
install-libs: 
	@echo "[Installing Libs]"
	@if ( test ! -d $(PREFIX)/lib ) ; then mkdir -p $(PREFIX)/lib ; fi
	@install -m 0755 ${LIBNAME} ${LIBDIR}
	@ln -sf ${LIBDIR}/${LIBNAME} ${LIBDIR}/${LIB}.so.1
	@ln -sf ${LIBDIR}/${LIBNAME} ${LIBDIR}/${LIB}.so
	@ldconfig

install-headers:
	@echo "[Installing Headers]"
	@if ( test ! -d ${HEADER_DIR} ) ; then mkdir -p ${HEADER_DIR} ; fi
	@install -m 0644 *.h ${HEADER_DIR}
