/*
 * PiGatewaySerial.cpp - MySensors Gateway for wireless node providing a serial interface
 *
 * Copyright 2014 Tomas Hozza <thozza@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <pty.h>
#include <termios.h>
#include <poll.h>
#include <sys/stat.h>

#include <RF24.h>
#include <MyGateway.h>
#include <Version.h>

/* variable indicating if the server is still running */
volatile static int running = 1;

/* PTY file descriptors */
int pty_master = -1;
int pty_slave = -1;

static const mode_t ttyPermissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
static const char *serial_tty = "/dev/ttyMySensorsGateway";

/*
 * handler for SIGINT signal
 */
void handle_sigint(int sig)
{
	printf("Received SIGINT\n");
	running = 0;
}


/*
 * callback function writting data from RF24 module to the PTY
 */
void write_msg_to_pty(char *msg)
{
	size_t len = 0;

	if (msg == NULL)
	{
		printf("[callback] NULL msg received!\n");
		return;
	}
	
	len = strlen(msg);
	write(pty_master, msg, len);
}


/*
 * configure PTY master FD
 */
void configure_master_fd(int fd)
{
	struct termios settings;

	tcgetattr(fd, &settings);
	/* turn off ECHO of written characters */
	settings.c_lflag &= ~ECHO;
	tcsetattr(fd, 0, &settings);
}


/*
 * Main gateway logic
 */
int main(int argc, char **argv)
{
	struct pollfd fds;
	MyGateway *gw = NULL;
	int status = EXIT_SUCCESS;
	int ret;

	printf("Starting PiGatewaySerial...\n");
	printf("Protocol version - %s\n", LIBRARY_VERSION);

	/* register the signal handler */
	signal(SIGINT, handle_sigint);
	signal(SIGTERM, handle_sigint);

	/* create MySensors Gateway object */
#ifdef __PI_BPLUS
	gw = new MyGateway(RPI_BPLUS_GPIO_J8_15, RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ, 60);
#else
	gw = new MyGateway(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ, 60);
#endif	
	if (gw == NULL)
	{
		printf("Could not create MyGateway! (%d) %s\n", errno, strerror(errno));
		status = EXIT_FAILURE;
		goto cleanup;
	}

	/* create PTY - Pseudo TTY device */
	ret = openpty(&pty_master, &pty_slave, NULL, NULL, NULL);
	if (ret != 0) 
	{
		printf("Could not create a PTY! (%d) %s\n", errno, strerror(errno));
		status = EXIT_FAILURE;
		goto cleanup;
	}
	ret = chmod(ttyname(pty_slave),ttyPermissions);
	if (ret != 0) 
	{
		printf("Could not change PTY permissions! (%d) %s\n", errno, strerror(errno));
		status = EXIT_FAILURE;
		goto cleanup;
	}
	printf("Created PTY '%s'\n", ttyname(pty_slave));
	
	/* create a symlink with predictable name to the PTY device */
	unlink(serial_tty);	// remove the symlink if it already exists
	ret = symlink(ttyname(pty_slave), serial_tty);
	if (ret != 0)
	{
		printf("Could not create a symlink '%s' to PTY! (%d) %s\n", serial_tty, errno, strerror(errno));
    	status = EXIT_FAILURE;
        goto cleanup;
	}
	printf("Gateway tty: %s\n", serial_tty);

	close(pty_slave);
	configure_master_fd(pty_master);

	fds.events = POLLRDNORM;
	fds.fd = pty_master;

	/* we are ready, initialize the Gateway */
	gw->begin(RF24_PA_LEVEL_GW, RF24_CHANNEL, RF24_DATARATE, &write_msg_to_pty);

	/* Do the work until interrupted */
	while(running)
	{
		/* process radio msgs */
		gw->processRadioMessage();
		
		/* process serial port msgs */
		ret = poll(&fds, 1, 500);
		if (ret == -1)
		{
			printf("poll() error (%d) %s\n", errno, strerror(errno));
		}
		else if (ret == 0)
		{
			/* timeout */
			continue;
		}
		else
		{
			if (fds.revents & POLLRDNORM)
			{
				char buff[256];
				ssize_t size;

				fds.revents = 0;
				size = read(pty_master, buff, sizeof(buff));
				if (size < 0)
				{
					printf("read error (%d) %s\n", errno, strerror(errno));
					continue;
				}
				buff[size] = '\0';
				
				gw->parseAndSend(buff);
			}
		}
	}


cleanup:
	printf("Exiting...\n");
	if (gw)
		delete(gw);
	(void) unlink(serial_tty);
	return status;
}
