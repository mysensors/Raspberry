#Wiring the NRF	24L01+ radio

|NRF24l01+|Rpi Header Pin|
|---|---|
|GND|25|
|VCC|17|
|CE|22|
|CSN|24|
|SCK|23|
|MOSI|19|
|MISO|21|
|IRQ|--|

#Building & Installing

##RF24 library
* Download the library from https://github.com/TMRh20/RF24
 * Either an official release(tested with 1.1.3) or clone the master branch.
* Decompress(if needed) and change to the library directory
* Run `make all` followed by `sudo make install`

##Serial Gateway

The standard configuration will build the Serial Gateway with a tty name of
'/dev/ttyMySensorsGateway' and PTS group ownership of 'tty' the PTS will be group read
and write. The default install location will be /usr/local/sbin. If you want to change
that edit the variables in the head of the Makefile.

###Build the Gateway
* Clone this repository
* Change to the Raspberry directory
* Run `make all` followed by `sudo make install`
* (if you want to start daemon at boot) sudo make enable-gwserial

For some controllers a more recognisable name needs to be used: e.g. /dev/ttyUSB020 (check if this is free).

`sudo ln -s /dev/ttyMySensorsGateway /dev/ttyUSB20`

To automatically create the link on startup, add `ln -s /dev/ttyMySensorsGateway /dev/ttyUSB20` just before `exit0` in `/etc/rc.local`

#Uninstalling

* Change to Raspberry directory
* Run `sudo make uninstall`

Support: http://forum.mysensors.org
