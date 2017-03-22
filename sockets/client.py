# TCP-Client
import RPi.GPIO as GPIO
import time
import socket

address = ('206.87.194.195', 8889)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(address)
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(27, GPIO.OUT)
GPIO.setup(17, GPIO.OUT)
GPIO.output(17, False)
GPIO.output(27, False)

while True:
    command = s.recv(512)
    if command == '1':
        GPIO.output(27, True)
        GPIO.output(17, False)
        s.send('Command 1 received by client')
        print "Command is ", command
    elif command == '2':
        GPIO.output(27, False)
        GPIO.output(17, True)
        s.send('Command 2 received by client')
        print "Command is ", command
    elif command == '3':
        s.send('Communication is disconnected')
        print "Command is ", command
        break
    else:
        s.send('Please enter 1, 2 or 3')
        print "Command is ", command
s.close()