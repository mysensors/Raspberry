#!/bin/sh
export LD_LIBRARY_PATH=librf24
./RadioGateway&
sleep 5
node ./js/NodeGateway.js&


