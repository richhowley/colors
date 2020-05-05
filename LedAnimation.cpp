#include "LedAnimation.h"

/*
 * Class for handleing LED animaitons.
 * 
 * Create an array of CRGB, add them to FastLED adn call init() with the array and length
 * 
 * Before starting animations call setColor() with the desired color
 * 
 * Call begin() to start animations
 * 
 * Call update() each time through loop()
 * 
 * The class will play animations until end() is called 
 * 
 */


const int brightLevel = 180;

LedAnimation::LedAnimation()
{

}

// init
//
// Pass array of leds that have been added to FastLED
//a
void LedAnimation::init(CRGB *pixels, const int cnt)
{

  // store led array and set count
  leds = pixels;
  ledCount = cnt;

  // turn off all LEDs
  FastLED.showColor(CRGB::Black);

}


// oddOn
//
// Odd numbered pixels show color
//
void LedAnimation::oddOn()
{

  for (int i = 0; i < ledCount; i++)
  {
    leds[i] = (i % 2) ? color : CRGB::Black;
  }
  FastLED.show();
}

// evenOn
//
// Even numbered pixels show color
//
void LedAnimation::evenOn()
{

  for (int i = 0; i < ledCount; i++)
  {
    leds[i] = !(i % 2) ? color : CRGB::Black;
  }
  FastLED.show();
}

/*
 * Each animation has a begin and update method.
 * 
 * The update method gets called based on its update frequency.
 * 
 */

// chaserBegin
//
void LedAnimation::chaserBegin()
{
  updateFreq = 60;

  FastLED.setBrightness(brightLevel);

  // all LEDs off
  FastLED.clear();

}

// Spot Light CW
//
void LedAnimation::spotLightBegin()
{

  updateFreq = 50;

  FastLED.setBrightness(brightLevel);

  // set all LEDs
  for (int i = 0; i < ledCount; i++) leds[i] = color;
  FastLED.show();

}

// Half Spot Light CW
//
void LedAnimation::halfSpotLightBegin()
{

  updateFreq = 50;

  FastLED.setBrightness(brightLevel);

  // odd pixels on
  oddOn();

}

// Wavy Fade
//
void LedAnimation::wavyFadeBegin()
{

  updateFreq = 50;


  FastLED.setBrightness(brightLevel);


  // set all LEDs
  for (int i = 0; i < ledCount; i++) leds[i] = color;
  FastLED.show();


}

// Even Odd
//
void LedAnimation::evenOddBegin()
{

  updateFreq = 150;

  FastLED.setBrightness(brightLevel);

  // even leds on
  evenOn();
  FastLED.show();


}



// chaserUpdate
//
//
void LedAnimation::chaserUpdate()
{

  *(leds + (ticks % ledCount)) =  CRGB::Black;
  ticks++;
  *(leds + (ticks % ledCount)) = color;
  FastLED.show();

}


// spotLightCWUpdate
//
void LedAnimation::spotLightCWUpdate()
{
  leds[ticks % ledCount].fadeLightBy( 128 );
  ticks++;
  leds[ticks % ledCount] = color;
  FastLED.show();

}

// halfSpotLightCWUpdate
//
void LedAnimation::halfSpotLightCWUpdate()
{
  leds[ticks % ledCount].fadeLightBy( 128 );
  ticks++;
  FastLED.show();

}

// spotLightCWUpdate
//
void LedAnimation::spotLightCCWUpdate()
{
  leds[map(ticks % ledCount, 0, 11, 11, 0)].fadeLightBy( 128 );
  ticks++;
  leds[map(ticks % ledCount, 0, 11, 11, 0)] = color;
  FastLED.show();

}

// wavyFadeUpdate
//
void LedAnimation::wavyFadeUpdate()
{
  leds[ticks % ledCount].fadeLightBy( 128 );
  ticks++;
  leds[map(ticks % ledCount, 0, 11, 11, 0)] = color;
  FastLED.show();

}

// evenOddUpdate
//
void LedAnimation::evenOddUpdate()
{

  (++ticks % 2) ? oddOn() : evenOn();

}


// newAnimation
//
// Select an animation at random.
//
// Call the begi method directly and set a poiter to its update method
//
void LedAnimation::newAnimation()
{
  // init ticks and last update
  ticks = 0;
  lastTick = 0UL;

  // select an animation effect
  animationEfx = random(efxend);

  switch ( animationEfx)
  {

    case chaser:
      chaserBegin();
      updateAnimation = &chaserUpdate;
      break;

    case spotlightcw:
      spotLightBegin();
      updateAnimation = &spotLightCWUpdate;
      break;

    case spotlightccw:
      spotLightBegin();
      updateAnimation = &spotLightCCWUpdate;
      break;

    case wavyfade:
      wavyFadeBegin();
      updateAnimation = &wavyFadeUpdate;
      break;

    case halfSpotlight:
      halfSpotLightBegin();
      updateAnimation = &halfSpotLightCWUpdate;
      break;

    case evenOdd:
      evenOddBegin();
      updateAnimation = &evenOddUpdate;
      break;

    default:
      break;

  }

  // mark start time
  eftStartTime = millis();


}


// setColor
//
// Call to set LED color based on track selected
//
void LedAnimation::setColor(CRGB c)
{
  color = c;
}

// begin
//
// Call when starting to play audio track
//
void LedAnimation::begin()
{

  newAnimation();
}

// update
//
// Call while track is playing
//
void LedAnimation::update()
{

  // if it's time for a new animation
  if ( millis() >= (eftStartTime + EFXRUNTIME) )
  {

    // start animation
    newAnimation();

  } else {

    // if it's time for next update
    if ( millis() >= (lastTick + updateFreq) )
    {

      (this->*updateAnimation)();
      lastTick = millis();

    } // if

  } // else

}


// end
//
// Call when track has finished playing
//
void LedAnimation::end()
{

  // turn off all LEDs
  FastLED.showColor(CRGB::Black);

}
