/**
 * CARDWRITER
 * 
 * 
 * -----------------------------------------------------------------------------------------
 *             MFRC522      
 *             Reader/PCD   Uno           
 * Signal      Pin          Pin           
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             
 * SPI SS      SDA(SS)      7          
 * SPI MOSI    MOSI         11 / ICSP-4   
 * SPI MISO    MISO         12 / ICSP-1   
 * SPI SCK     SCK          13 / ICSP-3   
 * 
 * Purpose of this program is write new bin locations to an RFID card
 * 
 * Thanks to SparkFun's wifly shield library and Miguel Balboa's MFRC522 library
 * https://github.com/sparkfun/WiFly-Shield/ https://github.com/miguelbalboa/rfid
 *
 * Definitions
 * PICC  = This is the RFID card or key fob 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <stdio.h>

#define RST_PIN           9           // Configurable, see typical pin layout above
#define SS_PIN            7         // Configurable, see typical pin layout above
#define BIN_LOCATION_SIZE  6          //number of characters in bin location
#define DEFAULT_READ_SIZE 18        //in order to read, the minimum/default read size is 18

//Arduino kept overwriting this variables so I had to put it here
byte dataBlock[BIN_LOCATION_SIZE+1];

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

//read areas of the card. Doesn't change
const byte sector         = 1;
const byte blockAddr      = 4;
const byte trailerBlock   = 7;


/**
 * Initialize.
 */
void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }


    Serial.println(F("No bin location set. Enter bin location to write to card:"));
   
}

/**
 * Main loop.
 */
void loop() {

/*************************************************************************************
 * TODO://
 * Make all cards the bin location so you only have to assign it once
 * ***********************************************************************************
 */


    //adding null character to end of datablock for reading purposes
    dataBlock[BIN_LOCATION_SIZE+1] = '\0';
    int dataBlock_size = sizeof(dataBlock) / sizeof(dataBlock[0]);

    //check if user has entered a number
    if(Serial.available())
    {
      //receives new bin location and writes it to the datablock array
      get_response(dataBlock,BIN_LOCATION_SIZE);

      //ready to write new number to card
      question_user(dataBlock,dataBlock_size);
    } 
    
   // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    //checks card compatibility
    if (!card_compatible())
      Serial.println(F("This sample only works with MIFARE Classic cards."));
  
    //authenticates cards for writing purposes
    authenticate();

    //writes datablock array to the card
    write_to_block(dataBlock);
  
        
    // Halt PICCt
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();

}


/* *************************************************************************
 * *************************************************************************
 * **************************** FUNCTIONS **********************************
 * *************************************************************************
 * ************************************************************************/

/* Function: to_char
 * Description: helper routine to dump a byte array as ascii values to Serial
 * Inputs: pointer to buffer and the buffer's size
 * Outputs: writes read characters to serial
 */
 void to_char(byte *buffer,byte bufferSize)
 {
  //Iterates through the buffer
    for (byte i=0; i < bufferSize; i++){
      //skips writing characters such as the enter key, backspace etc
     if(buffer[i]> 0x7E || (buffer[i] < 0x20 && buffer[i] >0x9)){
        
      }
      //This will pick up and write all numbers to serial. I have to
      //format as HEX otherwise it might try to write the character value
      //which is like the null character, acknowledge, etc
     else if(buffer[i]< 0x0A){
       Serial.print(buffer[i],HEX);
       }
       
     //everything else is a regular character and I can print it as such.
     //For some reason, serial.write prints out the character value but
     //serial.print will print the decimal value. I'll need to research
     //this more
     else{
       Serial.write(buffer[i]);
       }
      
    }
    Serial.println();
 }

 /* Function: card_compatible
 * Description: Checks rfid card for compatibility
 * Inputs: None
 * Outputs: boolean, true if card compatible, false if not
 */
 boolean card_compatible()
 {
    
    //get card type to check for compatibility
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        
        return false;
    }
    return true;
 }

 /* Function: authenticate
 * Description: Authenticates card to allow writing and reading functions
 * Inputs: none
 * Outputs: none
 */
 void authenticate()
 {
   byte status;
   // In order to read or write data to a PICC, the card must first be
    //authenticated using a key. Below, the PICC is being authenticated using 
    //key A so we can read data from card
   
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Authenticate using key B
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
 }

 
/* Function: read_from_block
 * Description: reads data from a specified block on the card
 * Inputs: empty buffer, size of the data to read, size of the buffer
 * Outputs: will print out the data written on the card
 */
 void read_from_block(byte *buffer, int data_size, byte *buff_size)
 {
    byte status;

    //method reads from the block address(blockAddr) and writes it to the
    //buffer.
    status = mfrc522.MIFARE_Read(blockAddr, buffer, buff_size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    
    // Read data from the block (should now be what we have written)
    Serial.print(F("Card bin location is now: ")); 

    //calls to_char to print out the buffer data
    to_char(buffer,data_size);
    Serial.println();
 }

/* Function: write_to_block
 * Description: Writes data onto the RFID card
 * Inputs: pointer to the data to write
 * Outputs: None
 */
void write_to_block(byte *data)
 {
  //sucks about the variables, I'll need to optimize this later
  byte status, count = 0, buffer[DEFAULT_READ_SIZE], buff_size = DEFAULT_READ_SIZE;
  int num_size = BIN_LOCATION_SIZE + 1;

    Serial.print(F("Writing data onto card...")); 

  //Writes data to the block address (blockAddr)
  //Not sure why I'm using 16 here. Need to research
    status = mfrc522.MIFARE_Write(blockAddr, data, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
  
     // Check that data in block is what we have written 
    for (byte i = 0; i < num_size; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (data[i] == dataBlock[i])
            count++;
    }

    //if data and the card data don't match
    if (count != num_size) {
        Serial.println(F("Failure, no match"));
        Serial.println(F("Perhaps the write didn't work properly..."));
    }
    else {
      Serial.println(" Success!");

      //call read_from_block to print out data on card
      read_from_block(buffer, BIN_LOCATION_SIZE, &buff_size);
     
    }

    //asks user for new bin location or new card
    question_user(data, num_size);
 }

 /* Function: question_user
 * Description: Prompt that tells user the current bin location stored
 *              and asks for a new card or bin location.
 * Inputs: pointer to the current bin location and the length of it
 * Outputs: None
 */
 void question_user(byte *datablock, int len)
 {
 
    Serial.print("Current number stored for writing is: ");
      while (*datablock != '\0')
        Serial.write(*datablock++);
    Serial.println("\nPlease swipe new card or enter new bin location when ready.\n");
   
 }

/* Function: get_response
 * Description: Receives user input and writes it to the dataBlock variable
 * Inputs: pointer to the dataBlock variable and the length to write
 * Outputs: None
 */
 void get_response(byte *buffer, int len)
 {
    int i = 0;

    //While there's still characters in the serial buffer and
    //i is less than the length to read
    while (Serial.available() != 0 && i++ <= len)
    {
      //assigns current buffer address to the next character read from Serial
      *buffer++ = Serial.read();
    }
    Serial.println("New number received.");
 }


