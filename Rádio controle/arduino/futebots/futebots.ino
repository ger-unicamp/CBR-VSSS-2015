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

void setup() {
  pinMode(rightMotor, OUTPUT);
  pinMode(leftMotor, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  enMotor(leftMotor, OFF); // ativa motor esquerdo
  enMotor(rightMotor, OFF); // ativa motor esquerdo
  //setMotor(leftMotor, FORWARD, 50); // motor esquerdo em sentido direto com 50% de potencia
  //setMotor(rightMotor, FORWARD, 100); // motor direito em sentido contrario com 50% de potencia
  controlMotor(leftMotor, FORWARD, 150);
}

void controlMotor(int motor, int sense, int ref) {
  float vel;
  
  if(motor==rightMotor)
     vel = rpmRight();
  else if(motor==leftMotor)
     vel = rpmLeft();
  
  Serial.println(vel);
  
  float err = ref - vel;
  int duty = 0.75*err;
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
