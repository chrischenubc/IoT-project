from flask import Flask
from flask_ask import Ask, statement
import requests
import json
import serial

ser = serial.Serial("/dev/ttyUSB0", 9600)

app = Flask(__name__)
ask = Ask(app, '/')

@ask.launch

@ask.intent("TemperatureReading")
def on():
    temp = ser.readline()
    #return statement("Current Temperature is %." % (temp))
    return statement(temp)

if __name__ == "__main__":
    app.run(debug=True)
