// MQ135 A0 -> A0

#define R_ZERO_VALUES_ACCUM_COUNT 10
float calibrationAccum[R_ZERO_VALUES_ACCUM_COUNT];
int calibrationAccumIndex = 0;
#define CALIBRATION_THRESHOLD 0.04

int MQ135_APIN = A0;

float MQ135_RZERO = 3.6;
float MQ135_RNAGRUZ = 0.992;

boolean isDht11Initialized = false;

#define MQ135_DELAY_CALIBRATING_SEC 2
#define MQ135_DELAY_OPERATING_SEC 10

#define CO2_THRESHOLD_BEEP 500

MQ135 mq135 = MQ135(MQ135_APIN, MQ135_RZERO, MQ135_RNAGRUZ);

float mq135RZeroValue = 0;
byte mq135RZeroEqualValuesCounter = 0;
boolean mq135Ready = false;
unsigned long previousMillis = 0;

void mq135Setup() {
  switchLightRed(true);
}

void mq135Processing() {
  float temperature = getTemperature();
  float humidity = getHumidity();

  if (!isDht11Initialized && temperature != 0.0 && humidity != 0.0) {
    isDht11Initialized = true;
  }

  float correctedRZero;
  float correctedPPM;

  correctedRZero = mq135.getCorrectedRZero(temperature, humidity);
  correctedPPM = mq135.getCorrectedPPM(temperature, humidity);

  int buttonState = getButtonState();
  if (buttonState == LOW) {
    if (mq135Ready) {
      mq135Ready = false;
      switchLightRed(true);
      mq135RZeroEqualValuesCounter = 0;
    } else {
      mq135Ready = true;
      switchLightRed(false);

      mq135 = MQ135(MQ135_APIN, correctedRZero, MQ135_RNAGRUZ);
    }

    zoomerBeep(1000, 300);
  }

  unsigned long currentMillis = millis();
  unsigned long interval;
  if (!mq135Ready) {
    interval = MQ135_DELAY_CALIBRATING_SEC * 1000;
  } else {
    interval = MQ135_DELAY_OPERATING_SEC * 1000;
  }
  if (currentMillis - previousMillis < interval) {
    return;
  }
  previousMillis = currentMillis;

  if (isDht11Initialized) {
    word co2;
    if (mq135Ready) {
      co2 = (word)correctedPPM;
      if (co2 > CO2_THRESHOLD_BEEP) {
        zoomerBeep(1500, 500);
      }
    } else {
      co2 = 0xffff;
    }
    word currentSensorValues[] = {
      (word)temperature,
      (word)humidity,
      co2
    };
    lcdLoop(currentSensorValues);
  }

<<<<<<< Updated upstream
  // Serial.print(correctedPPM);
  // Serial.print(" ");
  // Serial.println(correctedPPM);

  if (!mq135Ready) {c:\Users\Илья\Desktop\Arduino\meteoV4\meteoV4.ino
=======
  if (!mq135Ready) {
>>>>>>> Stashed changes
    if (calibrationAccumIndex == R_ZERO_VALUES_ACCUM_COUNT) {
      for (byte i = 0; i < R_ZERO_VALUES_ACCUM_COUNT - 1; i++) {
        calibrationAccum[i] = calibrationAccum[i + 1];
      }
      calibrationAccum[calibrationAccumIndex - 1] = correctedRZero;
      // Serial.print(calibrationAccumIndex);
      // Serial.print(" ");
      // Serial.print(correctedRZero);
      // Serial.println(" still filling accum");

      float min = calibrationAccum[0];
      float max = calibrationAccum[0];
      for (byte i = 1; i < R_ZERO_VALUES_ACCUM_COUNT; i++) {
        if (calibrationAccum[i] < min) min = calibrationAccum[i];
        if (calibrationAccum[i] > max) max = calibrationAccum[i];
      }
      if (max - min <= CALIBRATION_THRESHOLD) {
        mq135Ready = true;
        // Serial.print("Done: ");
        // Serial.print(min);
        // Serial.print("...");
        // Serial.print(max);
        // Serial.print(": ");
        // Serial.println(max - min);
        mq135 = MQ135(MQ135_APIN, correctedRZero, MQ135_RNAGRUZ);
        switchLightRed(false);
        zoomerBeep(1000, 300);
      } else {
        // Serial.print("Still calibrating: ");
        // Serial.print(min);
        // Serial.print("...");
        // Serial.print(max);
        // Serial.print(": ");
        // Serial.println(max - min);
        brightnessLightRed((max - min) * 1000);
        // zoomerBeep(1000 + mq135RZeroEqualValuesCounter * 50, 20);
      }
    } else {
      calibrationAccum[calibrationAccumIndex] = correctedRZero;
      // Serial.print(calibrationAccumIndex);
      // Serial.print(" ");
      // Serial.print(correctedRZero);
      // Serial.println(" still filling accum");
      calibrationAccumIndex++;
    }
  }

#ifdef DHT11_DEBUG
    Serial.print(humidity);
    Serial.print(" ");
    Serial.println(temperature);
#endif

#ifdef MQ135_DEBUG
    Serial.print("MQ135-1");
    // Serial.print("\tRZero: ");
    // Serial.print(rzero[0]);
    Serial.print("\t Corrected RZero: ");
    Serial.print(correctedRZero);
    // Serial.print("\t Resistance: ");
    // Serial.print(resistance[0]);
    // Serial.print("\t PPM: ");
    // Serial.print(ppm[0]);
    Serial.print("ppm");
    Serial.print("\t Corrected PPM: ");
    Serial.print(correctedPPM);
    Serial.println("ppm");
#endif
}
