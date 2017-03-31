#include "DHT.h"    //includes
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Servo.h>

SoftwareSerial BT(9, 10); 
LiquidCrystal lcd(4,12,13,A0,A3,A4);

const int DHT_READ_PIN = 11;       //pin assignment of DHT sensor
const int DHT_TYPE=11;            //DHT model 11
const int LM_READ_PIN = A5;       //pin assignment of LM35 sensor
const int PTC_READ_PIN = A1;      //pin assignment of photocell
const int PRECISION=4;            //precision of data display
const int LED_PIN = 3;
const int PIEZO_PIN = 8;

//For sonar
const int ECHO_PIN = 6;           //Pin for ECHO pin on the sonar
const int TRIG_PIN = 7;           //Pin for ECHO pin on the sonar
const int MOTOR_PIN = 5;          //Pin to control the motor    NOT USING FOR NOW

char data;
char dataSerial;
int led_status = 0;
int alarm_status = 0;
int alarm_alternation = 0;
int servo_status = 0;
int auto_mode_status = 0;
int triggered = 0;
int led_trigger_status = 0;
int led_alternation = 0;
float TmpLM;
float lightLevel;
float humidity;
float distance;

Servo myServo;

DHT dht(DHT_READ_PIN, DHT_TYPE);  //creator of dht library

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  //for DHT and LM35
  pinMode(DHT_READ_PIN, INPUT);
  
  //for sonar
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(PTC_READ_PIN, INPUT);
  myServo.attach(MOTOR_PIN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  lcd.begin(16, 2);

  doorClose();
}

/**
 * Read the switch status, and decide which style of display it shall follow
 */
void loop() {
  
  checkBluetoothForCommand();
  
  TmpLM=readTmpLM();
  lightLevel=readLight();
  humidity=readHumDHT();
  distance=readSonar();

  checkAutoMode();

  checkAlarmStatus();
  checkLEDStatus();
  
  String inputData = String(TmpLM) + " " + String(humidity) + " " + String(distance) + " " + String(lightLevel);
  Serial.println(inputData);
  
  updateViaBluetooth();
}

/*
 * Must come after reading input values
 * Must come before checkAlarmStatus() and checkLEDStatus()
 */
void checkAutoMode() {
  if(auto_mode_status == 1) {
    if(distance < 10) {
      triggered = 1;
      doorClose();
    }
    if(triggered == 1) {
      alarm_status = 1;
      led_trigger_status = 1;
    } else {            //if not triggered, don't do the trigger activity
      alarm_status = 0;
      led_trigger_status = 0;
    }
  }
}

void checkAlarmStatus() {
  if(alarm_status == 1) {
    if(alarm_alternation == 0){
      tone(PIEZO_PIN, 1865);
      alarm_alternation = 1;
    }
    else {
      tone(PIEZO_PIN, 1976);
      alarm_alternation = 0;
    }
  }
  else
    noTone(PIEZO_PIN);
}

void checkLEDStatus() {
  if(led_trigger_status == 1) {
    if(led_alternation == 0) {
      digitalWrite(LED_PIN, HIGH);
      led_alternation = 1;
    } else {
      digitalWrite(LED_PIN, LOW);
      led_alternation = 0;
    }
  } else {  
      led_alternation = 0;
  }
}

void updateViaBluetooth() {
  BT.print(addOneToLeft(TmpLM, "1"));
  delay(200);
  BT.print(addOneToLeft(lightLevel, "2"));
  delay(200);
  BT.print(addOneToLeft(distance, "3"));
  delay(200);
  BT.print(addOneToLeft(humidity, "4"));
  delay(200);
}

void checkBluetoothForCommand() {
  if(BT.available()) {
    data = BT.read();
    if(data == 'L') {
      LedOnOff();
    } else if(data == 'P') {
      AlarmOnOff();
    } else if(data == 'S') {
      ServoOnOff();
    } else if (data == 'A') { //turn on auto mode
      auto_mode_status = 1;
    } else if (data == 'R') { //reset the auto mode
      triggered = 0;
      digitalWrite(LED_PIN, LOW);
    } else if (data == 'O') { //turn off auto mode
      triggered = 0;
      alarm_status = 0;
      led_trigger_status = 0;
      auto_mode_status = 0;
    } else if (data == 'T') { //time
      String time = BT.readString();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(time);
    } else if (data == 'C') {
      String text = BT.readString();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(text);
    }
  }

  if(Serial.available()) {
    dataSerial = Serial.read();
    if(dataSerial == 'N') {
      LedOn();
    } else if(dataSerial == 'F') {
      LedOff();
    } else if(dataSerial == 'O') {
      doorOpen();
    } else if(dataSerial == 'C') {
      doorClose();
    } else if(dataSerial == 'A') {
      alarmOn();
    } else if(dataSerial == 'B') {
      alarmOff();
    } 
  }
}

void alarmOn() {
   alarm_status = 1;  
}

void alarmOff() {
   alarm_status = 0;
}

void doorOpen() {
  myServo.write(0);
  servo_status = 1;
}

void doorClose() {
  myServo.write(90);
  servo_status = 0;
}

void ServoOnOff() {
  if(servo_status == 0){
    myServo.write(0);
    servo_status = 1;
  }
  else {
    myServo.write(90);
    servo_status = 0;
  }
}

void AlarmOnOff() {
  if(alarm_status == 0)
    alarm_status = 1;    
  else
    alarm_status = 0;
}

void LedOnOff() {
  if(led_status == 0) {
   digitalWrite(LED_PIN, HIGH);
   led_status = 1; 
  }
  else {
   digitalWrite(LED_PIN, LOW);
   led_status = 0; 
  }
}

void LedOn() {
  digitalWrite(LED_PIN, HIGH);
  led_status = 1; 
}

void LedOff() {
  digitalWrite(LED_PIN, LOW);
  led_status = 0; 
}

float addOneToLeft(float value, String beginNum) {
  String string1 = String(value);
  String string2 = "9" + beginNum + string1;
  return string2.toFloat();
}

/**
 * Function: Read from analog pin the LM35 temperature sensor is connected to, and map the voltage to temperature in degree celsuis
 * Param: None
 * Return: Float value indicates the temperature reading from LM35, nan if the sensor is not ready
*/
float readTmpLM(){
  float readVoltage = analogRead(LM_READ_PIN);
  delay(10);
  readVoltage = analogRead(LM_READ_PIN);        //Read twice and discard the first value so that the reading is not interfered by noise
  float Tmp = mapping(readVoltage, 0, 225, 0, 100);
  return Tmp;
  }

/**
 * Function: Read from digital pin the DHT11 sensor is connected to, and map the voltage to temperature in degree celsuis
 * Param: None
 * Return: Float value indicates the temperature reading from DHT11, 'nan' if the sensor is not ready
*/
float readTmpDHT(){
  return dht.readTemperature();
  }

/**
 * Function: Read from digital pin the DHT11 sensor is connected to, and map the voltage to humidity in relative(%)
 * Param: None
 * Return: Float value indicates the humidity reading from DHT11, 'nan' if the sensor is not ready
*/
float readHumDHT(){
  return dht.readHumidity();
  }

/**
 * Function: Read from analog pin the photocell sensor is connected to, and map the voltage to ambient light level 
 * Param: None
 * Return: the light level reading from the photocell
*/
float readLight(){
  float readVoltage=analogRead(PTC_READ_PIN);
  delay(10);
  readVoltage=1023-analogRead(PTC_READ_PIN);
  float reversedValue=1023-readVoltage;         //The voltage and light level have a reversed propotional relationship
  float lightLevel=mapping(reversedValue, 0, 1023, 0, 100);
  return lightLevel;
  }

/**
 * Funtion: Basically same as the map() function in the standard ardurino math library, but able to convert it into float type.
 * The mapping is linear.
 * Param: float value: the value to be mapped
 *        inputLowerBond: The possible minium of the input value.
 *        inputUpperBond: The possible maxium of the output value.
 *        outputLowerBond: The wished value of which the minium input is mapped to
 *        outputUpperBond: The wished value of which the maxium input is mapped to
 * Return: float type being the value after mapping.
 * Preconditions: inputLowerBond<inputUpperBond && outputLowerBond<outputUpperBond
*/

float mapping(float value, float inputLowerBond, float inputUpperBond, float outputLowerBond, float outputUpperBond){
      float scale =(outputUpperBond - outputLowerBond)/(inputUpperBond - inputLowerBond);
      float offset = outputLowerBond - scale * inputLowerBond;
      return scale*value+offset;      //The process above can be mathmatically proved to be effective
  }

  /**
 * Function: Control the sonar to send a pulse, and measure the duration from the echo, calculate the distance as per temperature
 * returns: a float that indicates the distance it gets, in cm, from 0 to 200, -1 if the range is not reasonable.
*/

float readSonar(){
    float duration, distance;
    digitalWrite(TRIG_PIN, HIGH);           //set trigger pin to HIGH
    delayMicroseconds(1000);
    digitalWrite(TRIG_PIN, LOW);            //set trigger pin to LOW
    duration = pulseIn(ECHO_PIN, HIGH);     //read echo pin
    float temp=readTmpLM();                 //read temperature
    float sound_speed=331.5 + (0.6 * temp);                 //calculate the sound speed at the point
    distance = (duration * sound_speed * 0.0001)/2;        //compute distance from duration of echo Pin
    delay(200);
    if (distance >= 400 || distance <= 0){   //deciding whether or not distance is reasonable
        return(-1);                         //if not, return -1
    }
    else{
        return(distance);
    }
}

