#include <TweenDuino.h>

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// The pin to use to signal to other devices that stages are set
#define PIN_ATTRACT 10
#define PIN_STEP1 11
#define PIN_STEP2 12

bool touched[] = {false, false};
const size_t touchCount = sizeof(touched) / sizeof(touched[0]);

enum states {
  ATTRACT,
  STEP1,
  STEP2,
  DONE,
} state;

void setup() {
  Serial.begin(57600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_ATTRACT, OUTPUT);
  pinMode(PIN_STEP1, OUTPUT);
  pinMode(PIN_STEP2, OUTPUT);

  Serial.println("2024: Translation");

  digitalWrite(PIN_ATTRACT, LOW);
  digitalWrite(PIN_STEP1, LOW);
  digitalWrite(PIN_STEP2, LOW);

   if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
   MPR121.setInterruptPin(MPR121_INT);
}

unsigned long int startOf2 = 0;

void loop() {

  readTouchInputs(touched, touchCount);

  // React to touches depending on the state of the piece.
  if (state == ATTRACT) {
    if (touched[0]) {
      switchToStep1();
    }
  } else if (state == STEP1) {
    // TODO: Rapidly starting and stopping mp3 player can cause a crash, so some protection
    //       should go here.

    // Both touch points touched?
    if (touched[0] && touched[1]) {
      startOf2 = millis();
      switchToStep2();
    // Just the 1st touch touched?
    } else if (touched[0]) {
      // No change, stick to step 1.
    // No touches we care about, so go back to attract.
    } else {
      switchToAttract();
    }
  } else if (state == STEP2) {
    Serial.println("Running Active stage (step 2)");

    // Stay in the "stage 2" mode for 5 seconds, then revert to attract.
    auto time = millis();
    if (time - startOf2 > 5000) {
      Serial.println("Stage 2 done.");
      switchToDone();
    }
  } else if (state == DONE) {
    if (touched[0] && touched[1]) {
      switchToStep2();
    } else if (touched[0]) {
      switchToStep1();
    } else {
      switchToAttract();
    }

  } else {
    Serial.println("ERROR: Shouldn't be here.");
  }

  // Let the onboard LED communicate if touches are being detected.  As a treat.
  if (touched[0] || touched[1]) {
    digitalWrite(LED_BUILTIN, HIGH);  
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void switchToAttract() {
  Serial.println("Started attract.");
  state = ATTRACT;
  digitalWrite(PIN_ATTRACT, HIGH);
  digitalWrite(PIN_STEP1, LOW);
  digitalWrite(PIN_STEP2, LOW);
}

void switchToStep1() {
  Serial.println("Started step 1.");
  state = STEP1;
  digitalWrite(PIN_ATTRACT, LOW);
  digitalWrite(PIN_STEP1, HIGH);
  digitalWrite(PIN_STEP2, LOW);
}

void switchToStep2() {
  Serial.println("Started step 2.");
  state = STEP2;
  digitalWrite(PIN_ATTRACT, LOW);
  digitalWrite(PIN_STEP1, LOW);
  digitalWrite(PIN_STEP2, HIGH);
}

void switchToDone() {
  Serial.println("Started done stage.");
  state = DONE;
  digitalWrite(PIN_ATTRACT, LOW);
  digitalWrite(PIN_STEP1, LOW);
  digitalWrite(PIN_STEP2, LOW);
}

void readTouchInputs(bool touched[], size_t n){
  if(MPR121.touchStatusChanged()){
    
    MPR121.updateTouchData();

    // only make an action if we have one or fewer pins touched
    // ignore multiple touches

    // We only care about the first two touches.
    for (int i= 0; i < n; i++) {
      if (MPR121.isNewTouch(i)) {
        touched[i] = true;
        Serial.print("pin ");
        Serial.print(i);
        Serial.println(" was just touched");
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
          if(MPR121.isNewRelease(i)){
            touched[i] = false;
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" is no longer being touched");
          } 
        }
    }
    
    if(MPR121.getNumTouches()<=1){
      for (int i=0; i < 12; i++){  // Check which electrodes were pressed
        if(MPR121.isNewTouch(i)){        
            //pin i was just touched

        
            /*if(MP3player.isPlaying()){
              if(lastPlayed==i){
                // if we're already playing from the requested folder, stop it
                MP3player.stopTrack();
                Serial.println("stopping track");
              } else {
                // if we're already playing a different track, stop that 
                // one and play the newly requested one
                MP3player.stopTrack();
                Serial.println("stopping track");
                playRandomTrack(i);
                
                // don't forget to update lastPlayed - without it we don't
                // have a history
                lastPlayed = i;
              }
            } else {
              // if we're playing nothing, play the requested track 
              // and update lastplayed
              playRandomTrack(i);
              lastPlayed = i;
            }  */    
        }
      }
    }
  }
}
