#include <TweenDuino.h>

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// mp3 includes
#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h> 
#include <SFEMP3Shield.h>

// mp3 variables
SFEMP3Shield MP3player;
byte result;
int lastPlayed = 0;

// sd card instantiation
SdFat sd;
SdFile file;

// I2C Addresses
#define I2C_LIGHT_ADDR 0x2C
#define I2C_TOUCH_ADDR 0x8C

bool touched[] = {false, false};
const size_t touchCount = sizeof(touched) / sizeof(touched[0]);

#include "StateQueue.h"
StateQueue pendingReports;
states state;

void setup() {
  Serial.begin(57600);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("2024: Translation");

  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  result = MP3player.begin();
  MP3player.setVolume(10,10);

  if(result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to start MP3 player");
  }

  Wire.begin(I2C_TOUCH_ADDR);
  Wire.onRequest(requestEvent);
}

void loop() {

  readTouchInputs(touched, touchCount);

  // React to touches depending on the state of the piece.
  if (state == ATTRACT) {
    if (touched[0]) {
      switchToStep1();
    }
  } else if (state == PENDING) {
    // TODO: Rapidly starting and stopping mp3 player can cause a crash, so some protection
    //       should go here.

    // Keep that MP3 looping.
    if (!MP3player.isPlaying()) {
      playSound(PENDING);
    }

    // Both touch points touched?
    // See also: DONE state; this check is duplicated in both.
    if (touched[0] && touched[1]) {
      switchToStep2();
    // Just the 1st touch touched?
    } else if (touched[0]) {
      // No change, stick to step 1.
    // No touches we care about, so go back to attract.
    } else {
      switchToAttract();
    }
  } else if (state == FULL_ACTIVE) {
    // We like the touches, but maybe the MP3 player finished.
    if (!MP3player.isPlaying()) {
      switchToDone();
    }
  } else if (state == DONE) {

    // Both touch points touched?
    // See also: ATTRACT state; this check is duplicated in both.
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

void updateState(states newState) {
  pendingReports.enqueue(newState);
  state = newState;
}

void switchToAttract() {
  updateState(ATTRACT);
  playSound(ATTRACT);
}

void switchToStep1() {
  updateState(PENDING);
  playSound(PENDING);
}

void switchToStep2() {
  updateState(FULL_ACTIVE);
  playSound(FULL_ACTIVE);
}

void switchToDone() {
  updateState(DONE);
  playSound(DONE);
}

void readTouchInputs(bool touched[], size_t n){

  if (!MPR121.touchStatusChanged()) {
    return;
  }

  MPR121.updateTouchData();

  // We only care about the first two touches in this application.
  for (size_t i= 0; i < n; i++) {
    if (MPR121.isNewTouch(i)) {
      touched[i] = true;
      // Update onboard LED to reflect a recognized touch.
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      if (MPR121.isNewRelease(i)) {
        touched[i] = false;
      }
    }
  }
}

void playSound(states state) {
  if (MP3player.isPlaying()) {
    MP3player.stopTrack();
  }
  char fileName[255]; // 255 is the longest possible file name size
  switch(state) {
    case PENDING:
      strcpy(fileName, "step1.mp3" + '\0');
      break;
    case FULL_ACTIVE:
      strcpy(fileName, "step2.mp3" + '\0');
      break;
    default:
      // Don't make a peep!  Bail out early.
      return;
      break;
  }

  MP3player.playMP3(fileName);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  char buffer[2];

  // Report any pending state changes first, before reporting current state.
  states report = state;
  if (pendingReports.isEmpty()) {
    itoa(report, buffer, 10);     
  } else {
    pendingReports.dequeue(report);
    itoa(report, buffer, 10);
    Serial.print("reporting queued state: ");
    Serial.println(buffer);
  }
  Wire.write(buffer);
}