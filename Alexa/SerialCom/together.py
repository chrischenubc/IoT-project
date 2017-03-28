
from flask import Flask
from flask_ask import Ask, statement
import requests
import json
import serial

ser = serial.Serial("/dev/ttyUSB0", 9600)

app = Flask(__name__)
ask = Ask(app, '/')

@ask.launch

@ask.intent("LightOn")
def on():
    for x in xrange(1, 10):
        ser.write(b'N')

    return statement("Hall light turned on.")

@ask.intent("LightOff")
def off():
    for y in xrange(1, 10):
        ser.write(b'F')
        
    return statement("Hall light turned off.")

@ask.intent("Temperature")
def extra():
    temp = ser.readline()
    #return statement("Current Temperature is %." % (temp))
    return statement(temp)


if __name__ == "__main__":
    app.run(debug=True)

