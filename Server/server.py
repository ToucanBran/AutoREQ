#! python3
# simple server to listen for data from arduino
import socket,logging,sys, re
logging.basicConfig(stream=sys.stderr, level=logging.INFO)

from database import Database


def add_order(location, bin_location):
    logging.info("adding order...")

    # some data validation. Location code should only be 4 characters, bin_location
    # should only be 6
    if len(location) == 4 and len(bin_location) == 6:
        try:
             # get par level and unit of measure. 
            level, uom, item_num = get_pars(location, bin_location)
            logging.debug(level)
            logging.debug(uom)

             # insert the refill order into the database
            logging.info("executing insert statement...{}-{}-{}-{}".format(
                level,uom,item_num,bin_location))
            sql_statement = "INSERT INTO orders (item, location, binLocation, amount, uom) \
                VALUES(%s,%s, %s, %s,%s)"

            db.insert(sql_statement,(item_num, location, bin_location, level, uom))
        except Exception as e:
            print("An error occured: {}".format(e));

       
    else:
        logging.debug("data validation failed at add_order")
        return -1



# Function get_pars
# This function takes the item location and bin location then executes a sql 
# query to return the item's refill amount (par level) and the unit of measure
# as a tuple. I put a length restriction to help prevent sql injections with
# malicious rfid cards. Will need to read more about how to protect DB.
# Parameters: 4 char location, 6 character bin location
# Return: tuple object with par level and unit of measure
def get_pars(location, bin_location):
    logging.info(len(location))
    logging.info(len(bin_location))
    logging.info("Getting pars...")

    # sql statement. I have to use direct substitution for the table name 
    # because MySQLdb doesn't support table name substitutions. This is
    # ok because the table name doesn't come from user input, it's hardcoded
    # in the AutoREQ sourcecode. The bin_location is properly escaped though.
    sql_statement = "SELECT par, uom, item FROM {} WHERE binLocation = %s".format(location)
    logging.info(sql_statement)

    # executes the sql statement and substitutes in the bin location. 
    # note the tuple. Reasoning can be found in the MySQLdb docs.
    refill_values = db.query(sql_statement,(bin_location,))

    #returns the par level and the unit of measure as a tuple
    logging.info("printing refill values: {}".format(refill_values))
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
    print(dataList)
    location, num = dataList[1:3]
    logging.info("checking loc: {}; Checking bin-loc: {}".format(location,num))

    add_order(location, num)

    #closes connections
    clientSocket.close()
    db.close()

    