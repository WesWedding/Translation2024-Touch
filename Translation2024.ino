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

// The pin to use to signal to other devices that stages are set
#define PIN_ATTRACT 10
#define PIN_STEP1 11
#define PIN_STEP2 12

// I2C Addresses
#define I2C_LIGHT_ADDR 0x2C
#define I2C_TOUCH_ADDR 0x8C

bool touched[] = {false, false};
const size_t touchCount = sizeof(touched) / sizeof(touched[0]);

enum states {
  ATTRACT,
  PENDING,
  FULL_ACTIVE,
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
  Serial.println("Brgin");
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
    Serial.println("Running Active stage (step 2)");

    // Stay in the "stage 2" mode until MP3 player is done.
    if (!MP3player.isPlaying()) {
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

void updateState(states newState) {
  state = newState;
}

void switchToAttract() {
  Serial.println("Started attract.");
  updateState(ATTRACT);
  playSound(ATTRACT);
}

void switchToStep1() {
  Serial.println("Started step 1.");
  updateState(PENDING);
  playSound(PENDING);
}

void switchToStep2() {
  Serial.println("Started step 2.");
  updateState(FULL_ACTIVE);
  playSound(FULL_ACTIVE);
}

void switchToDone() {
  Serial.println("Started done stage.");
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
  Serial.println("reqyest");
  char buffer[2];
  itoa(state, buffer, 10);
  Wire.write(buffer);
}