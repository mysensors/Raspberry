#!/bin/sh
node ./js/gateway.js&
sleep 5 
export LD_LIBRARY_PATH=librf24
./PiGateway&

