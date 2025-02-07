#include "mbed.h"
 
DigitalOut myled(LED1);
InterruptIn mybutton(BUTTON1);
 
void on_button_fall(){
    myled = !myled;
}
 
int main() {
    mybutton.fall(&on_button_fall);
    ThisThread::sleep_for(1s);
}