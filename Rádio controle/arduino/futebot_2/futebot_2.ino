#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define MEDIA 5
#define ROB 2

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

float Kp = 1.0;
float Ki = 0.1;
float intErrR = 0;
float uR_1 = 0;
float uL_1 = 0;
float intErrL = 0;

double timeR;
bool tickR = false;
float rpmR;
float rpmR_aux[5]={};
int contR = 0;
int v_atualR=0;
int flag_R = 0;

double timeL;
bool tickL = false;
float rpmL;
float rpmL_aux[5]={};
int contL = 0;
int v_atualL=0;
int flag_L = 0;
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
     rpmR_aux[v_atualR] = (60000/(12*timeR)); // RPM calculation
     flag_R = 1;
     tickR = false; // reset flag
     timeR = 0;
     contR++;
   }
   else
    
    timeR++;
  
  if(timeR > 500){
    flag_R = 1;
    rpmR_aux[v_atualR] = 0;
    }
    
  sensor = analogRead(encoderL); // leitura do encoder da direita
  if(sensor < 100)
      tickL = true;
    
  if (tickL && sensor > 100) {
     rpmL_aux[v_atualL] = (60000/(12*timeL)); // RPM calculation
     flag_L = 1;
     tickL = false; // reset flag
     timeL = 0;
     contL++;
   }
   else
    timeL++;
  
  if(timeL > 500){
    rpmL_aux[v_atualL] = 0;
    flag_L = 1;  
  }
  if (flag_R == 1){
    rpmR = media_vetor(rpmR_aux,5);
    contR = 0;
    v_atualR++;    
    if (v_atualR ==5)
        v_atualR = 0;
    flag_R=0;
    }
  if(flag_L ==1){
    rpmL = media_vetor(rpmL_aux,5);
    contL = 0;
    v_atualL++;
    if (v_atualL ==5)
        v_atualL = 0;    
  }
}

void loop(){

  Serial.print("speed L ");
  Serial.println(rpmL);
  Serial.print("speed R ");
  Serial.println(rpmR);

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
  
  float errR = normalizaRPM(ref-rpmR);
  intErrR += errR;  
  if (intErrR > ref/Ki)
    intErrR = ref/Ki;
    
  float uR = Kp*errR + Ki*intErrR;
  
  int uRi = uR;
   Serial.println("Controle R");
   Serial.println(sense);
   Serial.println(errR);
   Serial.println(intErrR);
   Serial.println(uRi);
    
  setMotor(rightMotor, sense, uRi);

}

void controlMotorL(int sense, int ref) {
  
  float errL = normalizaRPM((ref)-(rpmL));
  intErrL += errL;  
  if (intErrL > ref/Ki)
    intErrL = ref/Ki;
  float uL =  Kp*errL + Ki*intErrL;
  int uLi = uL;
   Serial.println("Controle L");
   Serial.println(sense);
   Serial.println(errL);
   Serial.println(intErrL);
   Serial.println(uLi);
  
  setMotor(leftMotor, sense, uLi);
}

float normalizaRPM(float RPM){
  //normaliza o valor máximo de RPM de um robô para dentro de [0 255]
  //para o robo 1, o mínimo valor máximo é de 738
  return RPM*255/738;
}

float media_vetor (float v[], int n){
  int i;
  float res=0;
  for (i=0; i<n;i++)
    res+=v[i];
    return res/n;
}
