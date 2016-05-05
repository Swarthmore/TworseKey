This is an update of the [original project by Martin Kaltenbrunner](http://modin.yuri.at/tworsekey).

Tworse Key is an Arduino-powered Telegraph that transmits its messages to Twitter.  Inspired by Martin Kaltenbrunner's original 2012 project, students, faculty, and staff at Swarthmore College have updated the code to work with the newer Twitter API and added the ability to work with Wifi or Ethernet connections.  

Students in Professor Rachel Buurma's Victorian Novel Research Seminar at Swarthmore College built a bunch of Twitter telegraphs.

![image of wifi Twitter telegraph](https://lh3.googleusercontent.com/L02vxvcqnI_aJqlpOlvz9yQV1sykd9xZOuwSR3vdNPqIrvYjRj3Nb9NSxXWy8DGmM5ELPkIyb-HGlXI2lQlwzxKvjPnNICS9roqN3yR-_pXc6AnJ9x_dknc8qKv7bbuUJW2GJueqtCfxj1X24WizBp3ydMcxCI7p5xNIp5lhBP7gXx_B34gGgOoR4xIk0mauP5L5Sy9te_qb0CckUN2KW803__KYdZEcrgqCkC_YU2Qd-6pOw0VMmBL78JfodfzjIrKOaIVyUWIdOzoYfQfGt4V3yXfPJcVwogJp8HbBVXGy51NDY4d0pQSpzc5_nxzlyGQb-0utbFdXNlhn9y9TyoxvuDYFyx4dxlDnSVzsGYsE3n3DotesAz2dZYshgrj5lfrSCgxhBvat7kL0fwE5r9SMPZOyUcmAeyjDkREmzZx8Q473j9Rty7UWhkuZYQjyzC7Ajg0_JzpH7K1rAvmNr8uhSwTxxz9K0anaj8lFtim_tGTxmgJ4Cwtpdxh8Htcq4qsTBOhdUe8FZEXaFVNWZwg53Ymf6pXjo2mdYMoLQG5UZNtwqQxvhsQrO0lGto96PjMR=w1280-h505-no "Image of Twitter telegraph")

![image of Twitter telegraph](https://lh3.googleusercontent.com/a3qRwrhaUF4KljRIwV_GwITTfKGbxTCJTrInBhon1momFC-v_FUTPCWwkMqELqrP0EKd9cXUNiN77tdIrNlYi3J5wZ8Qywy624uaF-EXysKpKRsShuUgi-t3wT3SbatXGx0aZWwyHR7FLoMZULJGYbNn4RM2iGjbGmTlTC4FxRDLB9PUcCBQEVPe35HZIHMvws0uTmCanfw6B0jOr5CZc8DDWcqNnbzsrypXMxQLSF6XYcJAZnRBRe73BVqA9ehpWCOI3dqQVh0gOut8_U88_CtOhu2tJsSeS1XB__pEvPxED7GgAn940zndsTFThUtNs8HPzlifSgpVaH_TXXeP-gwvLd59c7Cx5NAJjRZG7rxm0PJH20W6P23UGOOML23yVoICSzlVRD1Qm7olMrmvLRH49FUNo24oUkgiahtvKIIuglmUzBbf46mdMRiP05OirhboYv8chNaDig0aq-lpDG0NJt4Qjkf6un83KrMx5Pxiy5ssV5RjbaDkuRhX6DlW7YzXytxCQcQcnBHDdW5zHGI7XOibO86-EG_jL8KmDjxpBgchO7NJK_62uBYlnShwOfyG=w1280-h720-no  "Image of Twitter telegraph")

# Hardware
Part| Notes
--- | --- 
Arduino Uno | Or Equivalent.  We purchased an [equivalent model from Sparkfun](https://www.sparkfun.com/products/12757).
Arduino Ethernet shield or Wifi shield | We purchased Sparkfun: [Ethernet](https://www.sparkfun.com/products/11229) or [Wifi](https://www.sparkfun.com/products/11287)
Morse Key|Model AM-K1 Telegraph Key sourced from [Morse Technologies](http://mtechnologies.com/) 
Piezo buzzer| We purchased a [12mm model from Sparkfun](https://www.sparkfun.com/products/7950) and [Amazon](http://www.amazon.com/Terminals-Electronic-Continuous-Sound-Buzzer/dp/B00B0Q4KKO/ref=sr_1_7)
RGB LED (common anode)| We purchased [this part from Sparkfun](https://www.sparkfun.com/products/10821)
330&Omega; resistor| for red portions of the RGB LED
Two 160&Omega; resistors| for green and blue portions of the RGB LED
50&Omega; resistor|For the buzzer
Breadboard | To plug in the electrical components. [A small one works fine](https://www.sparkfun.com/products/12046)
Hookup wire | To connect the parts 
Box or Wooden board | To mount the components


# Hardware setup 
Hook up the parts as in the diagram below
![circuit diagram](https://raw.githubusercontent.com/Swarthmore/TworseKey/master/Circuit%20Diagram_bb.png "Circuit diagram")

# Software setup
* Open the Arduino program: https://www.arduino.cc/en/Main/Software
* If you are using Ethernet, download the Ethenet2 Arduino library from https://github.com/adafruit/Ethernet2/archive/master.zip
  * The regular Ethernet library only works with older Ethernet shields.  The Ethernet 2 library is required for newer shields.
  * Unzip the master.zip file, Find the "src" folder, and copy it into the Documents → Arduino → libraries folder.
* In the Arudino app, under  the "Tools → Board" menu, select "Arduino / Genuino Uno"
* Connect your Arduino to your computer with the USB cable
* Under the "Tools → Port" menu select something like "dev/cu.usbmodem"
* Download TworseKey program from GitHub: (https://github.com/Swarthmore/TworseKey) by clicking on the "Download Zip" option.
* Unzip the file, open the folder, and double click on the "TworseKey.ino" to open up the code.

## Twitter connection
The Arduino is not sophisticated enough to authenticate to Twitter and send a Tweet.  We are using IFTTT.com to send the tweets for us.  
* Go to IFTTT.com, create an account, and then make a new recipe.  
* The Recipe is: If Maker Event "tweet", then post a tweet to <your Twitter handle>
  * You will need to add the Maker and Twitter channels and do whatever authentication is required.

## Configuration
* Open the config_dist.h file (should be a tab in the Arduino program)
* Open a new tab (arrow on far right) and create a config.h file.  Copy and paste the content from config_hist.h to config.h
* Modify config.h with the MAC address from your Ethernet or Wifi board.  Include "0x" in front of each set of two characters
* Edit the code for Wifi or Ethernet
* Enter your Maker Key set up from IFTTT.com
* Modify hashtag(s) as you see fit
* Save config.h
* Click the check button on the top left of the window to verify everything is ok. 
* If so, click the right arrow button to upload your code to the Arduino

# Operation
After the Arduino is first powered up, it will look for a network connection. The LED will be yellow.  The wifi can take a while to connect.  Once a network connection is established, the LED will turn green and tone will sound.

Press the Morse key down for a short time for a dot and longer time for a dash.  The defaults are 150mS or shorter is a dot and 150mS to 600mS is a dash.  These can be modified in the main program. Pauses for more than a 1.5 seconds will automatically add a space.

Press the key down for more than 3 seconds to send the message to twitter.  The LED will turn blue while the message is sent.  A tone will indicate success or failure.  

Type 8 dots to reset your message.

You can use the Arduino serial monitor to see what you are typing.  

If you want to test out the system without sending lots of crazy tweets, you can update your IFTTT.com recipe to record your messages to a Google spreadsheet instead of sending to Twitter.  


# Original README
Tworse Key http://modin.yuri.at/tworsekey
(c) 2012 by Martin Kaltenbrunner

The Tworse Key is an open design exercise in interface archaeology,  that decodes the input from a classic Morse key to send twitter messages. This standalone device connects through a standard LAN cable, the Morse signals are decoded by the built-in Arduino Ethernet board, which delivers the final message though the Twitter API. 

This project is fully documented in order to illustrate the Open Design process for beginners, the source code and building instructions are therefore available under the according free licenses. 

This project is using NeoCat's Twitter library:
http://arduino-tweet.appspot.com/

All source code is available under the GPL 3.0 and the hardware schematics are available under a Creative Commons 3.0 BY-SA license. You can find all documentation and building instructions on the project page.
