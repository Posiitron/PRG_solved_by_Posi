#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "prg_serial.h"
void set_raw(_Bool set)
{
    if (set) {
        system("stty raw -echo"); // enable raw, disable echo
    } else {
        system("stty -raw echo"); // disable raw, enable echo
    }
}
int main(int argc, char *argv[])
{
    int ret = 0;
    const char *serial = argc > 1 ? argv[1] : "/dev/ttyACM0";
    fprintf(stderr, "INFO: open serial port at %s\n", serial);
    int fd = serial_open(serial);
    set_raw(1);
    if (fd != -1) { // read from serial port
        _Bool quit = false;
        while (!quit) {
            int c = getchar();
            _Bool write_read_response = false;
            switch (c) {
            case 'q':
                printf("Quit the program\n");
                quit = true;
                break;
            case 's':
                printf("Send 's' - LED on\n");
                write_read_response = true;
                break;
            case 'e':
                printf("Send 'e' - LED off\n");
                write_read_response = true;
                break;
            case '0':
                printf("Send '0' - LED  keep\n");
                write_read_response = true;
                break;
            case '1':
                printf("Send '1' - LED 50ms\n");
                write_read_response = true;
                break;
            case '2':
                printf("Send '2' - LED 100ms\n");
                write_read_response = true;
                break;
            case '3':
                printf("Send '3' - LED 200ms\n");
                write_read_response = true;
                break;
            case '4':
                printf("Send '4' - LED 500ms\n");
                write_read_response = true;
                break;
            case '5':
                printf("Send '5' - LED 1000ms\n");
                write_read_response = true;
                break;
            case 'f':
                printf("Send 'f' - LED switch\n");
                write_read_response = true;
                break;
            default:
                printf("Ignoring char '%d'\n", c);
                break;
            }
            if (write_read_response) {
                int r = serial_putc(fd, c);
                if (r != -1) {
                    fprintf(stderr, "DEBUG: Received response '%d'\n", r);
                } else {
                    fprintf(stderr, "ERROR: Error in received responses\n");
                }
            }
        }
        serial_close(fd);
    } else {
        fprintf(stderr, "ERROR: Cannot open device %s\n", serial);
    }
    set_raw(0);
    return ret;
}