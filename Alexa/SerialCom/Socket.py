from flask import Flask
from flask_ask import Ask, statement
from socketIO_client import SocketIO, LoggingNamespace
import requests
import json
import serial

#Global variables
temp = 0;
humidity = 0;
light = 0;
distance = 0;

#SocketIO
def on_connect():
    print('connect')

def on_disconnect():
    print('disconnect')

def on_reconnect():
    print('reconnect')

def tempfromserver(args):
        print(args)
        global temp
        temp = args

socketIO = SocketIO('http://38.88.74.87', 9000, LoggingNamespace)
socketIO.on('connect', on_connect)
socketIO.on('disconnect', on_disconnect)
socketIO.on('reconnect', on_reconnect)
# Listen
socketIO.on('tempFromServer', tempfromserver)

ser = serial.Serial("/dev/ttyUSB0", 9600)

#Alexa
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
    socketIO.emit('tempFromPi')
    socketIO.wait(seconds=0.1)
    
    return statement(temp)


if __name__ == "__main__":
    app.run(debug=True)
