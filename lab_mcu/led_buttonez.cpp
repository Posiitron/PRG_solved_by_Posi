#include "mbed.h"
 
DigitalOut myled(LED1);
DigitalIn mybutton(BUTTON1); 
 
int main() {
   while (1) {
      if (mybutton == 0) {
         myled = 1;
      } else {
         myled = 0;
      }
   }
}