#include "Lcd.h"
#include "adc.h"
#include "motor.h"
#include <util/delay.h>
#include <stdlib.h>

int main(void) {
  buttoninit();
  Adc_Init();
  LCD_Init();
  float temperature;
  unsigned short adcsensor;
  unsigned short upper=0;
  unsigned short lower=0;
  unsigned char buffer[6];
  
while (1) {
  _delay_ms(200);

  adcsensor=Adc_ReadChannel(1);
  temperature=adcsensor*(5.0/1024.0)*100;

  dtostrf(temperature,5,1,buffer);
  if (((PINB>>PB4)&1)==0){
    lower+=5;
    _delay_ms(200);
  if (((PINB>>PB5)&1)==0){
    upper+=5;
    _delay_ms(200);
  if (((PIND>>PD0)&1)==0){
    upper=0;
    lower=0;
  }
LCD_Clear();
LCD_Command(0x80);
LCD_String("Temp:");
LCD_String(buffer);
LCD_String("C");

LCD_Command(0xC0);
LCD_String("L:");
itoa(lower,buffer,10);
LCD_String(buffer);
LCD_String('U');
itoa(upper,buffer,10);
LCD_String(buffer);




