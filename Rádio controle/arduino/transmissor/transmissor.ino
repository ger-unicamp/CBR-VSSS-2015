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

// How many have we sent already
unsigned long packets_sent;

// Structure of our payload
struct message
{
	int dirR;
  	int speedR;
  	int dirL;
	int speedL;
};

void setup(void)
{
  Serial.begin(9600);
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ master);
}

void loop(void)
{

  //Pump the network regularly
  network.update();
    
  if(Serial.available())
  {
    order = Serial.read();
    
    switch(order)
    {
    	case 'w':
		  message rob1_order = { 1, 100, 1, 100 };
		  break;
    	case 's':
		  message rob1_order = { -1, 100, -1, 100 };
		  break;
		case 'q':
		  message rob1_order = { 0, 0, 0, 0 };
		  break;
	}
	
	RF24NetworkHeader header(/*to node*/ rob1);
  	bool ok = network.write(header,&rob1_order,sizeof(rob1_order));
  	if (ok)
		Serial.println("ok.");
  	else
		Serial.println("failed.");
	}
}
