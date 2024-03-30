#define LIGHT_RED_DPIN 9
#define LIGHT_GREEN_DPIN 4

void lightSetup() {
  pinMode(LIGHT_RED_DPIN, OUTPUT);
  pinMode(LIGHT_GREEN_DPIN, OUTPUT);

  toggleLightRed(false);
  toggleLightGreen(false);
}

void toggleLightRed(boolean on) {
  digitalWrite(LIGHT_RED_DPIN, on);
}

void toggleLightGreen(boolean on) {
  digitalWrite(LIGHT_GREEN_DPIN, on);
}

void switchLightRed(boolean on) {
  toggleLightRed(on);
  toggleLightGreen(!on);
}

void brightnessLightRed(byte brightness) {
  analogWrite(LIGHT_RED_DPIN, brightness);
}