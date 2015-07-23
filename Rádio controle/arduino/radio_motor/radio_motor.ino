#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define encoderL A0
#define encoderR A1

#define rightMotor 3
#define rightFor 5
#define rightBack 6

#define leftMotor 4
#define leftFor 10
#define leftBack 9

#define FORWARD 1
#define BACKWARD 2

#define ON 1
#define OFF 0

bool tickR = false;
double dtStoreR[2];
float rpmR;

bool tickL = false;
double dtStoreL[2];
float rpmL;

RF24 radio(7,8);

const uint64_t pipe = 0xE8E8F0F0E2LL;

void setup() {
  SPI.begin;
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  pinMode(rightMotor, OUTPUT);
  pinMode(leftMotor, OUTPUT);
  Serial.begin(9600);
  
  enMotor(leftMotor, ON);
  enMotor(rightMotor, ON);
  setMotor(rightMotor, BACKWARD, 0);
  setMotor(leftMotor, BACKWARD, 0);
}

void loop() {
  
  int order;
  
  controlMotor(rightMotor, FORWARD, 50);
  controlMotor(leftMotor, FORWARD, 50);
  /*
  if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read( &order, sizeof(order) );
      
      if(order==0)
      {
        setMotor(rightMotor, BACKWARD, 0);
        setMotor(leftMotor, BACKWARD, 0);
      }
      if(order==1)
      {
        setMotor(leftMotor, FORWARD, 50); // motor esquerdo em sentido direto com 50% de potencia
      }
      else if(order==2)
      {
        setMotor(rightMotor, BACKWARD, 50); // motor direito em sentido contrario com 50% de potencia
      }
      else if(order==3)
      {
        controlMotor(rightMotor, FORWARD, 30);
        controlMotor(leftMotor, FORWARD, 30);
      }
    }
  }
  */
  //int sensor = analogRead(encoderR); // leitura do encoder da direita
  //Serial.println(sensor);  // imprime valor na porta serial
    
}

void controlMotor(int motor, int sense, int ref) {
  float vel;
  
  if(motor==rightMotor)
     vel = rpmRight();
  else if(motor==leftMotor)
     vel = rpmLeft();
  
  Serial.println(vel);
  
  float err = ref - vel;
  int duty = .5*err;
  setMotor(motor, sense, 50+duty); 
}

float rpmRight() {
  
  double dt;
  int sensor;
  
  sensor = analogRead(encoderR); // leitura do encoder da direita

  if(sensor < 100)
    tickR = true;
    
  if (tickR && sensor > 100) {
     dtStoreR[0]=dtStoreR[1]; // move the current time to the previous time spot
     dtStoreR[1] = millis(); // record the current time of the rising edge
     dt = dtStoreR[1]-dtStoreR[0]; // calculate dt between the last and current rising edge time
     rpmR = (60000/(12*dt)); // RPM calculation
     tickR = false; // reset flag
   }
   else
    rpmR=0;
   return rpmR;
}

float rpmLeft() {
  
  double dt;
  int sensor;
  
  sensor = analogRead(encoderL); // leitura do encoder da direita

  if(sensor < 100)
    tickL = true;
    
  if (tickL && sensor > 100) {
     dtStoreL[0]=dtStoreL[1]; // move the current time to the previous time spot
     dtStoreL[1] = millis(); // record the current time of the rising edge
     dt = dtStoreL[1]-dtStoreL[0]; // calculate dt between the last and current rising edge time
     rpmL = (60000/(12*dt)); // RPM calculation
     tickL = false; // reset flag
   }
   else
     rpmL=0;
   return rpmL;
}

void enMotor(int motor, int enable) {
  digitalWrite(motor, enable);
}

void setMotor(int motor, int sense, int duty) {

  if(duty>100)
    duty = 100;
  else if(duty<0)
    duty = 0;

  int value = (255*duty)/100;
  
  if(motor==rightMotor) {
     if(sense==FORWARD) {
       analogWrite(rightFor, value);
       analogWrite(rightBack, 0);
     }
     else if(sense==BACKWARD) {
       analogWrite(rightBack, value);
       analogWrite(rightFor, 0);
     }
  }
  else if(motor==leftMotor) {
     if(sense==FORWARD) {
       analogWrite(leftFor, value);
       analogWrite(leftBack, 0);
     }
     else if(sense==BACKWARD) {
       analogWrite(leftBack, value);
       analogWrite(leftFor, 0);
     }
  }
}
