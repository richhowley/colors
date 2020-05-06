# Colors

## The Experience

In 1966 Ken Nordine recorded an album all about colors appropriately named [Colors](https://music.apple.com/us/album/colors/526618816). There are 34 tracks, each telling the story of a specific hue. Delivered in Nordine’s trademark “Word Jazz” style, it’s clever and inventive.  As they would say in the 60’s, I dig it.

This project is about using physical computing to present Nordine’s work to a modern audience.  Hold a color chip up to a wooden box housing a speaker and it comes to life with the voice of Ken Nordine telling a story about the color in your hand.  The presentation is jazzed up a bit with LED animation.

## Hardware

A custom-built wooden box has a circular cutout for.a speaker that was rescued from a broken radio and a panel mount volume pot. Holes cut in a circle pattern using a drill press glow with light from RGB LED animations when audio is playing. 

Inside the box is a hand-soldered proto board that houses an Arduino Pro Mini and a BY8001-16P audio module.  A ring of 12 Adafruit NeoPixels is used for the LED animations.  The NeoPixels have their own power source to prevent interference with audio playback.

Also in the box is a RFID reader.  A small piece of Lexan with a sticker that has an icon of a microphone on it serves as a hint of where to tap a color chip.  There is no problem reading RFID tags through 1/4” plywood and the Lexan.  

The color chips are cut from paint color samples from a home supply store.  Two color chips are glued back to back with a RFID tag sandwiched between them. One side of each chip has a sticker identical to the one on the Lexan.

## Software

In the program an array stores the RFID tags for each color along with a color code to use with the FastLED library.   The array is arranged in order of the tracks on the SD card.

When a valid tag is read its index in the tag array is used to play the correct track.  The RFID reader is disabled until the track is completed.

As audio is playing the LEDs are lit in animated patterns.  An animation class handles the LEDs while the main loop monitors the progress of the audio playback and volume pot changes.  

The animation class is designed so that the calling code sets the desired color for the animations, tells it to begin and end animations and regularly calls an update method.  Inside the class the code selects animation sequences at random and switches sequences at regular intervals.  Everything works based on the internal timer so the main loop can be responsive to track playback and volume control.

![nordingreen](https://user-images.githubusercontent.com/318132/81127657-eace7580-8f0c-11ea-9f6f-8cd5601da396.jpg)
