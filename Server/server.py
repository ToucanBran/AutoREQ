#! python3
# simple server to listen for data from arduino
import socket,logging,sys, re
from database import Database
logging.basicConfig(stream=sys.stderr, level=logging.INFO)

def add_order(location, item_num):
    logging.info("adding order...")

    # some data validation. Location code should only be 4 characters, item_num
    # should only be 6
    if len(location) == 4 and len(item_num) == 6:

         # get par level and unit of measure. 
        level, uom = get_pars(location, item_num)
        logging.debug(level)
        logging.debug(uom)

         # insert the refill order into the database
        logging.info("executing insert statement...")
        sql_statement = "INSERT INTO orders (item, location, amount, uom) \
            VALUES(%s,%s, %s, %s)"

        db.insert(sql_statement,(item_num, location, level, uom))
       
    else:
        logging.debug("data validation failed at add_order")
        return -1



# Function get_pars
# This function takes the item location and item number then executes a sql 
# query to return the item's refill amount (par level) and the unit of measure
# as a tuple. I put a length restriction to help prevent sql injections with
# malicious rfid cards. Will need to read more about how to protect DB.
# Parameters: 4 char location, 6 integer item number
# Return: tuple object with par level and unit of measure
def get_pars(location, item_num):
    logging.info(len(location))
    logging.info(len(item_num))
    logging.info("Getting pars...")

    # sql statement. I have to use direct substituion for the table name 
    # because MySQLdb doesn't support table name substitutions. This is
    # ok because the table name doesn't come from user input, it's hardcoded
    # in the AutoREQ sourcecode. The item_num is properly escaped though.
    sql_statement = "SELECT par, uom FROM {} WHERE item = %s".format(location)
    logging.info(sql_statement)

    # executes the sql statement and substitutes in the item number. 
    # note the tuple. Reasoning can be found in the MySQLdb docs.
    refill_values = db.query(sql_statement,(item_num,))

    #returns the par level and the unit of measure as a tuple
    return refill_values



##################################################################
##################################################################
############################## MAIN ##############################
##################################################################
##################################################################

# creating a socket object
s = socket.socket()

# set host and port variables
host = "192.168.0.4" 
port = 5000

# bind socket to host/port
s.bind((host, port))
s.listen()

while True:
    #creating database object, opening the locations db.
    db = Database("locations")

    # establish connection
    clientSocket, addr = s.accept()
    logging.info(addr)
    #logging data to make sure things are coming across properly
    data = clientSocket.recv(1024)
    
    logging.info(len(data))
    logging.info(type(data))
    logging.info(sys.getsizeof(data))

    # Arduino sourcecode's client.print function adds some characters when 
    # sending seems to be adding some newline characters and other odd data. 
    # The location and part number are sandwiched between two apostrophes so I'll
    # split the data at those. I also am going to split at hyphens since 
    # that's what separates the location and the part number. 
    dataList = re.split("'|-",str(data))
    logging.info(dataList)
  
    # The list generated from the split results in the location being at index 1
    # and the part number at index 2
    location, num = dataList[1:3]
    logging.info("checking loc: {}; Checking num: {}".format(location,num))

    add_order(location, num)

    #closes connections
    clientSocket.close()
    db.close()

    