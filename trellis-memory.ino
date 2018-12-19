#include "Adafruit_NeoTrellisM4.h"
#include <Audio.h>

#include "AudioSampleBubblegum.h"
#include "AudioSampleCandyfloss.h"
#include "AudioSampleCarrot.h"
#include "AudioSampleDim.h"
#include "AudioSampleDolphin.h"
#include "AudioSampleFire.h"
#include "AudioSampleForest.h"
#include "AudioSampleGrammapink.h"
#include "AudioSampleGrape.h"
#include "AudioSampleIce.h"
#include "AudioSampleLimejello.h"
#include "AudioSampleLollipop.h"
#include "AudioSampleOcean.h"
#include "AudioSampleSnow.h"
#include "AudioSampleSunshine.h"
#include "AudioSampleVanillaicecream.h"

AudioPlayMemory sound0;
AudioOutputAnalogStereo audioOutput;

AudioConnection c1(sound0, 0, audioOutput, 0);
AudioConnection c2(sound0, 0, audioOutput, 1);

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

int pairs[32] = {
  0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15
};

int colors[16] = {
  trellis.Color(255,0,0),
  trellis.Color(40,255,0),
  trellis.Color(255,255,40),
  trellis.Color(0,255,40),
  trellis.Color(0,0,255),
  trellis.Color(255,40,40),
  trellis.Color(255,255,255),
  trellis.Color(40,40,40),
  trellis.Color(255,255,0),
  trellis.Color(255,0,255),
  trellis.Color(0,255,255),
  trellis.Color(40,0,255),
  trellis.Color(40,0,40),
  trellis.Color(0,40,40),
  trellis.Color(255,40,0),
  trellis.Color(255,0,40)
};

const unsigned int* sounds[16] = {
  AudioSampleFire,
  AudioSampleLimejello,
  AudioSampleVanillaicecream,
  AudioSampleForest,
  AudioSampleOcean,
  AudioSampleLollipop,
  AudioSampleSnow,
  AudioSampleDim,
  AudioSampleSunshine,
  AudioSampleCandyfloss,
  AudioSampleDolphin,
  AudioSampleGrape,
  AudioSampleGrammapink,
  AudioSampleIce,
  AudioSampleCarrot,
  AudioSampleBubblegum
};

int firstKey = -1;
boolean *litKeys;
int litKeyCount = 0;

int p1score = 0;
int p2score = 0;
boolean turn = true;

void setup() {
  Serial.begin(115200);

  trellis.begin();
  trellis.setBrightness(100);

  AudioMemory(10);

  Serial.println("Init");

  gameStart();

  // show red/blue start up
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      trellis.setPixelColor(j * 8 + i, trellis.Color(255,0,0));
      trellis.setPixelColor(j * 8 + 7 - i, trellis.Color(0,0,255));
    }
    delay(250);
  }
  delay(750);
  for (int i = 3; i > -1; i--) {
    for (int j = 0; j < 4; j++) {
      trellis.setPixelColor(j * 8 + i, trellis.Color(0,0,0));
      trellis.setPixelColor(j * 8 + 7 - i, trellis.Color(0,0,0));
    }
    delay(250);
  }
}

void gameStart() {
  firstKey = -1;
  litKeys = new boolean[trellis.num_keys()];
  for (int i=0; i<trellis.num_keys(); i++) {
    litKeys[i] = false;
  }
  litKeyCount = 0;

  p1score = 0;
  p2score = 0;

  // make it player 1's turn
  turn = true;
  
  randomSeed(analogRead(A0) * analogRead(A1) * analogRead(A2) * analogRead(A3));
  
  for (int i = 0; i < 32 - 1; i++) {
    int j = random(0, 32 - i);
    int t = pairs[i];
    pairs[i] = pairs[j];
    pairs[j] = t;
  }
}

void loop() {
  trellis.tick();

  while (trellis.available()) {
    keypadEvent e = trellis.read();

    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      int key = e.bit.KEY;
//      Serial.println(key);
      int pair = pairs[key];

      sound0.play(sounds[pair]);

      // Make sure the key hasn't been paired and already lit
      if (litKeys[key] == false) {
        // turn on the key with the assigned colour
        trellis.setPixelColor(key, colors[pair]);
  
        if (firstKey == -1 || key == firstKey) {
          firstKey = key;
        } else {
          if (pairs[key] == pairs[firstKey]) {
            // it's a match!
            litKeys[key] = true;
            litKeys[firstKey] = true;
            delay(500);
            trellis.setPixelColor(key, trellis.Color(0, 0, 0));
            trellis.setPixelColor(firstKey, trellis.Color(0, 0, 0));
            delay(250);
            trellis.setPixelColor(key, colors[pair]);
            trellis.setPixelColor(firstKey, colors[pair]);
            delay(250);
            trellis.setPixelColor(key, trellis.Color(0, 0, 0));
            trellis.setPixelColor(firstKey, trellis.Color(0, 0, 0));
            delay(250);
            trellis.setPixelColor(key, colors[pair]);
            trellis.setPixelColor(firstKey, colors[pair]);

            // add point
            if (turn) {
              p1score += 1;
            } else {
              p2score += 1;
            }
            Serial.print("1: ");
            Serial.println(p1score);
            Serial.print("2: ");
            Serial.println(p2score);

            litKeyCount += 2;
            if (litKeyCount == 32) {
              // game over!
              if (p1score == p2score) {
                // tie!
                // make red and blue meet in the middle
                for (int i = 0; i < 4; i++) {
                  for (int j = 0; j < 4; j++) {
                    trellis.setPixelColor(j * 8 + i, trellis.Color(255,0,0));
                    trellis.setPixelColor(j * 8 + 7 - i, trellis.Color(0,0,255));
                  }
                  delay(250);
                }
                delay(750);
                for (int i = 3; i > -1; i--) {
                  for (int j = 0; j < 4; j++) {
                    trellis.setPixelColor(j * 8 + i, trellis.Color(0,0,0));
                    trellis.setPixelColor(j * 8 + 7 - i, trellis.Color(0,0,0));
                  }
                  delay(250);
                }
              } else {
                int winnerColor;
                if (p1score > p2score) {
                  // p1 wins
                  // flood with winning color
                  for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 4; j++) {
                      trellis.setPixelColor(j * 8 + i, trellis.Color(255,0,0));
                    }
                    delay(250);
                  }
                  delay(750);
                  for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 4; j++) {
                      trellis.setPixelColor(j * 8 + i, trellis.Color(0,0,0));
                    }
                    delay(250);
                  }
                } else {
                  // p2 wins
                  // flood with winning color
                  for (int i = 7; i > -1; i--) {
                    for (int j = 0; j < 4; j++) {
                      trellis.setPixelColor(j * 8 + i, trellis.Color(0,0,255));
                    }
                    delay(250);
                  }
                  delay(750);
                  for (int i = 7; i > -1; i--) {
                    for (int j = 0; j < 4; j++) {
                      trellis.setPixelColor(j * 8 + i, trellis.Color(0,0,0));
                    }
                    delay(250);
                  }
                }
                
              }
              gameStart();
            }
            
          } else {
            // not a match :(
            delay(1000);
            trellis.setPixelColor(key, trellis.Color(0, 0, 0));
            trellis.setPixelColor(firstKey, trellis.Color(0, 0, 0));
            
            // switch players
            turn = !turn;
            if (turn) {
              Serial.println("P1 Go"); 
            } else {
              Serial.println("P2 Go"); 
            }
          }
          firstKey = -1;
        }
      }
    }
  }
}
