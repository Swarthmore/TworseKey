// EDIT THE LINES BELOW WITH YOUR INFORMATION AMD SAVE AS config.h

// Ethernet board/ahield MAC address (see the sticker on the board)
#define ARDUINO_ETHERNET_MAC { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// If wifi set to 1, Ethernet set to 0
#define WIFI 1

char ssid[] = "<ENTER SSID HERE>";            //  your network SSID (name)
char pass[] = "<ENTER WIFI PASSWORD HERE>";  // your network password

// Twitter token
// Maker location for IFTTT trigger
const char MAKER_URL[] = "maker.ifttt.com";
char MAKER_PAGE[60] = "/trigger/tweet/with/key/";
const int MAKER_PORT = 80;
const char MAKER_KEY[] = "<ENTER MAKER KEY HERE>";

const char TWITTER_HASHTAG[] = " #SWARTHMORSE";
