# Untitled Translation Submission
2024 NWACC SACF Entry

This is the code that drives the primary board in Tabitha Darrah and Weston Wedding's 2024 submission to the NWACC Spring Arts and Culture Festival.  This year's theme is Translation!

### Notes

##### Arduino 2.0 Works with Bare Conductive
Despite Bare Conductive's website asserting otherwise, its driver package does work with Arduino 2.0 IDEs.  The instructions are just a little more complicated because the sketch folder might be redefined.  Use all of the same folders the instructions mention, just put them where the Sketch folder has been defined in preferences.


##### MP3 Player and Touch Uses a ton of space
Just using some of the core features of the board gobble up most of the free space for program storage AND dynamic memory.  Anything else that might use up a lot of additional space (like NeoPixel strands) might be best put onto another board.


##### TweenDuino space measurements

Initial sketch
```cpp  
float value = 30.0f;

void setup() {
  value = 39.0f + value; // Avoid optimizing away value.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  int time = millis();
  time += 10;  // Avoid optimizing away time.
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                   // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);
}
```

Initial sketch: 4946 prog, 154 dynamic

Tween
`Tween tween(value, 302UL, 300.2f);` (also Adding `tween.update(time)`s to loop().)
1st tween: 9964 prog (+ 5018b) , 298 (+ 64b) dynamic
`Tween tween2(value, 30UL, 800.34f);`
2nd tween: 10036 prog (+ 72b), 348 (+50b) dynamic
`Tween tween3(value, 3402UL, 2.2f);`
3rd tween: 10088 prog (+ 52b), 398 (+50b) dynamic
`Tween tween4(value, 232UL, 20.3f);`
4th tween: 10138 prog (+ 50b), 448 (+50b) dynamic
`Tween tween5(value, 2382UL, 256324.3f);`
5th tween: 10190 prog (+ 52b), 498 (+50b) dynamic

**Base Tween logic/code footprint**:  ~4966b prog, 14 dynamic
**Per Tween memory footprint (default ease)**:  ~52b prog, 50b dynamic

Timeline (just empty, no tweens)
`Timeline tl;` plus `tl.update()`

1st timeline: 10236 prog (+ 5290 b) , 326 (+ 172b) dynamic
2nd timeline: 10296 prog (+ 60 b) , 404 (+ 78b) dynamic
3rd timeline: 10338 prog (+ 42 b) , 482 (+ 78b) dynamic
4th timeline: 10366 prog (+ 28 b) , 560 (+ 78b) dynamic
5th timeline: 10394 prog (+ 28 b), 638 (+ 78b) dynamic

Timeline (1 tween, default ease)
`t1.addTo(value, 100392.4f, 1093UL);`
1st timeline: 10842 prog (+ 5896b) , 326 (+ 172b) dynamic
`t2.addTo(value, 102.7f, 93UL);`
2nd timeline: 10932 prog (+ 74b) , 404 (+ 78b) dynamic
`t3.addTo(value, 1232.7f, 762UL);`
3rd timeline: 11006 prog (+ 74b) , 482 (+ 78b) dynamic
`t4.addTo(value, 99999.1f, 85734UL);`
4th timeline: 11066 prog (+ 60b) , 560 (+ 78b) dynamic
`t5.addTo(value, 11112.8f, 12UL);`
5th timeline: 11124 prog (+58b) , 638 (+ 78b) dynamic

Per timeline: ~74b prog, 78b dynamic


Timeline (5 tweens, default ease)
Empty timeline: 10236 prog (+ 5290b) , 326 (+ 172b) dynamic
`t1.addTo(value, 100392.4f, 1093UL);`
1st tween: 10842 prog (+ 606b) , 326 (+ 0b) dynamic
`t1.addTo(value, 102.7f, 93UL);`
2nd tween: 10872 prog (+ 30b) , 326 (+ 0b) dynamic
` t1.addTo(value, 1232.7f, 762UL);`
3rd tween: 10904 prog (+ 32b) , 326 (+ 0b) dynamic
`t1.addTo(value, 99999.1f, 85734UL);`
4th tween: 10936 prog (+ 32b) , 326 (+ 0b) dynamic
`t1.addTo(value, 11112.8f, 12UL);`
5th tween: 10966 prog (+ 30b) , 326 (+ 0b) dynamic


##### LED cost

Each Neopixel costs about 3 bytes of RAM, dynamically allocated during runtime.

The real overhead is just including the library and utilizing library functions; basic demo sketch size is 7806 bytes.

