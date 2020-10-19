#include <FastLED.h>
#include <Adafruit_CircuitPlayground.h>


#define NUM_LEDS 10
#define BRIGHTNESS 96

const float circleRadius = NUM_LEDS / PI;
const float angle = 2 * PI / NUM_LEDS;
float xOffsets[NUM_LEDS];
float yOffsets[NUM_LEDS];

int speedH = 4;   //Hue value is 16-bit
int scaleH = 50;
int speedS = 7;
int scaleS = 30;
int speedV = 10;
int scaleV = 50;

float colorStart = 0;     
float colorRange = .7;    //Range of each section of color 1 = 100%
float colorSpeed = .2;    //Speed of color cycling
int colorMax = 255;

String mode = "none"; // options 1: normal, 2: complementary, 3: triad

// The leds colors as WRGB
uint32_t leds[NUM_LEDS];

float x, y, zH, zS, zV;

uint8_t colorLoop = 1;

void setup()
{
  CircuitPlayground.begin();
  //CircuitPlayground.strip.show();
  CircuitPlayground.setBrightness(BRIGHTNESS);
  
  x = 0.0;
  y = 0.0;
  zH = 100;
  zS = 50;
  zV = 0;
  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    float thisAngle = angle * i;
    float xoffset = cos(thisAngle) * circleRadius;
    float yoffset = sin(thisAngle) * circleRadius;
    xOffsets[i] = xoffset;
    yOffsets[i] = yoffset;
  }
}

void fillnoise()
{

  for (int i = 0; i < NUM_LEDS; i++)
  {
    // signed on range from approx -70 to 70
    int8_t noiseValH = inoise8_raw(x + xOffsets[i] * scaleH, y + yOffsets[i] * scaleH, zH);

    // expand to 16 bit signed
    int16_t hue = (int16_t)(noiseValH * (256. * colorRange) + colorStart); // (int) (noiseValH * colorRange + colorStart);
    if (mode=="TRIAD"){
      if (noiseValH > colorMax * 5 / 8) {
        hue = hue + colorMax * 2 / 3 - colorMax * colorRange * 5 / 8;
      } else if (noiseValH > colorMax * 3 / 8) {
        hue = hue + colorMax / 3 - colorMax * colorRange * 3 / 8;
      }
    }
    if (mode=="COMPLEMENTARY") {
      if (noiseValH > colorMax / 2) {
        hue = hue + colorMax / 2 - colorMax * colorRange / 2;
      } 
    }
    //hue = hue % colorMax;

    // results scaled on 0-255
    uint8_t noiseValS = inoise8(x + xOffsets[i] * scaleS, y + yOffsets[i] * scaleS, zS);
    uint8_t noiseValV = inoise8(x + xOffsets[i] * scaleV, y + yOffsets[i] * scaleS, zV);

    uint8_t saturation = noiseValS/2 + 127; //constrain(noiseValS + 70, 0, 255);
    uint8_t value = 0xF8 & (noiseValV / 2 + 96); //1 << ((noiseValV/2 + 127) >> 5); //constrain(noiseValV - 20, 0, 255);
    
    leds[i] = CircuitPlayground.strip.gamma32(CircuitPlayground.strip.ColorHSV(hue, saturation, value));
    CircuitPlayground.strip.setPixelColor(i,leds[i]);
  }

  zH += speedH;
  zS += speedS;
  zV += speedV;

  // apply slow drift to X and Y, just for visual variation.
  x += speedS / 8;
  y -= speedS / 16;
  
  //colorStart += colorSpeed;
  //if (colorStart > colorMax) { colorStart -= colorMax; }

}

bool leftButtonClick()
{
  if (CircuitPlayground.leftButton()) {
    do {
      delay(20);
    } while (CircuitPlayground.leftButton());
    return true;
  }
  return false;
}


bool rightButtonClick()
{
  if (CircuitPlayground.rightButton()) {
    do {
      delay(20);
    } while (CircuitPlayground.rightButton());
    return true;
  }
  return false;
}


void loop()
{
  fillnoise();

  CircuitPlayground.strip.show();

  delay(20);

  if (rightButtonClick()) {
    colorStart += 4096;
  } else if (leftButtonClick()) {
    colorStart -= 4096;
  }
  if (colorStart < -32767) {
    colorStart += 65535;
  } else if (colorStart > 32767) {
    colorStart -= 65535;
  }
}
