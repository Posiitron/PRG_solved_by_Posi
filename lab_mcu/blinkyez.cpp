#include "mbed.h"
 
DigitalOut myled (LED1); // "Handle" to the LED.
 
int main() {
    while (1) {
        myled = !myled; // Flip the state of the LED.
        ThisThread::sleep_for(1s); // Efficient MCU sleep.
    }
}