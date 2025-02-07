#include "mbed.h"
 
DigitalOut myled(LED1);
DigitalIn mybutton(BUTTON1); 
 
int main() {
    bool mybutton_last = 0;
    while (1) {
        bool mybutton_current = mybutton;
        if (mybutton_last == 0 && mybutton_current == 1) {
            myled = !myled;
        }
        mybutton_last = mybutton_current;
    }
}