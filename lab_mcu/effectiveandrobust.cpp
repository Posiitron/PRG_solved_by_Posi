#include "mbed.h"
 
DigitalOut myled(LED1);
DigitalIn mybutton_read(BUTTON1);
InterruptIn mybutton_irq(BUTTON1);
Timeout timeout;
 
void on_timeout(){
    if(mybutton_read == 0){
        myled = !myled;
    }
}
 
void on_button_fall(){
    timeout.attach(&on_timeout, 1s);
}
 
int main() {
    mybutton_irq.fall(&on_button_fall);
    while(1){
        ThisThread::sleep_for(1s);
    }
}
