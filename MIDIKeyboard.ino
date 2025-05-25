// Arduino MIDI Keyboard: 12 Notes + Octave Up/Down with SoftwareSerial MIDI OUT

#include <SoftwareSerial.h>
#include <MIDI.h>

// SoftwareSerial on pins 0 (RX unused) and 1 (TX) for MIDI OUT at 31250 baud
SoftwareSerial midiSerial(0, 1); // RX, TX
MIDI_CREATE_INSTANCE(SoftwareSerial, midiSerial, MIDI);

const int noteButtons[12] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,13};
const int octaveUpPin = A1;    // move octave controls off A0/A1
const int octaveDownPin = A0;

int octave = 4;
const int baseNote = 60; // MIDI note for Middle C (C4)
bool lastButtonStates[12];
bool lastOctaveUp = false;
bool lastOctaveDown = false;

void setup() {
  Serial.begin(9600);           // USB serial for debug
  midiSerial.begin(31250);      // MIDI DIN baud rate
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Initialize button pins
  for (int i = 0; i < 12; i++) {
    pinMode(noteButtons[i], INPUT_PULLUP);
    lastButtonStates[i] = digitalRead(noteButtons[i]);
  }
  pinMode(octaveUpPin, INPUT_PULLUP);
  pinMode(octaveDownPin, INPUT_PULLUP);
}

void loop() {
  // Handle note buttons
  for (int i = 0; i < 12; i++) {
    bool state = digitalRead(noteButtons[i]);
    if (state != lastButtonStates[i]) {
      int note = baseNote + (octave * 12 - 48) + i;
      if (!state) { // pressed
        MIDI.sendNoteOn(note, 127, 1);
        Serial.print("[OUT] Note On: "); Serial.println(note);
      } else {     // released
        MIDI.sendNoteOff(note, 0, 1);
        Serial.print("[OUT] Note Off: "); Serial.println(note);
      }
      lastButtonStates[i] = state;
    }
  }

  // Handle octave up/down
  bool up = digitalRead(octaveUpPin);
  if (!up && !lastOctaveUp) {
    if (octave < 8) octave++;
    Serial.print("[CTRL] Octave Up -> "); Serial.println(octave);
    delay(200);
  }
  lastOctaveUp = up;

  bool down = digitalRead(octaveDownPin);
  if (!down && !lastOctaveDown) {
    if (octave > 1) octave--;
    Serial.print("[CTRL] Octave Down -> "); Serial.println(octave);
    delay(200);
  }
  lastOctaveDown = down;
}
