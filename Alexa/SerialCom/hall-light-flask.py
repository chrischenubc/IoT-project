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
    ser.write(b'N')
    return statement("Hall light turned on.")

@ask.intent("LightOff")
def off():
    ser.write(b'F')
    return statement("Hall light turned off.")

if __name__ == "__main__":
    app.run(debug=True)




