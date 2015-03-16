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
#include <Ethernet.h>
#include <Twitter.h>

byte mac[] = ARDUINO_ETHERNET_MAC;


// Your token to tweet (get it from http://arduino-tweet.appspot.com/)
Twitter twitter(TWITTER_TOKEN);



// This IP address is used if DHCP fails
byte ip[] = { 192, 168, 2, 2 };

// Message to post
char msg[] = "";

int buzzPin = 5;
int rledPin = 9;
int gledPin = 8;
int ledPin = rledPin;
int morsePin = 7; 
boolean keyDown = false;
boolean paused = false;
unsigned long time = 0;

int DIT = 100;
int DAH = 3*DIT;

String morseCode = "";
String message = "";
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

void setup()
{
  // define the pins
  pinMode(rledPin, OUTPUT);
  pinMode(gledPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);     // output for the buzzer
  pinMode(morsePin, INPUT);     // input from the morse switch 
  digitalWrite(morsePin, HIGH); // turns on pull-up resistor
  
  // initialize connections
  Serial.begin(9600);
  delay(1000);
  connectEthernet();
  time = millis();
}

void connectEthernet() {
  
  paused = true;
  Serial.println("connecting ..."); 
  
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with the default IP address:
    Ethernet.begin(mac, ip);
    Serial.print("using default IP address: ");
  } else {
    Serial.print("got IP address from DHCP: ");
  } 
  
  // play earcon to indicate a successful connection
  tone(buzzPin, 440,100);
  delay(100);
  tone(buzzPin, 660,100);
  delay(100);
  tone(buzzPin, 880,300);
  delay(300);
  
  // change LED color to GREEN
  digitalWrite(rledPin, HIGH);
  ledPin = gledPin;
  
  Serial.println(Ethernet.localIP());
  paused = false;
}


void resetAll() {

    paused = true;
    Serial.println("reset");
    
    // play earcon
    delay(200);   
    tone(buzzPin, 2000,50);
    delay(100);
    tone(buzzPin, 2000,50);
    delay(100);
    tone(buzzPin, 2000,50);
    delay(100);
    
    // reset strings
    morseCode = "";
    message = "";
   
    time = millis();
    paused = false;
    return;
}

/*
void resetWord() {
  
    paused = true;
 
    Serial.println("");
    if (message.length()==0) {
     delay(500);
     tone(buzzPin, 100,1000);
     morseCode = "";
     paused = false;
     return;  
   }

    delay(500);   
    tone(buzzPin, 2000,50);
    delay(100);
    tone(buzzPin, 2000,50);
    delay(100);
    tone(buzzPin, 2000,50);
   
   while (message.endsWith(" ")) {
     message.trim();
   }
   
   int index = message.lastIndexOf(" ");
   if (index<0) {
     message = "";
   } else {
     String tmp;
     Serial.print(String(index)+" ");
     for (int i=0;i<index;i++) {
       tmp+=message.charAt(i);
       Serial.print(message.charAt(i));
     } Serial.println();
     message = tmp+" ";
   }

   morseCode = "";
   Serial.println("del: _"+message+"_");
   
   time = millis();
   paused = false;
   return;
}
*/

void decodeMorse() {
  if (morseCode.length()==0) return;
  paused = true;
  
  // find character from table
  bool decoded = false;
  for (int i=0;i<40;i++) {
     if (morseCode==MORSE_CHARACTERS[i]) {
        message+=LATIN_CHARACTERS[i];
        decoded = true;
        break;
     }
  }

  if (!decoded) message+="*";
  morseCode = "";
  Serial.println("\n"+message);
  if (message.length()==130) sendTweet();
  
  paused = false;
}

void sendTweet() {
  
  if (message.length()==0) return;
  paused = true;
  
  Serial.print("sending tweet ... ");
  message += "#tworse";
  message.toCharArray(tweet,140);
  
  if (twitter.post(tweet)) {
    // Specify &Serial to output received response to serial port
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("done");
      
      // play earcon after successful submission
      tone(buzzPin, 440,100);
      delay(100);
      tone(buzzPin, 660,100);
      delay(100);
      tone(buzzPin, 880,300);
      delay(100);
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
      // play error buzz
      tone(buzzPin, 100,1000);
    }
  } else {
    Serial.println("connection failed.");
  }
  
  message = "";
  time = millis();
  paused = false;
}

void loop()
{  
  if (paused) return;
  
  if (digitalRead(morsePin) == HIGH) {    // the morse key is UP
      if (keyDown) {                      // the key was DOWN before
        unsigned long duration = millis() - time;
        if (duration<20) return;

        digitalWrite(ledPin, LOW);  // turn LED  OFF
        noTone(buzzPin);            // turn BUZZ OFF
        keyDown = false;

      if (duration<(DIT+50)) {           // DIT detected
          morseCode+=".";
          // DIT=(DIT+duration)/2;
          Serial.print(".");
          if (morseCode=="........") resetAll();
      } else if (duration<(2*DAH)) {    // DAH detected
         morseCode+="-";
         //DAH=(DAH+duration)/2;
         //DIT=DAH/3;
         Serial.print("-");
      } /*else if (duration>1000) {        // send tweet after pressing key for more than one second
          sendTweet();
      }*/
               
      time = millis();
     
    } else {                           // the key was UP before
      unsigned long duration = millis() - time;
      if ((duration>60000) && (message!="")) {             // reset after 60 seconds inactivity
        resetAll();
      } else if ((duration>1500) && (morseCode!="")) {     // start a new word after 1.5 seconds
          decodeMorse();
          if (message!="") message+=" ";
          tone(buzzPin, 2000,50);                          // short feedback beep
      }
    }
  } else {               // the morse key is DOWN
    if (!keyDown) {      // the key was UP before
      unsigned long duration = millis() - time;
      if (duration<20) {
        return;
      } else if ((duration>DAH+100) && (duration<=1500)) {
        decodeMorse();   // decode last letter
      }
      
      keyDown = true;
      time = millis();
      
      digitalWrite(ledPin, HIGH);    // turn LED  ON
      tone(buzzPin, 440);            // turn BUZZ ON
    } else {             // the key was already DOWN
      unsigned long duration = millis() - time;
      if (duration>1000) {
        
        digitalWrite(ledPin, LOW);  // turn LED  OFF
        noTone(buzzPin);            // turn BUZZ OFF
        keyDown = false;
 
        sendTweet();                // send tweet after pressing the key for one second
      }
    }
  }  
}

