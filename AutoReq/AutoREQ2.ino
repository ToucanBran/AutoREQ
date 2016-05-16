// (Based on Ethernet's WebClient Example)

#include <SPI.h> //from wifly library
#include <WiFly.h> //from wifly library
//contains server address as well as SSID
//credentials
#include "Credentials.h" 

/**
 * This program will allow the arduino to read an RFID card 
 * and send data to a listening port on a server.
 * 
 * Thanks to SparkFun's wifly shield library:
 * https://github.com/sparkfun/WiFly-Shield/
 */
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
}

void loop() {
/*
 * Testing loop for now, trying to see how
 * sending a message to a port on the server
 * works. Eventually, I'll bring in the RFID
 * reading code.
 * 
 * There's an issue though, I set up
 * and open port using netcat but it keeps
 * getting closed after the arduino messages
 * it once.
 * 
 * I'm thinking about using Python to listen
 * on a port and accept/handle data
 * 
 */
  Serial.println("sending message");
  messageServer();

}

void messageServer(){

  //create a connection to the server on port 5000(arbitrary)
  WiFlyClient client(server, 5000);
 
  if(client.connect())
  {
    Serial.println("Connected!");
    client.print("Hello, this is arduino");

    //closes connection. However, I think this also ends
    //up closing the port on servers end
    client.println("Connection: close");
    client.println();
  }
  else
  {
    Serial.println("Connection failed!!");
  }
// If we're disconnected, stop the client:
  if (!client.connected()) 
  {
    Serial.println(F("Disconnecting."));
    client.stop();
  }
  
}

