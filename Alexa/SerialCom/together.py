
from flask import Flask
from flask_ask import Ask, statement
import requests
import json
import serial
import os

ser = serial.Serial("/dev/ttyACM0", 9600)

app = Flask(__name__)
ask = Ask(app, '/')

@ask.launch

@ask.intent("LightOn")
def on():
    for a in xrange(1, 10):
        ser.write(b'N')

    return statement("Hall light turned on.")

@ask.intent("LightOff")
def off():
    for b in xrange(1, 10):
        ser.write(b'F')
        
    return statement("Hall light turned off.")
@ask.intent("DoorOpen")
def dooron():
    for c in xrange(1, 10):
	ser.write(b'O')

    return statement("Door opened")

@ask.intent("DoorClose")
def dooroff():
    for d in xrange(1, 10):
        ser.write(b'C')

    return statement("Door closed")

@ask.intent("Temperature")
def temperature():
    data = ser.readline().split()
    temp = data[0]
    #return statement("Current Temperature is %." % (temp))
    return statement("Current temperature is" + temp + "Degree Celsius")

@ask.intent("Humidity")
def humidity():
    data = ser.readline().split()
    hum = data[1]
    #return statement("Current Temperature is %." % (temp))
    return statement("Current humidity is" + hum + "percent")

@ask.intent("Distance")
def distance():
    data = ser.readline().split()
    dis = data[2]
    #return statement("Current Temperature is %." % (temp))
    return statement("There is no object within" + dis + "centimeters")

@ask.intent("Light")
def light():
    data = ser.readline().split()
    light = data[3]
    #return statement("Current Temperature is %." % (temp))
    return statement("The light level is" + light)

@ask.intent("BuzzerOn")
def buzzerOn():
    for e in xrange(1, 10):
      ser.write(b'A')

    return statement("Alarm turned On")

@ask.intent("BuzzerOff")
def buzzerOff():
    for f in xrange(1, 10):
      ser.write(b'B')
    
    return statement("Alarm turned Off")

@ask.intent("Joke")
def joke():

    return statement("Where are you going he asked. I'm leaving you because I found out you're a pedophile." + "Whoa whoa, he says. Pedophile." + "That's a pretty big word for an eight-year-old")

@ask.intent("OpenWebsite")
def web():

    os.command("xdg-open http://38.88.74.87")

    return statement("Victoria Security main page is opening")

@ask.intent("OpenGmailail")
def gmail():

    os.command("xdg-open https://mail.google.com/mail/u/0/")

    return statement("Your gmail is opening")   


if __name__ == "__main__":
    app.run(debug=True)

