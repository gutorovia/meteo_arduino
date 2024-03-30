#define BUTTON_DPIN 5

void buttonSetup() {
  pinMode(BUTTON_DPIN, INPUT_PULLUP);
}

int getButtonState() {
  return digitalRead(BUTTON_DPIN);
}