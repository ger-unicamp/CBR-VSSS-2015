#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(9,10);

// Network uses that radio
RF24Network network(radio);

// Address of our node
const uint16_t master = 0;

// Address of the robots
const uint16_t rob1 = 1;
const uint16_t rob2 = 2;
const uint16_t rob3 = 3;

// Structure of our payload
struct message
{
  int16_t dirR;
  int16_t speedR;
  int16_t dirL;
  int16_t speedL;
};

void setup(void)
{
  Serial.begin(9600);
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 66, /*node address*/ master);
}

void loop(void)
{
  
  char buffer[20];
  int rob;
  message order;
      
  //Pump the network regularly
  network.update();
    
  if(Serial.available())
  {
    Serial.readBytesUntil( 'E', buffer, 20);

    int n = sscanf(buffer, "S%d#%d#%d#%d#%dE", &rob, &(order.dirR), &(order.speedR), &(order.dirL), &(order.speedL));
    if(n==5)
    {
      
      Serial.print("RECEIVED: ");
      Serial.print(rob);
      Serial.print("  ");
      Serial.print(order.dirR);
      Serial.print("  ");
      Serial.print(order.speedR);
      Serial.print("  ");
      Serial.print(order.dirL);
      Serial.print("  ");
      Serial.println(order.speedL);
  
      Serial.print("transmiting... ");
      RF24NetworkHeader header( rob);
      bool ok = network.write(header,&order,sizeof(order));
      if (ok)
  	Serial.println("ok.");
      else
  	Serial.println("failed.");
    }	
  }
}
