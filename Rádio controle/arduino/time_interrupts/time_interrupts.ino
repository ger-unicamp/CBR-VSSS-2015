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

int sensor;

float Kp = 0.2;
float Ki = 0.1;
float uR_1 = 0;
float errR_1 = 0;
float uL_1 = 0;
float errL_1 = 0;

double timeR;
bool tickR = false;
float rpmR;

double timeL;
bool tickL = false;
float rpmL;

RF24 radio(7,8);
const uint64_t pipe = 0xE8E8F0F0E2LL;

void setup(){

  Serial.begin(9600);
  
  SPI.begin;
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  
  pinMode(rightMotor, OUTPUT);
  pinMode(leftMotor, OUTPUT);
  enMotor(leftMotor, ON);
  enMotor(rightMotor, ON);
  setMotor(rightMotor, BACKWARD, 0);
  setMotor(leftMotor, BACKWARD, 0);
  
  cli();//stop interrupts
  //set timer1 interrupt at 1kHz
  TCCR2A = 0;// set entire TCCR1A register to 0
  TCCR2B = 0;// same for TCCR1B
  TCNT2  = 0;//initialize counter value to 0
  // set timer count for 1khz increments
  OCR2A = 249;// = (16*10^6) / (1000*64) - 1
  //had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set 64 prescaler
  TCCR2B = TCCR2B & 0b11111000 | 0x04;  
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();//allow interrupts

}

ISR(TIMER2_COMPA_vect){//timer1 interrupt 1kHz
  
  sensor = analogRead(encoderR); // leitura do encoder da direita
  if(sensor < 100)
    tickR = true;
    
  if (tickR && sensor > 100) {
     rpmR = (60000/(12*timeR)); // RPM calculation
     tickR = false; // reset flag
     timeR = 0;
   }
   else
    timeR++;
  
  if(timeR > 500)
    rpmR = 0;
    
  sensor = analogRead(encoderL); // leitura do encoder da direita
  if(sensor < 100)
    tickL = true;
    
  if (tickL && sensor > 100) {
     rpmL = (60000/(12*timeL)); // RPM calculation
     tickL = false; // reset flag
     timeL = 0;
   }
   else
    timeL++;
  
  if(timeL > 500)
    rpmL = 0;
}

void loop(){
  
  int order;
  /*
  Serial.print("vel esquerda: ");
  Serial.println(rpmL);
  Serial.print("vel direita: ");
  Serial.println(rpmR);
  
  controlMotorR(FORWARD, 150);
  controlMotorL(FORWARD, 150);
  */
  if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read( &order, sizeof(order) );
      
      uR_1 = 0;
      errR_1 = 0;
      uL_1 = 0;
      errL_1 = 0;
    }
  }
  
  if(order==0)
  {
    setMotor(rightMotor, BACKWARD, 0);
    setMotor(leftMotor, BACKWARD, 0);
  }
  if(order==1)
  {
    controlMotorR(FORWARD, 100); // motor esquerdo em sentido direto com 50% de potencia
  }
  else if(order==2)
  {
    controlMotorL(FORWARD, 100); // motor direito em sentido contrario com 50% de potencia
  }
  else if(order==3)
  {
    controlMotorR(FORWARD, 150);
    controlMotorL(FORWARD, 150);
  }
  else if(order==4)
  {
    controlMotorR(BACKWARD, 150);
    controlMotorL(BACKWARD, 150);
  }   
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

void controlMotorR(int sense, int ref) {
  
  float errR = ref-rpmR;
  
  int uR = uR_1 + Kp*errR - Kp*errR_1 + Ki*errR_1;
  uR_1 = uR;
  errR_1 = errR;
  
  Serial.println(uR);
  
  setMotor(rightMotor, sense, uR);
}

void controlMotorL(int sense, int ref) {
  
  float errL = ref-rpmL;
  
  int uL = uL_1 + Kp*errL - Kp*errL_1 + Ki*errL_1;
  uL_1 = uL;
  errL_1 = errL;
  
  Serial.println(uL);
  
  setMotor(leftMotor, sense, uL);
}
