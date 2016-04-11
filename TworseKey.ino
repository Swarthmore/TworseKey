/*
Copyright (c) 2012 Martin Kaltenbrunner
http://modin.yuri.at/tworsekey/

TworseKey is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "config.h"
#include <SPI.h>
#include <Ethernet2.h>

byte mac[] = ARDUINO_ETHERNET_MAC;

// Define constants for LED colors
#define BLACK 0 // Off
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define PURPLE 5
#define AQUA 6
#define WHITE 7

#define buzzPin  5
#define rledPin A0
#define gledPin A1
#define bledPin A2
#define morsePin A3

#define DIT 100 // mS
#define DAH 300 // mS



EthernetClient client;

// This IP address is used if DHCP fails
byte ip[] = { 192, 168, 2, 2 };

// Message to post
char msg[] = "";

boolean keyDown = false;
boolean paused = false;
boolean addedSpace = false;
unsigned long time = 0;

String morseCode = "";
String message = "";
int maxMessageLength = 140;
char tweet[140];

char LATIN_CHARACTERS[] = {
        // Numbers
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9',
        // Letters 
        'a', 'b', 'c', 'd', 'e', 
        'f', 'g', 'h', 'i', 'j', 
        'k', 'l', 'm', 'n', 'o', 
        'p', 'q', 'r', 's', 't', 
        'u', 'v', 'w', 'x', 'y', 'z', 
        // Special
        '.', '?', '@', ' '
    };

String MORSE_CHARACTERS[] = {
        // Numbers
        "-----", ".----", "..---", "...--", "....-",
        ".....", "-....", "--...", "---..", "----.", 
        // Letters
        ".-",    "-...",  "-.-.",  "-..", ".", 
        "..-.",  "--.",   "....",  "..",  ".---",
        "-.-",   ".-..",  "--",    "-.",  "---",
        ".--.",  "--.-",  ".-.",   "...",  "-",     
        "..-",   "...-",  ".--",   "-..-", "-.--",  "--..",
        // Special
        ".-.-.-", "..--..", ".--.-.", " "
    };
    
    



    
// Program setup    
void setup() {
  
  // define the pins
  pinMode(rledPin, OUTPUT);
  pinMode(gledPin, OUTPUT);
  pinMode(bledPin, OUTPUT);  
  pinMode(buzzPin, OUTPUT);     // output for the buzzer
  pinMode(morsePin, INPUT);     // input from the morse switch 
  digitalWrite(morsePin, HIGH); // turns on pull-up resistor
  
  // Set LED color
  set_led_color(YELLOW);

  // Setup URL
  strcat(MAKER_PAGE, MAKER_KEY);
  
  // initialize serial and ethernet connections
  Serial.begin(9600);
  delay(1000);
  connectEthernet();

  // Calculate the maximum message length (based on extra hashtags)
  maxMessageLength = 140 - strlen(TWITTER_HASHTAG);

  // Start timer
  time = millis();
}




// Setup Ethernet connection
void connectEthernet() {
  
  paused = true;
  Serial.println(F("Connecting to Ethernet...")); 
  
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with the default IP address:
    Ethernet.begin(mac, ip);
    Serial.print(F("using default IP address: "));
  } else {
    Serial.print(F("got IP address from DHCP: "));
  } 
  
  // Play tone to indicate a successful connection
  tone(buzzPin, 440,100);
  delay(100);
  tone(buzzPin, 660,100);
  delay(100);
  tone(buzzPin, 880,300);
  delay(300);
  
  // change LED color to GREEN
  set_led_color(GREEN);
  
  Serial.println(Ethernet.localIP());
  paused = false;
}




// Reset Morse Code message.  
// This is used when there is a timeout or after a message is sent out
void resetAll() {

    paused = true;
    set_led_color(RED);
    Serial.println(F("Reset message"));
    
    // play reset tone
    delay(200);   
    tone(buzzPin, 2000,50);
    delay(100);
    tone(buzzPin, 2000,50);
    delay(100);
    tone(buzzPin, 2000,50);
    delay(100);
    
    // reset strings and timer
    morseCode = "";
    message = "";
    time = millis();
    
    paused = false;
    set_led_color(BLACK);
}




// Given a Morse Code letter in the morseCode variable, look up the corresponding 
// letter and add it to the message variable.
void decodeMorse() {
  
  if (morseCode.length()==0) return;  // Don't bother decoding if the function is called without anything in the morseCode variable
  
  paused = true;
  set_led_color(PURPLE);
  
  // find character from table.
  // Loop through all the possibilities, until a match is found (or reach the end of the possibilities)
  bool decoded = false;
  for (int i=0;i<40;i++) {
     if (morseCode==MORSE_CHARACTERS[i]) {
        message+=LATIN_CHARACTERS[i];
        decoded = true;
        break;
     }
  } 

  // User feedback based on decoding success or failure
  if (!decoded) {
    // Couldn't decode the Morse code character.  
    // Add "*" to the message, play the decode error buzz, and show the red light.
    message+="*";
    set_led_color(RED);  
    tone(buzzPin,50);
    delay(50);
    tone(buzzPin,262, 100);
    delay(100);
  } else {
    // Decoding successful - play a happy tone
     tone(buzzPin, 2000,50);            
  }

  // Clear morseCode variable to get ready for the next letter and print out the message so far.
  morseCode = "";
  Serial.println("\n"+message);

  // If we reach the max message length, send out the Tweet.
  if (message.length()>=maxMessageLength) sendTweet();
  
  paused = false;
  set_led_color(BLACK);
}



// Send message to Maker webhook.  
// This will trigger IFTTT to send a Tweet.
byte httpPOST(char const* domainBuffer,int port,char const* page,char* msg) {
  
  char jsonMessage[164] = "{\"value1\":\"";
  strcat(jsonMessage, msg);
  strcat(jsonMessage,"\"}");
  boolean result = false;

  Serial.print(F("Message: "));
  Serial.println(jsonMessage);
  
  if(client.connect(domainBuffer, port)) {
    
    // send the header
    client.print(F("POST "));
    client.print(page);
    client.print(F(" HTTP/1.1\r\n"));

    client.print(F("Host: "));
    client.print(domainBuffer);
    client.print("\r\n");
    
    client.print(F("Connection: close\r\nContent-Type: application/json\r\nContent-Length: "));
    client.print(strlen(jsonMessage));
    client.print(F("\r\n\r\n"));
    
    // send the body (variables)
    client.println(jsonMessage);
    
  } else {
    Serial.println(F("POST failed"));
  }
  
  if(client.connected()) {
    Serial.println(F("Waiting for response"));
    if (client.find("HTTP/1.1") && client.find("200 OK") ) {
      Serial.println(F("Message received"));
      result = true;
    } else {
      Serial.println(F("Message not received"));
    }
  } else {
    Serial.println(F("Message not received"));
  }
 
  client.stop();
  client.flush();
  return result;
}





// Set LED color
void set_led_color(int color) {
  
  // Turn all LED's off
  digitalWrite(rledPin, HIGH);
  digitalWrite(gledPin, HIGH);  
  digitalWrite(bledPin, HIGH);  
  
   switch(color)  {
     case RED:
       digitalWrite(rledPin, LOW);
       break;     
     case GREEN:
       digitalWrite(gledPin, LOW);
       break;       
     case BLUE:
       digitalWrite(bledPin, LOW);
       break;       
     case YELLOW:
       digitalWrite(rledPin, LOW);
       digitalWrite(gledPin, LOW);
       break;  
     case PURPLE:
       digitalWrite(rledPin, LOW);
       digitalWrite(bledPin, LOW);
       break;      
     case AQUA:
       digitalWrite(gledPin, LOW);
       digitalWrite(bledPin, LOW);
       break;   
     case WHITE:
       digitalWrite(rledPin, LOW);
       digitalWrite(gledPin, LOW);       
       digitalWrite(bledPin, LOW);
       break;         
   }
   
}


// Main program loop
void loop() {  
  
  if (paused) return; // Skip if in the middle of something else
 
  unsigned long duration = millis() - time;  // Figure out how long the key has been pressed/released
  
  if (digitalRead(morsePin) == HIGH) {     
    
    // the morse key is currently UP
    if (keyDown) {       
         // the key was DOWN before (i.e. the key was just released)            
        // If the key was pressed for a very short time, just ignore
        if (duration<20) return;

        // Otherwise, turn off the LED and buzzer, and set KeyDown to false to indicate the key is no longer being pressed
        set_led_color(BLACK);        // turn LED  OFF
        noTone(buzzPin);             // turn BUZZ OFF
        keyDown = false;
        addedSpace = false;          // Space has not been added

        // Determine if key was pressed for a DIT or DAH duration
        if (duration < (DIT+50) ) {           // DIT detected
          morseCode+=".";
          Serial.print(".");
          
          // If there are a series of DITs -- reset the message
          if (morseCode=="........") resetAll();
          
        } else if (duration<(2*DAH)) {    // DAH detected
          morseCode+="-";
          Serial.print("-");
        }
               
        time = millis();  // Reset the time.  This indicates how long the key is up for
      
    } else {   
      
      // the key was UP before (i.e. the key was up and still is up)
      if ( (duration>60000) && (message!="") ) {             // Reset the message after 60 seconds inactivity     
        resetAll();
       } else if ( (duration>DAH+100) && (duration<=1500) ) {  
        // If the key was up for between a DAH and 1.5 seconds, that is the 
        // end of a character (starting a new character)
        decodeMorse();   // decode last letter  
      } else if (duration>1500 && message!="" && !addedSpace) {     // start a new word after 1.5 seconds
          tone(buzzPin, 200,50);                                    // short feedback beep
          message += " ";
          addedSpace = true;
      }
    } // End of key UP
    
  } else {               
    
    // the morse key is currently DOWN   
    if (!keyDown) {      // the key was UP before (i.e. the key was just pressed)

      if (duration<20) {return;}     // If the key was released for a very short time, just ignore
        
      // Otherwise, turn on the LED and buzzer, and set KeyDown to true to indicate the key is being pressed
      set_led_color(GREEN);
      tone(buzzPin, 440);            // turn BUZZ ON
      keyDown = true;
      
      time = millis();    // Reset the time.  This indicates how long the key is down for
      
    } else {             // the key was already DOWN (i.e. the key was already being pressed before this loop)
    
      if ( ( duration > (DIT+50))  && (duration < 2*DAH) ) {
        set_led_color(AQUA);              // Change LED color for DAH
      } else if ( (duration > (2*DAH)) && (duration<1000) ) {
        set_led_color(WHITE);             // Turn off LED for greater than DAH
      } else if (duration>1000) {         // send tweet after pressing the key for one second
        
        // Turn off LED and buzzer, set keyDown to false (to reset timer)
        set_led_color(BLACK);
        noTone(buzzPin);            
        keyDown = false;
 
        sendTweet();         // Send the Tweet!       
      }
      
    } 
  }  // End of key up/down if statement
}




// Send out the Twitter message
void sendTweet() {

  // Don't bother if there isn't a mesage
  if (message.length()==0) return;
  
  // Switch LED to blue and pause other operations
  set_led_color(BLUE);
  paused = true;

  // Add the hashtag and convert the message String to a character array with max length 140 characters
  Serial.print(F("sending tweet ... "));
  message += TWITTER_HASHTAG;
  message.toCharArray(tweet,140);

  // POST the message to the MAKER URL
  if (httpPOST(MAKER_URL, MAKER_PORT, MAKER_PAGE, tweet)) {
      // Success! PLay happy tone
      tone(buzzPin, 440,100);
      delay(100);
      tone(buzzPin, 660,100);
      delay(100);
      tone(buzzPin, 880,300);
      delay(100); 
      
  } else {
      
      Serial.print("failed");
      // play error buzz and show red LED.  Then switch to green
      set_led_color(RED);
      tone(buzzPin, 100,1000); 
  }

  // Reset variables and get ready for next message
  set_led_color(GREEN);
  message = "";
  time = millis();
  paused = false;
}


