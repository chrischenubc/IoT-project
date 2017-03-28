int led = 9;

void setup(){
  pinMode(led, OUTPUT);
  pinMode(A1, INPUT);
  Serial.begin(9600);
}

void loop(){
  unsigned long oldtime;
  
  if(Serial.read() == 'N'){
    digitalWrite(led, HIGH);
  }else if(Serial.read() == 'F'){
    digitalWrite(led, LOW);
  }
  
    /*Serial.println(analogRead(A1));
    delay(500);*/
    
    Serial.println(Serial.read());
  
}
