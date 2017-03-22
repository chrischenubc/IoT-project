# TCP-Server
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


address = ('206.87.112.234', 8889)
s.bind(address)


# listen(backlog), The backlog argument specifies the maximum number of queued connections and should be at least 0;
# the maximum value is system-dependent (usually 5), the minimum value is forced to 0.
s.listen(5)
print 'Waiting for connection...'

new_s, addr = s.accept()
print 'Got connect from', addr


command = ' '
print ('Please enter 1 to turn on the led, 2 to turn off the led,3 to disconnect the communication:')
while True:
    command = raw_input()
    new_s.send(command)
    data = new_s.recv(512)
    print data
    if data == 'Communication is disconnected':
        break

new_s.close()
s.close()