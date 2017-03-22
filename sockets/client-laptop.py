# TCP-Client


import socket

address = ('206.87.194.195', 8889)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(address)

while True:
    command = s.recv(512)
    if command == '1':

        s.send('Command 1 received by client')
        print "Command is ", command
    elif command == '2':

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