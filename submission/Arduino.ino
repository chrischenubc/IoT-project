#include "DHT.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Servo.h>

SoftwareSerial BT(9, 10);             //bluetooth object
LiquidCrystal lcd(4,12,13,A0,A3,A4);  //lcd object

const int DHT_READ_PIN = 11;       //pin assignment of DHT sensor
const int DHT_TYPE=11;            //DHT model 11
const int LM_READ_PIN = A5;       //pin assignment of LM35 sensor
const int PTC_READ_PIN = A1;      //pin assignment of photocell
const int PRECISION=4;            //precision of data display
const int LED_PIN = 3;
const int PIEZO_PIN = 8;
const int THRESHOLD = 10;         //threshold for motion sensor/ultrasonic sensor
                                  //change the threshold to the farthest distance from the direction the sensor points at

//For sonar
const int ECHO_PIN = 6;           //Pin for ECHO pin on the sonar
const int TRIG_PIN = 7;           //Pin for ECHO pin on the sonar
const int MOTOR_PIN = 5;          //Pin to control the motor    NOT USING FOR NOW

char data;
char dataSerial;                //
int led_status = 0;             //used to indicate if led is turned on or off
int alarm_status = 0;           //used to indicate if alarm is turned on or off
int alarm_alternation = 0;      //used to alternate between different pitches of the alarm (piezo buzzer)
int servo_status = 0;           //used to indicate if the door is open or closed
int auto_mode_status = 0;       //Auto mode = security mode. This is used to indicate if this mode is on or off
int triggered = 0;              //In Auto(Security) mode, this indicates if the intruder protocol is triggered or not
int led_trigger_status = 0;     //used to indicate if led blinking pattern is on or off
int led_alternation = 0;        //used to alternate between on and off so that it blinks
float TmpLM;                    //holds temperature read from LM35
float lightLevel;               //holds photosensor input value
float humidity;                 //holds humidty input value from DHT 11
float distance;                 //holds distance value read from the ultrasonic sensor

Servo myServo;                  //global servo object

DHT dht(DHT_READ_PIN, DHT_TYPE);  //creator of dht library

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  //sensors
  pinMode(DHT_READ_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  //door initialization
  myServo.attach(MOTOR_PIN);

  //lcd initialization
  lcd.begin(16, 2);
  
  //for sonar
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(PTC_READ_PIN, INPUT);
  
  doorClose();                  //have door shut
  digitalWrite(LED_PIN, LOW);   //have led start off for demo
}

/**
 * Read the switch status, and decide which style of display it shall follow
 */
void loop() {
  
  checkForCommand();   
  
  TmpLM=readTmpLM();
  lightLevel=readLight();
  humidity=readHumDHT();
  distance=readSonar();

  checkAutoMode();              

  checkAlarmStatus();     //checks alarm flag and turns on/off alarm accordingly
  checkLEDStatus();       //checks led trigger flag and turns on/off alarm accordingly
  
  String inputData = String(TmpLM) + " " + String(humidity) + " " + String(distance) + " " + String(lightLevel);
  Serial.println(inputData);    //Print it to Serial for the Raspberry Pi to be able to read the values
                                //if the user asks Alexa what the input sensor values are
  
  updateViaBluetooth();     //Sends the input sensor values over Bluetooth
}

/*
 * Checks for auto_mode_status flag and evaluates if the distance
 * 
 * Must come after reading input values
 * Must come before checkAlarmStatus() and checkLEDStatus()
 */
void checkAutoMode() {
  if(auto_mode_status == 1) {
    if(distance < THRESHOLD) {            //threshold is set to 10cm. Can be adjusted from constants
      triggered = 1;                      //this flag will later turn on the alarm and turn the blinking pattern in next if statement
      doorClose();                        //close door
    }
    if(triggered == 1) {
      alarm_status = 1;                 //alarm_status flag raised which will be evaluated later in the loop() to turn the alarm on
      led_trigger_status = 1;           //led_trigger_status flag raised which will be evalued later in the loop() to turn the led pattern on
    } else {                            //if not triggered, don't do the trigger activity
      alarm_status = 0;                 //don't raise the alarm flag
      led_trigger_status = 0;           //don't raise the led trigger pattern flag
    }
  }

  //if not auto)mode, don't even worry about the flags
}

/*
 * Checks if alarm_status flag is on, and if it is on, then turn the alarm on and vice versa.
 */
void checkAlarmStatus() {
  if(alarm_status == 1) {           //if flag on
    if(alarm_alternation == 0){     //these two if statements allow the alternation between the tones
      tone(PIEZO_PIN, 1865);        //hardcoded lower tone
      alarm_alternation = 1;        //switch to higher tone next time
    }
    else {
      tone(PIEZO_PIN, 1976);        //hardcoded higher tone
      alarm_alternation = 0;        //switch to lower tone next time
    }
  }
  else
    noTone(PIEZO_PIN);              //if flag is not raised, then don't turn on the alarm
}

/*
 * Checks for the LED trigger flag and turns it on and off accordingly
 */
void checkLEDStatus() {
  if(led_trigger_status == 1) {         //if trigger flag is on, then go into alternation of light on and off for blinking effect
    if(led_alternation == 0) {
      digitalWrite(LED_PIN, HIGH);
      led_alternation = 1;
    } else {
      digitalWrite(LED_PIN, LOW);
      led_alternation = 0;
    }
  } else {                              //if trigger flag is off, then there is no alternation of blinking
      led_alternation = 0;              //Either led is turned on or off from the checkForCommands(), not here
  }
}

/*
 * Passes the input sensor readings to the bluetooth socket so that the Android phone can liten to the values and update
 * the readings on the phone. Then the Android will send these data to the server which then the server will send the data to other
 * html clients as well
 * 
 * Refer to function below this one for the importance of addOneToLeft function
 */
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

/*
 * This function is VERY importent in Bluetooth communication.
 * This function addresses two problems.
 * Problems: 1) When passing values from Arduino to Bluetooth, the first character in a string is always lost on the other side.
 *            Soln: We just attach a random "9" in the beginning so no data is lost
 *           2) There is a limit on how many bytes we can send over Bluetooth. 
 *            Soln: We number code the values we want to pass and send multiple smaller bytes. 
 *                This number coding is followed after the first character that is lost and is specified by the input parameter
 */         
float addOneToLeft(float value, String beginNum) {
  String string1 = String(value);
  String string2 = "9" + beginNum + string1;
  return string2.toFloat();
}


/*
 * Checks for command given by the Bluetooth (Android) and the Raspberry Pi of voice control (Alexa)
 * The commands are letter coded
 */
void checkForCommand() {
  //checks Bluetooth for commands
  if(BT.available()) {        //some function names are self-explanatory given the letter coding
    data = BT.read();
    if(data == 'L') {
      LedOnOff();
      
    } else if(data == 'P') {
      AlarmOnOff();
      
    } else if(data == 'S') {
      ServoOnOff();
      
    } else if (data == 'A') { //turn on auto/security mode
      auto_mode_status = 1;
      
    } else if (data == 'R') { //reset the auto/security mode
      triggered = 0;          //lower trigger flag
      digitalWrite(LED_PIN, LOW);   //turn off the led
      
    } else if (data == 'O') { //turn off auto/security mode
      triggered = 0;            //lowerering trigger activity flags
      alarm_status = 0;         //""
      led_trigger_status = 0;   //""
      auto_mode_status = 0;     //lowers auto/security mode flag
      
    } else if (data == 'T') {   //Command to update the LCD clock time. This was not used during demo
      String time = BT.readString();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(time);
      
    } else if (data == 'C') {   //command to write messages (from html client) to the lcd
      String text = BT.readString();    
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(text);
    }
  }

  //checks Serial (Raspberry pi of voice control) for commands
  //these commands are activated if you talk to Alexa
  if(Serial.available()) {
    dataSerial = Serial.read();       //function names are self-explanatory given the letter coding
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

/*
 * Raises alarm flag
 */
void alarmOn() {
   alarm_status = 1;  
}

/*
 * Lowers alarm flag
 */
void alarmOff() {
   alarm_status = 0;
}

/*
 * Opens the house door and raises the servo_status flag to indicate that the door is open right now
 */
void doorOpen() {
  myServo.write(0);
  servo_status = 1;
}

/*
 * Closes the door and lowers the servo_status flag
 */
void doorClose() {
  myServo.write(90);
  servo_status = 0;
}

/*
 * Same as the two functions above, but this function makes logic easier for button (on Android and html client) implementation
 */
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

/*
 * Used for button logic (in Android and html client)
 * If alarm flag on, then turn it off. If alarm flag is off, then turn it on
 */
void AlarmOnOff() {
  if(alarm_status == 0)
    alarm_status = 1;    
  else
    alarm_status = 0;
}

/*
 * Used for button logic (in Android and html client)
 * If led flag is on, turn led off and turn flag off
 * If led flag is off, turn led on and turn the flag on
 */
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

/*
 * Turns led on and sets flag
 */
void LedOn() {
  digitalWrite(LED_PIN, HIGH);
  led_status = 1; 
}

/*
 * Turns led off and sets flag
 */
void LedOff() {
  digitalWrite(LED_PIN, LOW);
  led_status = 0; 
}

//__________________Functions below were brought from the previous labs 1-5______________________

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

