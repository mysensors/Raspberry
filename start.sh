#!/bin/sh
node ./js/gateway.js&
sleep 1
export LD_LIBRARY_PATH=librf24
./PiGateway&

