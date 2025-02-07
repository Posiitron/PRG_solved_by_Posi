#include "mbed.h"

DigitalOut myled(LED1);
BufferedSerial serial(USBTX, USBRX, 115200);
Ticker ticker;

void blink(uint32_t i, Kernel::Clock::duration_u32 t = 50ms) {
  for (; i > 0; i--) {
    myled = true;
    ThisThread::sleep_for(t);
    myled = false;
    ThisThread::sleep_for(t);
  }
}

void flick() { myled = !myled; }

int main() {
  serial.set_blocking(true);
  const char init_msg = 'i';
  serial.write(&init_msg, 1);
  blink(5);
  while (true) {
    char received_value;
    ssize_t received_in_fact = serial.read(&received_value, 1);
    char acknowledge_msg = 'a';
    if (received_in_fact == 1) {
      switch (received_value) {
      case 's':
        myled = true;
        ticker.detach();
        break;
      case 'e':
        myled = false;
        ticker.detach();
        break;
      case 'f':
        myled = !myled;
        break;
      case '0':
        ticker.detach();
        break;
      case '1':
        ticker.attach(&flick, 50ms);
        break;
      case '2':
       ticker.attach(&flick, 100ms);
        break;
      case '3':
      ticker.attach(&flick, 200ms);
        break;
      case '4':
        ticker.attach(&flick, 500ms);
        break;
      case '5':
        ticker.attach(&flick, 1000ms);
        break;
      default:
        acknowledge_msg = '!';
      }
    } else {
      acknowledge_msg = '*';
    }
    serial.write(&acknowledge_msg, 1);
  }
}
