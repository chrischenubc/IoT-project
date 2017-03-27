#include "DHT.h"    //includes
#include <SoftwareSerial.h>

SoftwareSerial BT(9, 10); 

const int DHT_READ_PIN = 11;      //pin assignment of DHT sensor
const int DHT_TYPE=11;            //DHT model 11
const int SWITH_READ_PIN =12;     //pin assignment of switch
const int LM_READ_PIN = A5;       //pin assignment of LM35 sensor
const int PTC_READ_PIN = A1;      //pin assignment of photocell
const int ON = 1;
const int OFF = 0;
const int PRECISION=4;            //precision of data display
const int LED_PIN = 3;

//For sonar
const int ECHO_PIN = 6;           //Pin for ECHO pin on the sonar
const int TRIG_PIN = 7;           //Pin for ECHO pin on the sonar
//const int MOTOR_PIN = 9;          //Pin to control the motor    NOT USING FOR NOW

char data;
int led_status = 0;
float TmpLM;
float lightLevel;

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
  //myServo.attach(MOTOR_PIN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

/**
 * Read the switch status, and decide which style of display it shall follow
 */
void loop() {
  if(BT.available()) {
    data = BT.read();
    if(data == 'L') {
      LedOnOff();
    }
  }
  
  TmpLM=readTmpLM();
  lightLevel=readLight();
  BT.print(addOneToLeft(TmpLM, "1"));
  Serial.println(TmpLM);
  Serial.println(lightLevel);
  delay(300);
 // BT.print(addOneToLeft(lightLevel, '2'));
  
  //delay(400);
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

float addOneToLeft(float value, String beginNum) {
  String string1 = String(value);
  String string2 = "1" + beginNum + string1;
  return string2.toFloat();
}


/**
 * Function: Collect data from three sensors and display it in format when the switch is off
 * 
*/
void offMode(){
  float TmpLM=readTmpLM();
  float TmpDHT=readTmpDHT();
  float HumDHT=readHumDHT();
  float lightLevel=readLight();
  format(TmpLM, TmpDHT, HumDHT, lightLevel);
  return;
  }

  

/**
 * Function: Collect data from three sensors and display it in format when the switch is on
 * 
*/
void onMode(){
  float TmpLM=readTmpLM();
  float TmpDHT=readTmpDHT();
  float HumDHT=readHumDHT();
  float lightLevel=readLight();
  noFormat(TmpLM, TmpDHT, HumDHT, lightLevel);
  return;
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
 * Funtion: Print the data with full format
 * Param: float TmpLM: The tempterature reading from the LM35 Sensor
 *        float TmpDHT: The temperature reading from DHT sensor
 *        float HumDHT: The humidity reading from DHT sensor
 *        float lightLevel: The light reading from the photocell
 * Return: None
*/
void format(float TmpLM, float TmpDHT, float HumDHT, float lightLevel){
  Serial.print("The temperature reading from LM35 is: ");
  Serial.print(TmpLM, PRECISION);
  Serial.print(" degree celsius. \n");
  Serial.print("The temperature reading from DHT11 is: ");
  Serial.print(TmpDHT, PRECISION);
  Serial.print(" degree celsius\n");
  Serial.print("The humidity readging from DHT is: ");
  Serial.print(HumDHT, PRECISION);
  Serial.print("% \n");
  Serial.print("The light reading is: ");
  Serial.print(lightLevel, PRECISION);
  Serial.print("\n");
  Serial.print("End of this round of reading. \n\n");
}

/**
 * Funtion: Print the data without format
 * Param: float TmpLM: The tempterature reading from the LM35 Sensor
 *        float TmpDHT: The temperature reading from DHT sensor
 *        float HumDHT: The humidity reading from DHT sensor
 *        float lightLevel: The light reading from the photocell
 * Return: None
*/
void noFormat(float TmpLM, float TmpDHT, float HumDHT, float lightLevel){
  Serial.print(TmpLM, PRECISION);
  Serial.print(" ");
  Serial.print(TmpDHT, PRECISION);
  Serial.print(" ");
  Serial.print(HumDHT, PRECISION);
  Serial.print(" ");
  Serial.print(lightLevel, PRECISION);
  Serial.print("\n");
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
    if (distance >= 200 || distance <= 0){   //deciding whether or not distance is reasonable
        return(-1);                         //if not, return -1
    }
    else{
        return(distance);
    }
}

