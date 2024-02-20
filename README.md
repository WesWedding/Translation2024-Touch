# Untitled Translation Submission
2024 NWACC SACF Entry

This is the code that drives the primary board in Tabitha Darrah and Weston Wedding's 2024 submission to the NWACC Spring Arts and Culture Festival.  This year's theme is Translation!

### Notes

##### Arduino 2.0 Works with Bare Conductive
Despite Bare Conductive's website asserting otherwise, its driver package does work with Arduino 2.0 IDEs.  The instructions are just a little more complicated because the sketch folder might be redefined.  Use all of the same folders the instructions mention, just put them where the Sketch folder has been defined in preferences.


#### MP3 Player and Touch Uses a ton of space
Just using some of the core features of the board gobble up most of the free space for program storage AND dynamic memory.  Anything else that might use up a lot of additional space (like NeoPixel strands) might be best put onto another board.






