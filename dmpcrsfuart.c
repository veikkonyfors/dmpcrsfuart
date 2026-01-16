/*
 * dmpcrsfuart.c
 *
 *  Created on: Jan 15, 2026
 *      Author: pappa
 */

int main(int argc, char *argv[]) {
	const char *uart_port = "/dev/pts/4";
    int baudrate = 420000;

    printf("CRSF UART reader\n");
    printf("UART port: %s\n", uart_port);
    printf("Baud rate: %d\n", baudrate);
    printf("Press Ctrl+C to stop\n\n");

    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Init channels
    for (int i = 0; i < 16; i++) {
        channels[i] = 1000;
    }
    channels[0] = 1500; // Roll mid
    channels[1] = 1500; // Pitch mid
    channels[3] = 1500; // Yaw mid

    if (uart_init(uart_port, baudrate) != 0) {
        fprintf(stderr, "UART init failed\n");
        return 1;
    }

    struct timespec loop_start, loop_end;
    const long loop_delay_ns = 4000000; // 4ms = 250Hz

    printf("Start CRSF reading 250Hz...\n");

    while (running) {
        clock_gettime(CLOCK_MONOTONIC, &loop_start);

        // 1. Päivitä kanavien arvot (simulaatio)
        update_channels_simulation();

        // 2. Tulosta kanavat (debug, 10Hz)
        static int print_counter = 0;
        if (++print_counter >= 25) {
            print_counter = 0;
            print_channels();
        }

        // 3. Lähetä CRSF-data
        send_crsf_data();

        // 4. Odota täsmälleen 4ms
        clock_gettime(CLOCK_MONOTONIC, &loop_end);

        long elapsed_ns = (loop_end.tv_sec - loop_start.tv_sec) * 1000000000L +
                         (loop_end.tv_nsec - loop_start.tv_nsec);

        if (elapsed_ns < loop_delay_ns) {
            long sleep_ns = loop_delay_ns - elapsed_ns;
            struct timespec sleep_time = {
                .tv_sec = sleep_ns / 1000000000L,
                .tv_nsec = sleep_ns % 1000000000L
            };
            nanosleep(&sleep_time, NULL);
        } else {
            printf("Liian hidas! %ld ns myöhässä\n", elapsed_ns - loop_delay_ns);
        }
    }

    // Siivous
    printf("\nSuljetaan UART...\n");
    if (uart_fd >= 0) {
        close(uart_fd);
    }

    pthread_mutex_destroy(&channels_mutex);
    printf("Ohjelma lopetettu\n");

    return 0;
}
