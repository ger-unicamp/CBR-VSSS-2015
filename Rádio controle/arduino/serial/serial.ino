const byte MAXIMUM_INPUT_LENGTH = 10;
char input[MAXIMUM_INPUT_LENGTH+1] = {'\0'};
byte currentIndex = 0;

void setup() { 
  Serial.begin(9600); 
} 

void loop() {
  
  for (byte i=0; i<=MAXIMUM_INPUT_LENGTH; i++){
        input[i] = '\0'; //clear input
      }
   /*
  while (Serial.available()){
    currentIndex = 0; //start over
    
    input[currentIndex++] = Serial.read(); //append to input
    */
    if (Serial.available()){
    Serial.readBytesUntil('n', input, MAXIMUM_INPUT_LENGTH);
    Serial.print("input is: ");
    Serial.println(input);
    
  }
}
