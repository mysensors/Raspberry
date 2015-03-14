/*
Copyright (c) 2011-2014 Roger Light <roger@atchoo.org>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of mosquitto nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include "mosquitto_internal.h"
#include "net_mosq.h"

void *_mosquitto_thread_main(void *obj);

int mosquitto_loop_start(struct mosquitto *mosq)
{
#ifdef WITH_THREADING
	if(!mosq || mosq->threaded) return MOSQ_ERR_INVAL;

	mosq->threaded = true;
	pthread_create(&mosq->thread_id, NULL, _mosquitto_thread_main, mosq);
	return MOSQ_ERR_SUCCESS;
#else
	return MOSQ_ERR_NOT_SUPPORTED;
#endif
}

int mosquitto_loop_stop(struct mosquitto *mosq, bool force)
{
#ifdef WITH_THREADING
#  ifndef WITH_BROKER
	char sockpair_data = 0;
#  endif

	if(!mosq || !mosq->threaded) return MOSQ_ERR_INVAL;


	/* Write a single byte to sockpairW (connected to sockpairR) to break out
	 * of select() if in threaded mode. */
	if(mosq->sockpairW != INVALID_SOCKET){
#ifndef WIN32
		if(write(mosq->sockpairW, &sockpair_data, 1)){
		}
#else
		send(mosq->sockpairW, &sockpair_data, 1, 0);
#endif
	}
	
	if(force){
		pthread_cancel(mosq->thread_id);
	}
	pthread_join(mosq->thread_id, NULL);
	mosq->thread_id = pthread_self();
	mosq->threaded = false;

	return MOSQ_ERR_SUCCESS;
#else
	return MOSQ_ERR_NOT_SUPPORTED;
#endif
}

#ifdef WITH_THREADING
void *_mosquitto_thread_main(void *obj)
{
	struct mosquitto *mosq = obj;

	if(!mosq) return NULL;

	pthread_mutex_lock(&mosq->state_mutex);
	if(mosq->state == mosq_cs_connect_async){
		pthread_mutex_unlock(&mosq->state_mutex);
		mosquitto_reconnect(mosq);
	}else{
		pthread_mutex_unlock(&mosq->state_mutex);
	}

	if(!mosq->keepalive){
		/* Sleep for a day if keepalive disabled. */
		mosquitto_loop_forever(mosq, mosq->keepalive*1000*86400, 1);
	}else{
		/* Sleep for our keepalive value. publish() etc. will wake us up. */
		mosquitto_loop_forever(mosq, mosq->keepalive*1000, 1);
	}

	return obj;
}
#endif

