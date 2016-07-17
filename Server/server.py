#! python3
# simple server to listen for data from arduino
import socket,logging,sys
logging.basicConfig(stream=sys.stderr, level=logging.INFO)

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
    logging.info(addr)
    #logging data to make sure things are coming across properly
    data = clientSocket.recv(1024)
    
    logging.info(len(data))
    logging.info(type(data))
    logging.info(sys.getsizeof(data))

    #the writer program I'm using to add part numbers to the RFID card
    #seems to be adding some newline characters and other odd data. I'm 
    #going to put the data in a list and then parse out the part number

    #data comes across as ints so I'm converting them to characters
    dataList = list(str(data))
    logging.info(dataList)
   
   #set up a string that I can append characters to for part number
    partNumber = "" 

    #go through the list and append characters to string. The first two characters
    #of the RFID card seem to be junk data so I'm skipping over those.
    for i in range(2,len(dataList)):

        #there's just more junk data once the "'" shows up so I'm just going to
        #break the loop once we get there.
        if (ord(dataList[i]) == ord("'")):
            break
        #append chars to string
        partNumber += dataList[i]
    
    logging.info(partNumber)
    #closes connection
    clientSocket.close()

    