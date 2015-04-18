/*
 The MySensors library adds a new layer on top of the RF24 library.
 It handles radio network routing, relaying and ids.

 Created by Henrik Ekblad <henrik.ekblad@gmail.com>
 12/10/14 - Ported to Raspberry Pi by OUJABER Mohamed <m.oujaber@gmail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
#include "MyMessage.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __Raspberry_Pi
	#define min(a,b) (a<b?a:b)

    char * itoa(int value, char *result, int base);
    char * ltoa(long value, char *result, int base);
    char * dtostrf(float f, int width, int decimals, char *result);
    char * utoa( unsigned int num, char *str, int radix);
	char * ultoa( unsigned long num, char *str, int radix);
#endif

MyMessage::MyMessage() {
	destination = 0; // Gateway is default destination
}

MyMessage::MyMessage(uint8_t _sensor, uint8_t _type) {
	destination = 0; // Gateway is default destination
	sensor = _sensor;
	type = _type;
}

bool MyMessage::isAck() const {
	return miGetAck();
}

/* Getters for payload converted to desired form */
void* MyMessage::getCustom() const {
	return (void *)data;
}

const char* MyMessage::getString() const {
	uint8_t payloadType = miGetPayloadType();
	if (payloadType == P_STRING) {
		return data;
	} else {
		return NULL;
	}
}

// handles single character hex (0 - 15)
char MyMessage::i2h(uint8_t i) const {
	uint8_t k = i & 0x0F;
	if (k <= 9)
		return '0' + k;
	else
		return 'A' + k - 10;
}

char* MyMessage::getCustomString(char *buffer) const {
	for (uint8_t i = 0; i < miGetLength(); i++)
	{
		buffer[i * 2] = i2h(data[i] >> 4);
		buffer[(i * 2) + 1] = i2h(data[i]);
	}
	buffer[miGetLength() * 2] = '\0';
	return buffer;
}

char* MyMessage::getStream(char *buffer) const {
	uint8_t cmd = miGetCommand();
	if ((cmd == C_STREAM) && (buffer != NULL)) {
		return getCustomString(buffer);
	} else {
		return NULL;
	}
}

char* MyMessage::getString(char *buffer) const {
	uint8_t payloadType = miGetPayloadType();
	if (payloadType == P_STRING) {
		strncpy(buffer, data, miGetLength());
		buffer[miGetLength()] = 0;
		return buffer;
	} else if (buffer != NULL) {
		if (payloadType == P_BYTE) {
			itoa(bValue, buffer, 10);
		} else if (payloadType == P_INT16) {
			itoa(iValue, buffer, 10);
		} else if (payloadType == P_UINT16) {
			utoa(uiValue, buffer, 10);
		} else if (payloadType == P_LONG32) {
			ltoa(lValue, buffer, 10);
		} else if (payloadType == P_ULONG32) {
			ultoa(ulValue, buffer, 10);
		} else if (payloadType == P_FLOAT32) {
			dtostrf(fValue,2,fPrecision,buffer);
		} else if (payloadType == P_CUSTOM) {
			return getCustomString(buffer);
		}
		return buffer;
	} else {
		return NULL;
	}
}

uint8_t MyMessage::getByte() const {
	if (miGetPayloadType() == P_BYTE) {
		return data[0];
	} else if (miGetPayloadType() == P_STRING) {
		return atoi(data);
	} else {
		return 0;
	}
}

bool MyMessage::getBool() const {
	return getInt();
}

float MyMessage::getFloat() const {
	if (miGetPayloadType() == P_FLOAT32) {
		return fValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atof(data);
	} else {
		return 0;
	}
}

long MyMessage::getLong() const {
	if (miGetPayloadType() == P_LONG32) {
		return lValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atol(data);
	} else {
		return 0;
	}
}

unsigned long MyMessage::getULong() const {
	if (miGetPayloadType() == P_ULONG32) {
		return ulValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atol(data);
	} else {
		return 0;
	}
}

int MyMessage::getInt() const {
	if (miGetPayloadType() == P_INT16) { 
		return iValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atoi(data);
	} else {
		return 0;
	}
}

unsigned int MyMessage::getUInt() const {
	if (miGetPayloadType() == P_UINT16) { 
		return uiValue;
	} else if (miGetPayloadType() == P_STRING) {
		return atoi(data);
	} else {
		return 0;
	}

}

MyMessage& MyMessage::setType(uint8_t _type) {
	type = _type;
	return *this;
}

MyMessage& MyMessage::setSensor(uint8_t _sensor) {
	sensor = _sensor;
	return *this;
}

MyMessage& MyMessage::setDestination(uint8_t _destination) {
	destination = _destination;
	return *this;
}

// Set payload
MyMessage& MyMessage::set(void* value, uint8_t length) {
	miSetPayloadType(P_CUSTOM);
	miSetLength(length);
	memcpy(data, value, min(length, MAX_PAYLOAD));
	return *this;
}

MyMessage& MyMessage::set(const char* value) {
	uint8_t length = min(strlen(value), MAX_PAYLOAD);
	miSetLength(length);
	miSetPayloadType(P_STRING);
	strncpy(data, value, length);
	return *this;
}

MyMessage& MyMessage::set(uint8_t value) {
	miSetLength(1);
	miSetPayloadType(P_BYTE);
	data[0] = value;
	return *this;
}

MyMessage& MyMessage::set(float value, uint8_t decimals) {
	miSetLength(5); // 32 bit float + persi
	miSetPayloadType(P_FLOAT32);
	fValue=value;
	fPrecision = decimals;
	return *this;
}

MyMessage& MyMessage::set(unsigned long value) {
	miSetPayloadType(P_ULONG32);
	miSetLength(4);
	ulValue = value;
	return *this;
}

MyMessage& MyMessage::set(long value) {
	miSetPayloadType(P_LONG32);
	miSetLength(4);
	lValue = value;
	return *this;
}

MyMessage& MyMessage::set(unsigned int value) {
	miSetPayloadType(P_UINT16);
	miSetLength(2);
	uiValue = value;
	return *this;
}

MyMessage& MyMessage::set(int value) {
	miSetPayloadType(P_INT16);
	miSetLength(2);
	iValue = value;
	return *this;
}

#ifdef __Raspberry_Pi

/**
* C++ version 0.4 char* style "itoa":
* Written by Lukás Chmela
* Released under GPLv3.
*/
char *itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }
	
	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;
	
	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );
	
	// Apply negative sign
	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char *ltoa(long value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	long tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

char *dtostrf(float f, int width, int decimals, char *result)
{
    char widths[3];
    char decimalss[3];  
    char format[100];
    itoa(width,widths,10);
    itoa(decimals,decimalss,10);
    strcpy(format,"%");
    strcat(format,widths);
    strcat(format,".");
    strcat(format,decimalss);
    strcat(format,"f");
  
    sprintf(result,format,f);
    return result;
}


/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the software nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
char *utoa(unsigned int num, char *str, int radix)
{
  unsigned int value;
  char *sp = str;
  char *sp2;
  
  value = num;

  /* Store sign at start of buffer for negative base-10 values */
  if (10 == radix && 0 > num) {
    *sp++ = '-';
    value = -num;
  }

  sp2 = sp;

  do {
    char rem = value % radix;
    value /= radix;
    if (10 > rem) {
      *sp++ = '0' + rem;
    } else {
      *sp++ = 'A' + rem - 10;
    }
  } while (0 < value);

  /* Mark end of string */
  *sp-- = 0;

  /* Reverse string contents (excluding sign) in place */
  while (sp2 < sp) {
    char tmp = *sp2;
    *sp2++ = *sp;
    *sp-- = tmp;
  }

  return str;
}

char *ultoa(unsigned long num, char *str, int radix)
{
  unsigned long value;
  char *sp = str;
  char *sp2;
  
  value = num;

  /* Store sign at start of buffer for negative base-10 values */
  if (10 == radix && 0 > num) {
    *sp++ = '-';
    value = -num;
  }

  sp2 = sp;

  do {
    char rem = value % radix;
    value /= radix;
    if (10 > rem) {
      *sp++ = '0' + rem;
    } else {
      *sp++ = 'A' + rem - 10;
    }
  } while (0 < value);

  /* Mark end of string */
  *sp-- = 0;

  /* Reverse string contents (excluding sign) in place */
  while (sp2 < sp) {
    char tmp = *sp2;
    *sp2++ = *sp;
    *sp-- = tmp;
  }

  return str;
}

#endif
