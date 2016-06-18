#! python3
# simple server to listen for data from arduino
import socket

# creating a socket object
s = socket.socket()

# set host and port variables
host = "192.168.0.4" 
port = 5000

# bind socket to host/port
s.bind((host, port))
s.listen()

while True:
    # establish connection
    clientSocket, addr = s.accept()
    #use this line for debug purposes to check for connection
    #print("got a connection from %s" % str(addr))

    #this is receiving too much data. Need to figure out a way
    #to receive the right amount as well as the correct data
    data = clientSocket.recv(1024)
    print(data)

    #closes connection
    clientSocket.close()

    