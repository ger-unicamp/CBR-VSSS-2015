#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define MEDIA 5
#define ROB 1

#define encoderL A0
#define encoderR A1

#define rightMotor 3
#define rightFor 5
#define rightBack 6

#define leftMotor 4
#define leftFor 10
#define leftBack 9

#define FORWARD 1
#define STOP 0
#define BACKWARD -1

#define ON 1
#define OFF 0

int sensor;

int dirR = 0;
int speedR = 0;
int dirL = 0;
int speedL = 0;

float Kp = 0.1;
float Ki = 0.015;
float uR_1 = 0;
float errR_1 = 0;
float uL_1 = 0;
float errL_1 = 0;

double timeR;
bool tickR = false;
float rpmR;
float rpmR_aux;
int contR = 0;

double timeL;
bool tickL = false;
float rpmL;
float rpmL_aux;
int contL = 0;

RF24 radio(7,8);
RF24Network network(radio);

struct message
{
  int16_t dirR;
  int16_t speedR;
  int16_t dirL;
  int16_t speedL;
};

void setup(){

  delay(5000); 
  /*NUNCA, JAMAIS, DEIXE DE COLOCAR UM DELAY ANTES DE SETAR A PORTA SERIAL, PRINCIPALMENTE SE ESTIVER MEXENDO COM UM ARDUINO.
  Para a lição: arduino é programado por serial. Se um programa utilizar a saída serial, ele deixa de poder ser programado 
  durante a execução do programa. E como os programas normalmente são um loop... o Arduíno é travado.
  Isso normalmente pode ser solucionado através de um botão de "hard reset", comum em placas de prototipagem quando algum pro-
  grama trava a placa dessa forma. Infelizmente não é o caso do Arduino Nano, utilizado nesse projeto. Se por algum motivo 
  o Arduino Nano for travado nesse estado, deve ser colocado um novo bootloader na placa. Um tutorial para tal existe nos se-
  guintes sites por exemplo:
  http://www.instructables.com/id/How-To-Burn-a-Bootloader-to-Clone-Arduino-Nano-30/3/?lang=pt
  http://sysexit.wordpress.com/2013/02/07/burning-a-bootloader-to-an-arduino-nano-using-another-arduino/
  Ass.: Rooney
  */

  Serial.begin(9600);
  
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 66, /*node address*/ ROB);
  
  pinMode(rightMotor, OUTPUT);
  pinMode(leftMotor, OUTPUT);
  enMotor(leftMotor, ON);
  enMotor(rightMotor, ON);
  setMotor(rightMotor, STOP, 0);
  setMotor(leftMotor, STOP, 0);
  
  cli();//stop interrupts
  //set timer1 2nterrupt at 1kHz
  TCCR2A = 0;// set entire TCCR1A2register to 0
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

ISR(TIMER2_COMPA_vect){ //timer1 nterrupt 1kHz
  
  sensor = analogRead(encoderR); // leitura do encoder da direita
  if(sensor < 100)
    tickR = true;
    
  if (tickR && sensor > 100) {
     rpmR_aux += (60000/(12*timeR)); // RPM calculation
     tickR = false; // reset flag
     timeR = 0;
     contR++;
   }
   else
    timeR++;
  
  if(timeR > 500)
    rpmR = 0;
    
  sensor = analogRead(encoderL); // leitura do encoder da direita
  if(sensor < 100)
    tickL = true;
    
  if (tickL && sensor > 100) {
     rpmL_aux += (60000/(12*timeL)); // RPM calculation
     tickL = false; // reset flag
     timeL = 0;
     contL++;
   }
   else
    timeL++;
  
  if(timeL > 500)
    rpmL = 0;
    
  if(contR>=MEDIA)
  {
    rpmR = rpmR_aux/MEDIA;
    rpmR_aux = 0;
    contR = 0;
  }
  
  if(contL>=MEDIA)
  {
    rpmL = rpmL_aux/MEDIA;
    rpmL_aux = 0;
    contL = 0;
  }

}

void loop(){

  Serial.print("speed L ");
  Serial.println(rpmL);
  Serial.print("speed R ");
  Serial.println(rpmR);
//  delay(300);

  // Pump the network regularly
  network.update();
  
  // Is there anything ready for us?
  if ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    message order;
    network.read(header,&order,sizeof(order));
  
    Serial.println("Received order ");
    Serial.println(order.dirR);
    Serial.println(order.speedR);
    Serial.println(order.dirL);
    Serial.println(order.speedL);
  
    uR_1 = 0;
    errR_1 = 0;
    uL_1 = 0;
    errL_1 = 0;
    contR = 0;
    contL = 0;
    
    dirR = order.dirR;
    speedR = order.speedR;
    dirL = order.dirL;
    speedL = order.speedL;
  }
  
  controlMotorR(dirR, speedR);
  controlMotorL(dirL, speedL);
  
}

void enMotor(int motor, int enable) {
  digitalWrite(motor, enable);
}

void setMotor(int motor, int sense, int value) {

  if(value>255)
    value = 255;
  else if(value<0)
    value = 0;
  
  if(motor==rightMotor) {
     if(sense==FORWARD) {
       analogWrite(rightFor, value);
       analogWrite(rightBack, 0);
     }
     else if(sense==BACKWARD) {
       analogWrite(rightBack, value);
       analogWrite(rightFor, 0);
     }
     else if(sense==STOP) {
       analogWrite(rightBack, 0);
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
     else if(sense==STOP) {
       analogWrite(leftBack, 0);
       analogWrite(leftFor, 0);
     }
  }
}

void controlMotorR(int sense, int ref) {
  
  float errR = ref-rpmR;
  
  float uR = uR_1 + Kp*errR - Kp*errR_1 + Ki*errR_1;
  uR_1 = uR;
  errR_1 = errR;
  
  Serial.println(uR);
  
  int uRi = uR;
  
  setMotor(rightMotor, sense, uRi);
}

void controlMotorL(int sense, int ref) {
  
  float errL = ref-rpmL;
  
  float uL = uL_1 + Kp*errL - Kp*errL_1 + Ki*errL_1;
  uL_1 = uL;
  errL_1 = errL;
  
  Serial.println(uL);
  
  int uLi = uL + 5;
  
  setMotor(leftMotor, sense, uLi);
}
