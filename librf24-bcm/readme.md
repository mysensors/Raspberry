# Optimized Raspberry Pi RF24 and RF24 Network Libraries  

 General Documentation: http://tmrh20.github.io  
   
 Library functions are mostly the same.  
 See the included examples for RPi specific usage  
 
## Raspberry Pi - PreConfig
    
### Possible pre-configuration:  
If SPI is not already enabled, load it on boot:   

    sudo raspi-config  

A. Update the tool via the menu as required  
B. Select Advanced and enable the SPI kernel module      
C. Update other software and libraries:  

     sudo apt-get update  
     sudo apt-get upgrade  


# RPi - RF24 Quick-Start  
     
A. Make a directory to contain the RF24 and possibly RF24Network lib and enter it:  

    mkdir ~/rf24libs  
    cd ~/rf24libs  

B.  Clone the RF24 Repo  

    git clone https://github.com/tmrh20/RF24.git rtemp  

C.  Copy the RPi library folder to the current directory, and delete the rest  

    mv rtemp/RPi/RF24 ./  
    rm -r rtemp  
    cd RF24  

D. Build the library, and run an example file:  

    sudo make install
    cd examples  
    make
    sudo ./gettingstarted  
  

# RPi - RF24Network Quick-Start  

A. Enter the same directory that contains the RF24 library folder  

    cd ~/rf24libs  

B. Clone the RF24Network Repo  

    git clone https://github.com/tmrh20/RF24Network.git ntemp  

C. Copy the RF24Network folder to the current directory, and delete the rest  

    mv ntemp/RPi/RF24Network ./  
    rm -r ntemp  
    cd RF24Network  

D. Build the library  

    sudo make install
    cd examples  
    make  
    sudo ./helloworld_rx   OR   sudo ./helloworld_tx  
  

# Connection Info

Using pin 15/GPIO 22 for CE, pin 24/GPIO8 (CE0) for CSN

Can use either RPi CE0 or CE1 pins for radio CSN.
Choose any RPi output pin for radio CE pin.

**Constructor:**

    RF24 radio(RPI_V2_GPIO_P1_15,BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);
    or
    RF24 radio(RPI_V2_GPIO_P1_15,BCM2835_SPI_CS1, BCM2835_SPI_SPEED_8MHZ);
	
	RPi B+:
	RF24 radio(RPI_BPLUS_GPIO_J8_15,RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ);
	or
	RF24 radio(RPI_BPLUS_GPIO_J8_15,RPI_BPLUS_GPIO_J8_26, BCM2835_SPI_SPEED_8MHZ);

**Pins:**  

| PIN | NRF24L01 |    RPI     | RPi -P1 Connector |
|-----|----------|------------|-------------------|
|  1  |   GND    | rpi-gnd    |     (25)          |
|  2  |   VCC    | rpi-3v3    |     (17)          |
|  3  |   CE     | rpi-gpio22 |     (15)          |
|  4  |   CSN    | rpi-gpio8  |     (24)          |
|  5  |   SCK    | rpi-sckl   |     (23)          |
|  6  |   MOSI   | rpi-mosi   |     (19)          |
|  7  |   MISO   | rpi-miso   |     (21)          |
|  8  |   IRQ    |    -       |       -           |
  
  
See http://www.airspayce.com/mikem/bcm2835/index.html for BCM2835 class documentation.  
Note: The BCM library has been customized slightly to allow use of hardware CE pins not
in use for SPI, and to include a millis() function.  
   
****************
  
Based on the arduino lib from J. Coliz <maniacbug@ymail.com>.  
the library was berryfied by Purinda Gunasekara <purinda@gmail.com>.  
then forked from github stanleyseow/RF24 to https://github.com/jscrane/RF24-rpi  
Network lib also based on https://github.com/farconada/RF24Network

Currently optimized and aligned with Arduino fork of libraries by TMRh20:  
https://github.com/tmrh20/RF24/RPi and https://github.com/tmrh20/RF24Network/RPi  
Documentation: http://tmrh20.github.io


