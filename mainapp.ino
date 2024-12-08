#include <util/delay.h>  // including delay library to allow delays in the code
#include "analogdig.h" // including analog to digital converter file
#include <EEPROM.h> // including EEPROM library
#include "Lcd.h" // including LCD display file
#include "Uart.h" // Including UART file
#include "pwm.h" // including pwm file
#include "motorr.h" // including motor control file
#include <stdlib.h> // including library which is used for later conversions

int addresslow = 0; //defines storage address for EEPROM lower limit 
int addressup = 5;   //defines storage address for EEPROM upper limit 

int main(void) { // beginning of main code that returns a integer and doesn't require arguments
    unsigned short sensorreading; // defining "sensorreading" as a variable that only holds positive integers and holds numbers up to 16 bits
    unsigned short upper = EEPROM.read(addressup); //define variable "upper" to store value in addressup of EEPROM
    unsigned short lower = EEPROM.read(addresslow); //define variable "lower" to store value in addresslow of EEPROM
    unsigned char buffer[4]; // Define the buffer as an unsigned character that holds only +ve integers to store 4 elements

    // Initialize modules
    Adc_Init(); // initialization for ADC
    Uart_Init(); // initialization for UART
    LCD_Init(); // initialization for LCD
    buttoninit(); // initialization for buttons

    while (1) { // using while (1) as an infinite continuous loop
        _delay_ms(200);  // Short delay for smooth execution

        // Read sensor value
        sensorreading = (Adc_ReadChannel(1)*150*5)/(1023*1.5);  //Read the ADC value from channel 1, convert to temperature, and store in sensorreading
        itoa(sensorreading, buffer, 10); //Convert the temperature reading to a string and store it in buffer
        Uart_SendString(buffer, 4); //  Send the temperature string through UART
        Uart_SendChar('\n'); //  this sends a newline character to indicate the end of the data

        // Display sensor and limit values on LCD
        LCD_Clear(); // clear LCD
        LCD_String("TempVal: "); //displays the word"TempVal:"
        LCD_String(buffer); //displays the value stored in the buffer for TempVal
        LCD_String("C "); // displays degree celcius

        
        LCD_Command(0xC0); // command is used to shift to the second row of the LCD
        itoa(upper, buffer, 10); // converts the upper limit value to a string so it can be displayed on the LCD
        LCD_String("U: "); // This displays "U:"
        LCD_String(buffer); //displays the value stored in the buffer for the upper limit
        LCD_String("  L: "); // displays "L:"
        itoa(lower, buffer, 10); // converts the Lower limit value to a string so it can be displayed on the LCD
        LCD_String(buffer); // /displays the value stored in the buffer for the upper limit

        if (((PIND >> PD0) & 1) == 0) { //this if contition is used to check if the button is pressed or not
            upper =upper+3; // if the button is pressed the upper limit will increase by 3
            EEPROM.update(addressup, upper); // this updates the value stored in the eeprom, it makes the new adressup =  upper
            _delay_ms(300); // we added a delay of 300 milliseconds to ensure the code runs smoothly
        }

        
        if (((PINB >> PB5) & 1) == 0) { //this if contition is used to check if the button is pressed or not
            lower =lower+3; // if the button is pressed the lower limit will increase by 3
            EEPROM.update(addresslow, lower); // this updates the value stored in the eeprom, it makes the new adresslow = lower
            _delay_ms(300); // we added a delay of 300 milliseconds to ensure the code runs smoothly
        }

        
        if (((PINB >> PB4) & 1) == 0) { { //this if contition is used to check if the button is pressed or not
            upper=0; // if button is pressed makes upper = 0
            lower=0; // if button is pressed makes lower = 0
            EEPROM.update(addressup, upper); // this updates the value stored in the eeprom, it makes the new adressup =  upper
            EEPROM.update(addresslow, lower); // this updates the value stored in the eeprom, it makes the new adresslow = lower
            _delay_ms(300); // we added a delay of 300 milliseconds to ensure the code runs smoothly
        }

        if (sensorreading >= lower && sensorreading < upper) { //this checks if the tempereture sensor value is inside the range or not
            LCD_Command(0x8E);  // moves curser to a specified charecter in this case the 15th
            LCD_String("OK"); //displays "OK" if the value is in the range
            DC_Start(0, clockwise, 100);  // Start motor conected to channel 0 clockwise 
            _delay_ms(500);  // Motor runs for 500ms
            DC_Stop(0); // motor stops
            _delay_ms(1000); // mottor stops for 1000ms
        } else {
            LCD_Command(0x8D); // moves curser to a specified charecter in this case the 14th
            LCD_String("NOK"); //displays "NOK" if the value is not in the range
            DC_Start(0, anticc, 100);  // / Start motor conected to channel 0 anticlockwise 
            _delay_ms(500);  // Motor runs for 500ms
            DC_Stop(0); // motor stops
            _delay_ms(1000); // mottor stops for 1000ms
        }
        
    }
    return 0;
}

void buttoninit(){
  DDRD &= ~(1 << PD0);  // PD0 as input 
  PORTD |= (1 << PD0);  // Enable pull-up resistor for PD0
  DDRB &= ~(1 << PB4);  // PB4 as input
  PORTB |= (1 << PB4);  // Enable pull-up resistor for PB4
  DDRB &= ~(1 << PB5);  // PB5 as input 
  PORTB |= (1 << PB5);  // Enable pull-up resistor for PB5
}
