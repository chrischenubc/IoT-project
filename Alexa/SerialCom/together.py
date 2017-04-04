#import flask and flask_ask as Python framework 
#to simplify the skill development procedure
from flask import Flask
from flask_ask import Ask, statement
import requests
import json

#import serial library to enable serial communication 
import serial

#import os library in order to enable Voice Control system to 
#execute Linux commands and shell scripts. This significantly 
#expands the capability of voice control system. Opening web browser
#function is achieved using this method
import os

#Setting things up
ser = serial.Serial("/dev/ttyACM0", 9600)

app = Flask(__name__)
ask = Ask(app, '/')

@ask.launch

#link the function to the intent "LightOn"
@ask.intent("LightOn")
#Once the Alexa Web Service receives the words "LightOn", the 
#intent "LightOn" will be triggered and the function on() below 
#will be executed
def on():
    #use a for loop to send the byte 'N' to the 
	#Arduino for 10 times, reducing the possibility 
	#that Arduino fails to receive the signal
    for a in xrange(1, 10):
		#Send the byte 'N' to Arduino using serial communication
        ser.write(b'N')
	
	#This is the return statement which is supposed to be first sent
	#to Amazon Web Server, and later returned in audio format to be 
	#played on local machine
    return statement("Hall light turned on.")

#The code below has the same structure and foramt as the previous code,
#so most of the comments are omitted 
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

	#Execute the Linux command below to open the url using 
	#the default web browser
    os.system("xdg-open http://38.88.74.87")

    return statement("Victoria Security main page is opening")

#The code below has the same structure and foramt as the previous code,
#so the comments are omitted 
@ask.intent("OpenGmail")
def gmail():

    os.system("xdg-open https://mail.google.com/mail/u/0/")

    return statement("Your gmail is opening")   

@ask.intent("OpenChatroom")
def chatroom():
    
    os.system("xdg-open http://38.88.74.87/Subwebsite/chatRoom.html")

    return statement("Victoria Security chatroom is opening")

@ask.intent("Test")
def test():

    return statement("The computer is fine")

@ask.intent("OpenSurveillance")
def videoStream():

    os.system("xdg-open http://128.189.84.138:3000")

    return statement("Victoria Security video servallence is opening")

@ask.intent("OpenDatapage")
def datapage():

    os.system("xdg-open http://38.88.74.87/Subwebsite/Monitoring.html")

    return statement("Victoria Security monitoring page is opening")

@ask.intent("OpenFacebookPage")
def facebookPage():

    os.system("xdg-open https://www.facebook.com/Victoria-Security-1821395828176172/")

    return statement("Victoria Security facebook page is opening")

if __name__ == "__main__":
    app.run(debug=True)

