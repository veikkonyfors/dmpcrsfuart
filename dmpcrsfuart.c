/*
 * dmpcrsfuart.c
 *
 *  Created on: Jan 15, 2026
 *      Author: pappa
 */

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "crsf.h"
#include "uart.h"

static volatile bool running = true;
extern int uart_fd;  // From libuart.a


static void signal_handler(int sig) {
    printf("Got signal %d, closing...\n", sig);
    running = false;
}

/**
 * @file dmpcrsfuart.c
 *
 * Created on: Jan 6, 2026
 * Author: pappa
 *
 * @brief command line program to dump crsf messages read from given UART port.
 * @param -t UART port read crsf messages from (default /tmp/ttyV1).
 * @param -b Baudrate (default 420000).
 * @return 0.
 * @note To be run on RPI connected to drone's FC UART.
 * @note Never ending loop to be terminated with SIGINT or SIGKILL.
 * @note Receiver accepts UDP connection from any IP address.
 */
int main(int argc, char *argv[]) {

	const char *tty = "/tmp/ttyV1";
    int baudrate = 420000;
    int opt;

	while ((opt = getopt(argc, argv, "t:b:")) != -1) {
		switch (opt) {
			case 't':
				tty = optarg;
				break;
			case 'b':
				baudrate = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-t UART] [-b baudrate] \n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

    printf("CRSF UART reader\n");
    printf("UART port: %s\n", tty);
    printf("Baud rate: %d\n", baudrate);
    printf("Press Ctrl+C to stop\n\n");

    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    if (uart_init(tty, baudrate) != 0) {
        fprintf(stderr, "UART init failed\n");
        return 1;
    }

    printf("Start CRSF reading 250Hz...\n");

#define MAX_FRAME_SIZE 256
    uint8_t frame[MAX_FRAME_SIZE];
    size_t size_of_frame;
    char crsf_frame_as_string[2048];

    while (running) {

        // Read next frame
        size_of_frame = uart_read_frame(frame, MAX_FRAME_SIZE);
        printf("Read frame of %ld bytes of type 0x%02x\n", size_of_frame, crsf_get_frame_type(frame));
        crsf_to_string(frame, crsf_frame_as_string, sizeof(crsf_frame_as_string));
        printf("%s", crsf_frame_as_string);
    }

    printf("\nClose UART...\n");
    if (uart_fd >= 0) {
        close(uart_fd);
    }

    printf("Exit\n");

    return 0;
}
