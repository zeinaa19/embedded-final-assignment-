#include <util/delay.h>
#include "analogdigital.h"
#include "Lcd.h"
#include "DC_Motor.h"
#include <stdlib.h>


void buttoninit() {
    DDRB = DDRB & ~(1 << PB4);
    PORTB = PORTB | (1 << PB4);
    DDRB = DDRB & ~(1 << PB5);
    PORTB = PORTB | (1 << PB5);
    DDRD &= ~(1 << PD0);
    PORTD |= (1 << PD0);
}

int uaddress = 0 ;
int laddress = 0 ;

int main (void) {
    buttoninit();
    Adc_Init();
    LCD_Init();
    float temperature;
    unsigned short adcsensor;
    unsigned short upper = EEPROM.read(uaddress);
    unsigned short lower = EEPROM.read(laddress);
    unsigned char buffer[6];
    
    while (1) {
        _delay_ms(200);

        adcsensor = Adc_ReadChannel(1);
        temperature = adcsensor * (5.0 / 1024.0) * 100;

        dtostrf(temperature, 5, 1, buffer);

        if (((PINB >> PB4) & 1) == 0) {
            lower += 5;
            EEPROM.update(laddress, lower);
            _delay_ms(200);
        }
        if (((PINB >> PB5) & 1) == 0) {
            upper += 5;
            EEPROM.update(uaddress, upper);
            _delay_ms(200);
        }
        if (((PIND >> PD0) & 1) == 0) {
          upper = 0;
          lower = 0;
        }

        LCD_Clear();
        LCD_Command(0x80);
        LCD_String("Temp:");
        LCD_String(buffer);
        LCD_String("C");

        LCD_Command(0xC0);
        LCD_String("L:");
        itoa(lower, buffer, 10);
        LCD_String(buffer);
        LCD_String("U:");
        itoa(upper, buffer, 10);
        LCD_String(buffer);

        if (temperature >= lower && temperature < upper) {
            LCD_Command(0x8E);
            LCD_String("OK");
            DC_Start(0, DIRECTION_CW, 0);
            _delay_ms(1000);
            DC_Stop(0);
            _delay_ms(1000);
        } else {

            LCD_Command(0x8D);
            LCD_String("Nok");
            DC_Start(0, DIRECTION_CCW, 0);
            _delay_ms(1000);
            DC_Stop(0);
            _delay_ms(1000);
        }
    }

    return 0;
}

