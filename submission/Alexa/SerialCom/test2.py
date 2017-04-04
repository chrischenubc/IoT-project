
from flask import Flask
from flask_ask import Ask, statement
import requests
import json
import serial
import os

app = Flask(__name__)
ask = Ask(app, '/')

@ask.launch

@ask.intent("OpenWebsite")
def web():

    os.system("xdg-open http://38.88.74.87")

    return statement("Victoria Security main page is opening")

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

if __name__ == "__main__":
    app.run(debug=True)


