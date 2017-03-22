from nanpy import (ArduinoApi, SerialManager)
from time import sleep

led_pin = 4 #digitalwrite
DHT_pin = 5 #uses library DHT.h
LM35_pin = 6 #analogRead
photosensor_pin = 7 #analogRead
trig_pin = 8
echo_pin = 9

try:
    connection = SerialManager()
    a = ArduinoApi(connection = connection)
except:
    print "Failed to connect to Arduino"

#function declarations
a.pinMode(led_pin, a.OUTPUT)
a.pinMode(DHT_pin, a.INPUT)
a.pinMode(trig_pin, a.OUTPUT)
a.pinMode(echo_pin, a.INPUT)

def hello_world():
    print ("Hi")
    return

def read_tmp_lm(): #returns float
    read_voltage = a.analogRead(LM35_pin)
    sleep(0.0001)
    read_voltage = a.analogRead(LM35_pin)
    tmp = mapping(read_voltage, 0, 255, 0, 100)
    return tmp

def read_tmp_dht(): #returns float
    return a.dht.readTemperature()

def read_hum_dht(): #returns float
    return a.dht.readHumidity()

def read_light(): #returns float
    read_voltage = a.analogRead(photosensor_pin)
    sleep(0.0001)
    read_voltage = a.analogRead(photosensor_pin)
    reversed_value = 1023 - read_voltage
    light_level = mapping(reversed_value, 0, 1023, 0, 100)
    return light_level

def mapping(value, input_lower, input_upper, output_lower, output_upper): #returns float
    scale = (output_upper - output_lower) / (input_upper - input_lower)
    offset = output_lower - scale*input_lower
    return scale*value+offset

def led_on_off(): #returns nothing. turns led on if off and off if on
    status = a.digitalRead(led_pin)
    if status == a.HIGH:
        a.digitalWrite(led_pin, a.LOW)
    else:
        a.digitalWrite(led_pin, a.HIGH)
    return

def read_sonar():
    a.digitalWrite(trig_pin, a.HIGH)
    sleep(0.001)
    a.digitalWrite(trig_pin, a.LOW)
    duration  = a.pulseIn(echo_pin, a.HIGH)
    temp = read_tmp_lm()
    sound_speed = 331.5 + (0.6*temp)
    distance = (duration*sound_speed*0.0001)/2
    return distance

#loop()
while True:
    #more to come
    read_tmp_lm()
    hello_world()