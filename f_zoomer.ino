#define ZOOMER_DPIN 7

void zoomerSetup() {
  pinMode(ZOOMER_DPIN, OUTPUT);
}

void zoomerBeep(word freq, word delayMs) {
  tone(ZOOMER_DPIN, freq, delayMs);
}
