#! python3
# simple server to listen for data from arduino
import MySQLdb,socket,logging,sys
logging.basicConfig(stream=sys.stderr, level=logging.INFO)

#connect to location database. Will need to hide credentials in the future
db = MySQLdb.connect(
    user="root",
    db="locations")

#Function get_pars
#This function takes the item location and item number then executes a sql 
#query to return the item's refill amount (par level) and the unit of measure
#as a tuple. I put a length restriction to help prevent sql injections with
#malicious rfid cards. Will need to read more about how to protect DB.
#Parameters: 4 char location, 6 integer item number
#Return: tuple object with par level and unit of measure
def get_pars(location, item_num):
    logging.info(len(location))
    logging.info(len(item_num))

    if len(location) == 4 and len(item_num) == 6:
        cursor = db.cursor()

        #sql statement
        cursor.execute('SELECT par, uom FROM {} WHERE item like {}'.format(location,item_num))

        #returns the par level and the unit of measure as a tuple
        return cursor.fetchone()
    else:
        #lengths are wrong, return error
        return (-1,-1)

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

    #the writer program I'm gitusing to add part numbers to the RFID card
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

    #get par level and unit of measure. 
    #I'll need to have the arduino also transmit the location so for now
    #I'm just hardcoding it in.
    level, uom = get_pars('emrg',partNumber)
    logging.info(level)
    logging.info(uom)
    #closes connection
    clientSocket.close()

    