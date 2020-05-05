


/*
    Colors

   Play tracks from Ken Nordine's 1966 album Colors

   Tracks are triggered by RFID and a LED animation
   is displayed when tracks are plying

*/

#include <SoftwareSerial.h>
#include "FastLED.h"
#include "LedAnimation.h"
#include <BY8001.h>


// RFID
const byte rfidEnable =      9;         // RFID Enable pin
const byte RFID_TAG_LEN  =  10;         // length of a valid tag in bytes
const char RFID_START_BYTE = (char)10;  // char to signal start of tag

// NEOPIXELS
const byte NUM_LEDS = 12;
const byte DATA_PIN = 2;
CRGB leds[NUM_LEDS];
LedAnimation ledShow = LedAnimation();

// BY8001-16P audio module
SoftwareSerial mp3Serial(10, 11);  // RX, TX
BY8001 mp3; // class object
//int trackNo = (-1);
const byte volumePin = A3;
const int MIN_VOLUME = 0;
const int MAX_VOLUME = 30;
byte volume;

// buffer for storing one tag id
char tagBuffer[RFID_TAG_LEN + 1]; // buffer for RFID tag

// Program States

typedef enum { waitingForTag, gotTag, playingTrack, trackComplete } state;
state currentState = waitingForTag;

const bool QUIET = false;  // don't play sound

// Track Info

typedef struct trackinfo {
  char *tagID;        // RFID tag that triggers track
  CRGB  color;        // color for LEDs when track is playing

};

int trackIdx = (-1);  // index of tag, init to illegal value
word trackDuration;   // track duration in seconds


// tag for each color, must be in same order as tracks on SD card
const trackinfo trackInfo[] = {
  "4100469DBA", CRGB::Olive,          // Olive
  "4300069B9C", CRGB::Lavender,       // lavender
  "410043C4F9", CRGB::DarkRed,        // Burgandy
  "410042A83A", CRGB::Yellow,         // Yellow
  "4300091FB7", CRGB::Green,          // Green
  "4100428500", CRGB::Beige,          // Beige
  "4300069E58", CRGB::Maroon,         // Maroon
  "410043AD14", CRGB::Beige,          // Ecru
  "43000529FA", CRGB::Yellow,         // Chartreuse
  "430003A514", CRGB::Turquoise,      // Turquoise
  "4300052A1E", CRGB::WhiteSmoke,     // White
  "5A00523B8A", CRGB::Tan,            // Flesh
  "430009610E", CRGB::Azure,          // Azure
  "43000612AC", CRGB::RosyBrown,      // Pucep
  "5A005327BE", CRGB::Magenta,        // Magenta
  "4300069E38", CRGB::Orange,         // Orange
  "4100439559", CRGB::Purple,         // Purple
  "4100441FCE", CRGB::SaddleBrown,    // Muddy
  "410043C2CF", CRGB::OrangeRed,      // Russet
  "410043953A", CRGB::DarkGoldenrod,  // Amber
  "4300092047", CRGB::Blue,           // Blue
  "430003D2A7", CRGB::Black,          // Black
  "4300092A1E", CRGB::Gold,           // Gold
  "4100428526", CRGB::Crimson,        // Crimson
  "41004366BC", CRGB::Brown,          // Brown
  "5A00523420", CRGB::MistyRose,      // Rosey
  "5A005419E6", CRGB::SandyBrown,     // Hazel
  "5A00534011", CRGB::MediumPurple,   // Mauve
  "5A00521156", CRGB::LightPink,      // Fuschia
  "5A00520A97", CRGB::SandyBrown,     // Sepia
  "5A0051D785", CRGB::Khaki,          // Nutria
  "5A005255A4", CRGB::HotPink,        // Cerise
  "41004428B2", CRGB::Gray,           // Gray
  "4100442C26", CRGB::Coral           // Coral
};



const byte  NUM_TRACKS = 34;

bool wasPlaying = false;    // detect when track stops playng

void setup() {

  // RFID
  Serial.begin(2400);
  pinMode(rfidEnable, OUTPUT);    // RFID Enable

  // BY8001-16P
  mp3Serial.begin(9600);
  mp3.setup(mp3Serial); // tell BY8001 library which serial port to use.
  delay(900);  // wait for BY8001

  // Volume pot
  pinMode(volumePin, INPUT);

  // set volume
  volume = getVolume();
  mp3.setVolume(volume);
  mp3.stopPlayback();

  // NEOPIXEL
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  ledShow.init(leds, NUM_LEDS);


}


void loop(void) {

  // read volume pin
  int newVolume = getVolume();

  // set volume if necessary
  if ( newVolume != volume )
  {
    volume = newVolume;
    mp3.setVolume(volume);

  } // if


  switch ( currentState )
  {
    case waitingForTag:

      // if there is a tag available
      if ( (Serial.available() > 0) && (Serial.read() == RFID_START_BYTE) )
      {

        // read it
        readTag();

        // get its index into track info array
        trackIdx = tagIdx(tagBuffer);


        //        printID(tagBuffer);            // print the TAG code

        // if we got a valid tag
        if ( trackIdx >= 0 )
        {

          // RFID reader is disabled
          digitalWrite(rfidEnable, HIGH);

          // advance state
          currentState = gotTag;


        } // if


      } // if

      break;

    case gotTag:

      // set color and kick off led animation
      ledShow.setColor(trackInfo[trackIdx].color);
      ledShow.begin();

      // play track
      if ( !QUIET ) mp3.playTrackByIndexNumber(trackIdx + 1);

      // let track start to play
      delay(50);

      // get time remaining
      trackDuration = mp3.getTotalTrackPlaybackTime();

      // advance state
      currentState = playingTrack;

      break;

    case playingTrack:

      // update LED animation
      ledShow.update();

      // advance state if track has ended
      if ( trackDuration == mp3.getElapsedTrackPlaybackTime()) currentState = trackComplete;


      break;

    case trackComplete:

      // stop animation
      ledShow.end();

      // flush any bytes in the serial buffer to prevent double reads
      while ( Serial.available() ) Serial.read();

      // set state
      currentState = waitingForTag;

      // enable RFID reader
      digitalWrite(rfidEnable, LOW);

      break;

    default:
      break;

  } // switch


}


// tagIdx
//
// Return index of passed tag or -1 if not found
//
int tagIdx(char *tag)
{

  int retVal = (-1);

  // compare each known tag to passed tag
  for (int i = 0; i < NUM_TRACKS; i++)
  {

    // if we found a match
    if ( !strcmp(tag, trackInfo[i].tagID) )
    {
      // set return value
      retVal = i;

      // stop looking
      break;
    }


  } // for

  return (retVal);

}

// readTag
//
// Read a RFID tag into buffer.
// Call after reading a RFID start byte.
//
void readTag() {

  int cnt;

  // read a full tag's worth of chars
  for (cnt = 0; cnt < RFID_TAG_LEN; ) if ( Serial.available() > 0 )  tagBuffer[cnt++] = (char)Serial.read();

  // terminate string
  tagBuffer[cnt] = '\0';

  // read stop byte
  if ( Serial.available() > 0 ) Serial.read();;

}


// printID
//
// Print a tag ID to the serial port.
//
void printID(char *id)
{
  // while more bytes
  for (int c = 0; c < RFID_TAG_LEN; c++)
    // print this byte
    Serial.write(*id++);

  // add newline
  Serial.print(F("\n"));

}

// getVolume
//
// Return set volume level based on pot
//
int getVolume() {

  return (map(analogRead(volumePin), 0, 1023, MIN_VOLUME, MAX_VOLUME));

}
