#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(5, 4, NEO_RGB);
uint32_t colour[3] = { neopixel.Color(150, 0, 0), neopixel.Color(0, 150, 0), neopixel.Color(0, 0, 150) };
int ran[3] = { 0, 0, 0 };
int buttons[4] = { 23, 24, 26, 27 };
bool buttonOn[3][4] = {
  { false, false, false, false },
  { false, false, false, false },
  { false, false, false, false }
};
bool buttonState[4] = { LOW, LOW, LOW, LOW };
bool lastButtonState[4] = { LOW, LOW, LOW, LOW };
bool channelButtonState[2] = { LOW, LOW };
bool lastChannelButtonState[2]{ LOW, LOW };
int tempoPot = A17;
int channelButton[2] = { 39, 40 };
int midiNote[3] = { 36, 38, 42 };
int channelDisplayed = 0;
unsigned long lastStepTime = 0;
int step = 0;
int prevStep = 0;


void setup() {
  neopixel.begin();
  neopixel.clear();
  neopixel.show();
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT);
  }
  pinMode(channelButton[0], INPUT);
  pinMode(channelButton[1], INPUT);
}
void checkChannel() {
  for (int i = 0; i < 2; i++) {
    lastChannelButtonState[i] = channelButtonState[i];
    channelButtonState[i] = digitalRead(channelButton[i]);
  }
  if (channelButtonState[0] == HIGH && lastChannelButtonState[0] == LOW) {
    channelDisplayed--;
    if (channelDisplayed < 0) {
      channelDisplayed = 2;
    }
    Serial.println(channelDisplayed);
    delay(5);
  } else if (channelButtonState[0] == LOW && lastChannelButtonState[0] == HIGH) {
    delay(5);
  }
  if (channelButtonState[1] == HIGH && lastChannelButtonState[1] == LOW) {
    channelDisplayed++;
    if (channelDisplayed > 2) {
      channelDisplayed = 0;
    }
    Serial.println(channelDisplayed);
    delay(5);
  } else if (channelButtonState[1] == LOW && lastChannelButtonState[1] == HIGH) {
    delay(5);
  }
}

void checkButton() {
  for (int i = 0; i < 4; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttons[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      buttonOn[channelDisplayed][i] = !buttonOn[channelDisplayed][i];
      delay(5);
    } else if (buttonState[i] == LOW && lastButtonState[i] == HIGH) {
      delay(5);
    }
  }
}

void checkLeds() {
  if (step != prevStep) {
      
      for (int i = 0; i < 3; i++) {
        ran[i] = random(0, 100);
      }
      neopixel.setPixelColor(4, ran[0], ran[1], ran[2]);
      neopixel.show();
    }
  for (int i = 0; i < 4; i++) {

    
    
    if (buttonOn[channelDisplayed][i] == true) {
      neopixel.setPixelColor(i, colour[channelDisplayed]);
      neopixel.show();

    } else {
      neopixel.setPixelColor(i, 0, 0, 0);
      neopixel.show();
    }
  }
}
void sequence() {
  prevStep = step;
  int tempo = analogRead(tempoPot);
  if (millis() > lastStepTime + tempo) {



    for (int i = 0; i < 3; i++) {
      usbMIDI.sendNoteOff(midiNote[i], 0, 1);
    }
    
    step++;
    if (step > 3) {
      step = 0;
    }

    for (int i = 0; i < 3; i++) {
      if (buttonOn[i][step] == true) {
        usbMIDI.sendNoteOn(midiNote[i], 127, 1);
      }
      lastStepTime = millis();
    }
  }
}



void loop() {
  checkChannel();
  checkLeds();
  checkButton();
  sequence();
}
