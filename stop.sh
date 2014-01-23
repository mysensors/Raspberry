#!/bin/sh

kill -9 `ps -ef | grep PiGateway | grep -v grep | awk '{print $2}'`

kill -9 `ps -ef | grep gateway.js | grep -v grep | awk '{print $2}'`
