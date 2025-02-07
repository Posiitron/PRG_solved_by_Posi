#include "mbed.h"
 
DigitalOut myled(LED1);
DigitalIn mybutton(BUTTON1);
 
// This will define the memory footprint and the total time window captured by the program.
constexpr uint32_t BUFFER_SIZE = 500;
constexpr Kernel::Clock::duration_u32 TIME_UNIT = 10ms;
constexpr uint32_t RECORDING_SIGNAL_BLINK_COUNT = 0;
 
// Here we will store what the user "wrote" using the button.
bool buffer [BUFFER_SIZE];
 
// A generic blinking function to show the user mode changes. Note that this BLOCKS the program.
void blink(uint32_t i, Kernel::Clock::duration_u32 t = 50ms){
    for(; i > 0; i--){
        myled = true;
        ThisThread::sleep_for(t);
        myled = false;
        ThisThread::sleep_for(t);
    }
}
 
void record(){
    for(uint32_t i = 0; i < BUFFER_SIZE; i++){
        // Take one sample from the middle of the sampling period.
        ThisThread::sleep_for(TIME_UNIT/2);
        buffer[i] = !mybutton;
        ThisThread::sleep_for(TIME_UNIT/2);
        // Tell the user that the sample was taken. Note, that this blocks!
        blink(RECORDING_SIGNAL_BLINK_COUNT); 
    }
}
 
void replay(){
    for(uint32_t i = 0; i < BUFFER_SIZE; i++){
        myled = buffer[i];
        ThisThread::sleep_for(TIME_UNIT);
    }
    myled=false; // Turn of the light after the show.
}
 
int main() {
    blink(6); 
    record();
    blink(6);
    replay();
    blink(6);
    // The program ends. Use the black button to restart the MCU.
}