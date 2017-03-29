#import logging
#logging.getLogger('socketIO-client').setLevel(logging.DEBUG)
#logging.basicConfig()

from socketIO_client import SocketIO, LoggingNamespace
import time

temp = 0

def on_connect():
    print('connect')

def on_disconnect():
    print('disconnect')

def on_reconnect():
    print('reconnect')

def on_python(args):
    print(args)

print("wtf")
socketIO = SocketIO('http://38.88.74.87', 9000, LoggingNamespace)
socketIO.on('connect', on_connect)
print("wtf")
socketIO.on('disconnect', on_disconnect)
socketIO.on('reconnect', on_reconnect)

# Listen
socketIO.on('tempFromServer', on_python)

while True:
    socketIO.emit('tempFromPi')
    socketIO.wait(seconds=1)
