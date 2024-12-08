#include <avr/io.h>
#include "motorr.h"
#include "pwm.h"  
#if !defined(__AVR_ATmega328P__)
#include <avr/iom328p.h>
#endif

unsigned char MotorArray[][2] = {
    {2, 3}  
};

void DC_Init() {
  
    DDRD |= (1 << MotorArray[0][0]);
    DDRD |= (1 << MotorArray[0][1]);

    PWM_init(); 
}

void DC_Start(unsigned char MotorID, tenuDirection Direction, unsigned char Speed) {
    if (Direction == clockwise) {
        PORTD |= (1 << MotorArray[MotorID][0]);
        PORTD &= ~(1 << MotorArray[MotorID][1]);
    } else {
        PORTD &= ~(1 << MotorArray[MotorID][0]);
        PORTD |= (1 << MotorArray[MotorID][1]);
    }

    Set_DutyCycle(Speed); 
}

void DC_Stop(unsigned char MotorID) {
    // Disable motor movement by setting direction pins low
    PORTD &= ~(1 << MotorArray[MotorID][0]);
    PORTD &= ~(1 << MotorArray[MotorID][1]);

    // Set duty cycle to 0 to stop PWM
    Set_DutyCycle(0);
}
