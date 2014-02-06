#!/bin/sh
export LD_LIBRARY_PATH=librf24
./PiGatewayTCP&
sleep 5
node ./js/gatewayTCP.js&


