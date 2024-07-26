/*
The Playel
Turning the Lego Grand Piano into a fully MIDI-compliant keyboard.
Version 1.1, release 26.7.2024
Features:
- all 25 keys (pin 0-24)
- sustain pedal (pin 25)
- fader (pin A0) - Note: this is not covered in the instructions yet!
- uses millis() instead of delay() to avoid bouncing when keys are pressed
- implements an option for the pedal to use it for transposing the 2 octaves (currengtly 12 tones - one octaves up)
- extended code commenting
https://www.drhoedl.com/permalink/theplayel
https://github.com/oliverhoedl/theplayel

by Oliver Hödl http://www.drhoedl.com
Other code sources:
https://docs.arduino.cc/built-in-examples/communication/Midi/ (by Tom Igoe)
https://docs.arduino.cc/tutorials/generic/midi-device/ (by Arturo Guadalupi)
*/

int keyState[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // state of 25 keys + 1 damper
int keyLastState[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // last state of the 25 keys for state change detection + 1 damper
int noteValue[] = {72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,64}; // MIDI pitch 25 tones (2 octaves from C to c"); 440 Hz is represented in MIDI terms by the integer 69, which is a'
const int fader = 0; // Analog pin A0 where the fader is connected to
uint8_t intensity; // input value of the fader
int transpose = 0; // transpose value of key pressed
unsigned long currentMillis = 0;
unsigned long previousMillis[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // will store last time a key was pressed

void setup() {
  for (int thisPin = 2; thisPin < 28; thisPin++) {
    pinMode(thisPin, INPUT_PULLUP);
  }
  // Set Serial Monitor baud rate (for debugging):
  //Serial.begin(9600);
  // Set MIDI baud rate:
  Serial.begin(31250);
} // setup() end


void loop() {
  currentMillis = millis();

  readFaderIntensity(); // read fader input
  
  for (int thisPin = 2; thisPin < 28; thisPin++) {
    keyState[thisPin-2] = digitalRead(thisPin);
  }


  for (int thisKey = 0; thisKey < 26; thisKey++) { // compare the keyState to its previous state
    if (keyState[thisKey] != keyLastState[thisKey]) { // if the state has changed MIDI noteon or noteoff
      if (currentMillis - previousMillis[thisKey] >= 30) { // avoid bouncing
        previousMillis[thisKey] = currentMillis;
        
        if (keyState[thisKey] == LOW) { // Key was pressed (if the current state is LOW (means pressed because circuit is closed) then key was pressed (noteon went from OFF to ON))
          if (thisKey == 25) { // if the pedal was pressed
            //noteOn(0xB0, noteValue[thisKey], 0x45); // send a sustain control command (only for sustain pedal); uncomment if pedal is used for something else
            transpose = 12; // pedal transposes pressed key 12 half tones up
          } else { // if a key was pressed
            noteOn(0x90, noteValue[thisKey]+transpose, 0x45); // send a note command (applies to the 25 keys)
          }
          //Serial.println(String(noteValue[thisKey]) + " on"); // for debugging
        } else { // key released (if the current state is HIGH (or other than LOW) then the key went from on to off)
          if (thisKey == 25) { // if the pedal was released
            //noteOn(0xB0, noteValue[thisKey], 0x00); // send a sustain control command (only for sustain pedal); uncomment if pedal is used for something else
            transpose = 0; // resets transposition value when pedal is released
          } else { // if a key was released
            noteOn(0x80, noteValue[thisKey]+transpose, 0x00); // send a note command (applies to the 25 keys)
          }
          //Serial.println(String(noteValue[thisKey]) + " off"); // for debugging
        }

      }
      // Delay a little bit to avoid bouncing
      //delay(30);
    }
    // save the current state as the last state, for next time through the loop
    keyLastState[thisKey] = keyState[thisKey];
  } // for thisKey end
} // loop() end


// Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

void readFaderIntensity() {
  int val = analogRead(fader);
  intensity = (uint8_t) (map(val, 0, 1023, 0, 127)); // maps fader input values to MIDI values 0-127
  noteOn(0xB0, 65, intensity); // send a control command: 0xB0 = control change, 65 = custom command value for fader
}
