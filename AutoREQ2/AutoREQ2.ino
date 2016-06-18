#include <stdio.h>
#include <SPI.h> //from wifly library
#include <WiFly.h> //from wifly library
#include <MFRC522.h>
//contains server address as well as SSID
//credentials
#include "Credentials.h" 
/*
 * Pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       
 *             Reader/PCD   Uno           
 * Signal      Pin          Pin           
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             
 * SPI SS      SDA(SS)      7            
 * SPI MOSI    MOSI         11 / ICSP-4   
 * SPI MISO    MISO         12 / ICSP-1   
 * SPI SCK     SCK          13 / ICSP-3   
 * 
 * This program will allow the arduino to read an RFID card 
 * and send data to a listening port on a server.
 * 
 * Thanks to SparkFun's wifly shield library and Miguel Balboa's MFRC522 library
 * https://github.com/sparkfun/WiFly-Shield/ https://github.com/miguelbalboa/rfid
 *
 * Definitions
 * PICC  = This is the RFID card or key fob 
 *
 */
 
 

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          7          // Configurable, see typical pin layout above
#define MIN_BUFFER_SIZE 18         // Minimum buffer size for read function
#define DEST_PORT 5000              //Arbtrary port number, can change
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

void setup() {
  
   Serial.begin(9600);
 
  //start up wifly
  WiFly.begin();

  //Connect to network
  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }  
  }
  
  //print out wifly IP
  Serial.println(WiFly.ip());
  
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  
 // Prepare the key (used both as key A and as key B)
 // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (int i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
  }

}

void loop() {

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    
    //will need to get rid of this. This is the
    //actual value in the block of data I'm reading
    byte dataBlock[] = "DYND12345";
    byte trailerBlock   = 7;
    byte status;
    byte buffer[MIN_BUFFER_SIZE];
    byte size = sizeof(buffer);

    // In order to read or write data to a PICC, the card must first be
    //authenticated using a key. Below, the PICC is being authenticated using 
    //key A so we can read data from card
    Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

    //check if authenticated
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Read data from the block
    Serial.print(F("Reading data from block ")); 
    Serial.print(blockAddr);
    Serial.println(F(" ..."));

    //reads data from the specified block address and puts
    //it into the buffer
    status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);

    //If read status is ok, call sendMessage function
    if (status == MFRC522::STATUS_OK) {
        sendMessage(buffer,sizeof(dataBlock));
    }
    else{
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    
    

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
  
}

/*  Function sendMessage
 *  Description: Function sends data over to the server. Data is passed in the
 *  buffer variable with the size of the data as the int. In the beginning of
 *  the function, the Wifly connects to the server via port 5000. Port 5000
 *  can be configured to another port if needed. Once the Wifly client has
 *  been connected, the buffer is iterated through and passed as chars to
 *  the server using client.print(). Once the data size cap is reached, the wifly
 *  disconnects from the socket.
 *  Inputs: pointer to a byte and an int
 *  Outputs: None.
 */
 void sendMessage(byte *buffer,int dataSize)
 {
  //Creates WiFly client attached to the server through port 5000
   WiFlyClient client(server, DEST_PORT);
     if(client.connect())
    {
    
      Serial.println("Connected!");
      char c; //char variable for buffer value
      
      //converts decimals in buffer to chars and sends across to server
      for (int i=0; i < dataSize; i++)
      {
         client.print(c = buffer[i]);
      }
     
      //disconnects from socket
      client.println();
      client.stop();
    }
    else
    {
      Serial.println("Connection failed. Is port open?");
     
    }
    // If we're disconnected, stop the client:
    if (!client.connected()) 
    {
      Serial.println(F("Disconnecting."));
      client.stop();
    }
 }

