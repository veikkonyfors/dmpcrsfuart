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

#include "crsf.h"
#include "uart.h"

static volatile bool running = true;
extern int uart_fd;  // From libuart.a


static void signal_handler(int sig) {
    printf("Got signal %d, closing...\n", sig);
    running = false;
}

int main(int argc, char *argv[]) {
	const char *uart_port = "/tmp/ttyV1";
    int baudrate = 420000;

    printf("CRSF UART reader\n");
    printf("UART port: %s\n", uart_port);
    printf("Baud rate: %d\n", baudrate);
    printf("Press Ctrl+C to stop\n\n");

    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    if (uart_init(uart_port, baudrate) != 0) {
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
