#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E2LL;

int led = 0;

int led1 = 6;
int led2 = 7;

void setup(void)
{
  SPI.begin;
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT); 
}

void loop(void)
{
  if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read( &led, sizeof(led) );
      
      if(led==1)
      {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, LOW);
      }
      else if(led==2)
      {
        digitalWrite(led1, LOW);
        digitalWrite(led2, HIGH);
      }
      else
      {
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
      }
  }
    }
}
