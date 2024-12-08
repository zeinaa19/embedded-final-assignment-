#include <util/delay.h>
#include "analogdig.h"
#include <EEPROM.h>
#include "Lcd.h"
#include "Uart.h"
#include "pwm.h"
#include "motorr.h"
#include <stdlib.h>

int addresslow = 0;
int addressup = 5;

int main(void) {
    unsigned short sensorreading;
    unsigned short upper = EEPROM.read(addressup);
    unsigned short lower = EEPROM.read(addresslow);
    unsigned char buffer[4];

    // Initialize modules
    Adc_Init();
    Uart_Init();
    LCD_Init();
    buttoninit();

    while (1) {
        _delay_ms(200);  // Short delay for smooth execution

        // Read sensor value
        sensorreading = (Adc_ReadChannel(1)*150*5)/(1023*1.5);
        itoa(sensorreading, buffer, 10);
        Uart_SendString(buffer, 4);
        Uart_SendChar('\n');

        // Display sensor and limit values on LCD
        LCD_Clear();
        LCD_String("TempVal: ");
        LCD_String(buffer);
        LCD_String("C ");
        LCD_Command(0xC0);
        itoa(upper, buffer, 10);
        LCD_String("U: ");
        LCD_String(buffer);
        LCD_String("  L: ");
        itoa(lower, buffer, 10);
        LCD_String(buffer);

        if (((PIND >> PD0) & 1) == 0) {
            upper =upper+3;
            EEPROM.update(addressup, upper);
            _delay_ms(300);
        }

        // Button 2 (Decrease limit)
        if (((PINB >> PB5) & 1) == 0) {
            lower =lower+3;
            EEPROM.update(addresslow, lower);
            _delay_ms(300);
        }

        // Button 3 (Switch limit control)
        if (((PINB >> PB4) & 1) == 0) {
            upper=0;
            lower=0;
            EEPROM.update(addressup, upper);
            EEPROM.update(addresslow, lower);
            _delay_ms(300);
        }

        if (sensorreading >= lower && sensorreading < upper) {
            LCD_Command(0x8E);
            LCD_String("OK");
            DC_Start(0, clockwise, 100);  // Start motor clockwise
            _delay_ms(500);  // Motor runs for 500ms
            DC_Stop(0);
            _delay_ms(1000);
        } else {
            LCD_Command(0x8D);
            LCD_String("NOK");
            DC_Start(0, anticc, 100);  // Start motor counterclockwise
            _delay_ms(500);  // Motor runs for 500ms
            DC_Stop(0);
            _delay_ms(1000);
        }
        
    }
    return 0;
}

void buttoninit(){
  DDRD &= ~(1 << PD0);  // PD0 as input (Increase limit)
  PORTD |= (1 << PD0);  // Enable pull-up resistor for PD0
  DDRB &= ~(1 << PB4);  // PB4 as input (Decrease limit)
  PORTB |= (1 << PB4);  // Enable pull-up resistor for PB4
  DDRB &= ~(1 << PB5);  // PB5 as input (Switch between limits)
  PORTB |= (1 << PB5);  // Enable pull-up resistor for PB5
}
