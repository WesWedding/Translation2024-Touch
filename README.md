# Sample no. 43 (Touch + Audio Driver)
2024 NWACC SACF Entry w/ Tabitha Darrah

This is the code that drives the Touch and MP3 board in Tabitha Darrah and Weston Wedding's 2024 submission to the NWACC Spring Arts and Culture Festival.  This year's theme is Translation!

This code can be (with some modification) run on any Arduino-like hardware but this project, as is, has been written with some underlying assumptions about Hardware and Software (detailed later).

This code will run fine on its own, but half of the show is a visual component managed by the sister project [Sample no. 43 - Lights](https://github.com/WesWedding/Translation2024-Lights), which controls a strand of NeoPixel LEDs embedded inside a ceramic artwork.  There are also MP3 files that have not been included 

## Summary of Behavior
The driver watches for touches on two specific pads, E0 and E1 which are detected by checking the state reported by the MPR121 on the board.

Initially the driver is in an attract/waiting state, doing nothing except reporting its state to a I2C parent that asks for it.

- When E0 is touched, "step1.mp3" begins to play (and loop) on the MP3 player.  Releasing E0 will immediately stop the MP3, and E1 touches will be completely ignored while E0 is not touched.

- When E1 is touched at the same time as E0, the fully-activated state begins and a longer "step2.mp3" begins to play from start to finish.

- The driver resets to a "Done" state before transitioning back to either the waiting/"step 1"/"step 2" according to the rules outlined previously.

State changes are accumulated in a queue, which is consumed 1 state at a time via I2C requests.  This allows the Lights board to be aware all state changes and reset itself accordingly, even if they only exist extremely briefly between I2C requests from the Lights board.


## Usage
Connect some wires, conductive thread, or other conductive material to the board's "0" sensor input.  Likewise wire up to the board's "1" sensor input.  These are marked E0 and E1 respectively.

Touching the 1st input will play a sound file expected on the SD Card named "step1.mp3".

Touching the 1st and 2nd input at the same time will play a sound file expected to be on the SD card named "step2.mp3".

Due to licensing restrictions, neither MP3 file is provided with the source code.

## Expected Hardware
### Bare Conductive Touch Board
Bare Conductive's [Touch Board](https://www.bareconductive.com/collections/touch-board) was the hardware of choice for this project; far more reliable than any bespoke capacitive touch solution used in previous projects.  The onboard MPR121 provides capacitive touch, the VS1053b provides the MP3 player.

This board is not available via the Arduino Board Manager, so requires some extra steps.  The installers provided by Bare Conductive have not been updated to work with Arduino 2.x, so you'll probably need to get the "Arduino IDE Installer Manual" zip file from [the Bare Conductive Product Resources](https://www.bareconductive.com/pages/product-resources) page and follow the instructions contained therein.

### SD Card
Any old SD card with "step1.mp3" and "step2.mp3" files on it.


## Expected Software

### SDFat Library

This library is included in the installer provided by Bare Conductive, and required to access the SD card.

I updated it via the Arduino Package Manager to the latest version and it still worked;  the provided version was at least 1 major version out of date.

### Wire Library

This is a dependency of the MPR121 Library, but is also used in I2C communication with the Light board.

### SFEMP3Shield Library

This library is provided by Bare Conductive and is required for MP3 playback.

### MPR121 Library

This library is provided by Bare Conductive and is required for touch detection.

## Credits

This driver is written by Weston Wedding.

## License
This driver is free software; you can redistribute it and/or modify it under the terms of the the MIT License.  For full details, check out the [COPYING](COPYING.md) file in this sourcecode or the explanation of [MIT License here](https://tlo.mit.edu/understand-ip/exploring-mit-open-source-license-comprehensive-guide).