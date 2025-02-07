#include "mbed.h"
 
DigitalOut myled(LED1);
DigitalIn mybutton(BUTTON1); 
 
int main() {
    bool button_last = 0;
    while (1) {
        bool button_current = mybutton;
        if (button_last == 0 && button_current == 1) {
            ThisThread::sleep_for(100ms); // Efficient MCU sleep.
            if (mybutton) {
                myled = !myled;
            }
        }
        button_last = button_current;
    }
}