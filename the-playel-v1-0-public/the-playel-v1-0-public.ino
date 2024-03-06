/*
The Playel
Turning the Lego Grand Piano into a fully MIDI-compliant keyboard.
Version 1.0
https://www.drhoedl.com/permalink/theplayel
https://github.com/oliverhoedl/theplayel

by Oliver Hödl http://www.drhoedl.com
This code is based on http://www.arduino.cc/en/Tutorial/Midi
*/

// state of 25 keys
int keyState[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// last state of the 25 keys for state change detection
int keyLastState[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// MIDI pitch 25 tones (2 octaves from C to c")
// 440 Hz is represented in MIDI terms by the integer 69, which is a'
int noteValue[] = {72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48};

void setup() {

  for (int thisPin = 2; thisPin < 27; thisPin++) {
    pinMode(thisPin, INPUT_PULLUP);
  }
  // Set Serial Monitor baud rate (for debugging):
  //Serial.begin(9600);
  // Set MIDI baud rate:
  Serial.begin(31250);

} // setup() end


void loop()
{
  for (int thisPin = 2; thisPin < 27; thisPin++) {
    keyState[thisPin-2] = digitalRead(thisPin);
  }

  for (int thisKey = 0; thisKey < 25; thisKey++) {
    // compare the keyState to its previous state
    if (keyState[thisKey] != keyLastState[thisKey]) {
      // if the state has changed MIDI noteon or noteoff
      if (keyState[thisKey] == LOW) {
        // if the current state is LOW (means pressed because circuit is closed)
        // then noteon went from off to on:
        noteOn(0x90, noteValue[thisKey], 0x45);
        //Serial.println(String(noteValue[thisKey]) + " on");
      } else {
        // if the current state is HIGH then the key went from on to off:
        noteOn(0x90, noteValue[thisKey], 0x00);
        //Serial.println(String(noteValue[thisKey]) + " off");
      }
      // Delay a little bit to avoid bouncing
      delay(150);
    }
    // save the current state as the last state, for next time through the loop
    keyLastState[thisKey] = keyState[thisKey];
  } // for thisKey end
  
} // loop() end


// Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
