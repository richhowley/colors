
#include "FastLED.h"

typedef enum {
  chaser,
  spotlightcw,
  spotlightccw,
  wavyfade,
  halfSpotlight,
  evenOdd,
  efxend

} efx;

class LedAnimation
{
  public:
    LedAnimation();
    void init(CRGB *leds, const int cnt);
    void setColor(CRGB color);
    void begin();
    void update();
    void end();

  private:
    void allOff();
    void chaserBegin();
    void chaserUpdate();
    void fadeChaserBegin();
    void halfSpotLightBegin();
    void fadeChaserUpdate();
    void spotLightBegin();
    void evenOddBegin();
    void evenOddUpdate();
    void spotLightCWUpdate();
    void spotLightCCWUpdate();
    void halfSpotLightCWUpdate();
    void wavyFadeBegin();
    void wavyFadeUpdate();
    void (LedAnimation::*updateAnimation)();
    void newAnimation();
    void oddOn();
    void evenOn();
    
    CRGB color;       // color based on selected track
    CRGB *leds;       // array of led objects
    int ledCount;     // # of leds in array
    unsigned long updateFreq; // how often to update animation
    unsigned long lastTick;   // used to determine if it's fime for next frame
    int ticks;                // frame count during animation
    const unsigned long EFXRUNTIME = (1000*5);  // time that one animation will play
    unsigned long eftStartTime;
    efx animationEfx;

};
