// SSD1306 SCL -> A5
// SSD1306 SDA -> A4

#include <GyverOLED.h>

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

#define MAX_HISTORY_SIZE 45

/*
  элемент [0] = температура (t)
  элемент [1] = влажность (h)
  элемент [2] = CO2 (c)
*/
#define SENSORS 3

char *chartNames[] = {"t", "h", "c"};
char *chartText[] = {"C", "%", "ppm"};

#define SENSOR_CO2 (SENSORS - 1) // сенсор CO2 всегда последний
byte history[SENSORS-1][MAX_HISTORY_SIZE];
word historyCo2[1][MAX_HISTORY_SIZE];
word historyRange[SENSORS][2] = {{20, 30}, {30, 70}, {300, 500}}; // границы графика по y, чтобы он не вылезал за пределы
byte currentHistoryIndex[SENSORS];
#define RANGE_MIN 0
#define RANGE_MAX 1

#define DISPLAY_SIZE_X 128
#define DISPLAY_SIZE_Y 64

#define Y_AXIS_POSITION_X 8
#define SHIFT_GRAPHICS_FROM_Y_AXIS_X 10
#define TEXT_POSITION_IN_CHART_Y 8
#define TEXT_POSITION_IN_CHART_X 95

#define CHARTS_SIZE_Y (DISPLAY_SIZE_Y / SENSORS)

void lcdSetup() {
  oled.init();        // инициализация
  oled.clear();       // очистка

  // рисуем горизонтальные линии осей
  oled.fastLineV(Y_AXIS_POSITION_X, 0, DISPLAY_SIZE_Y);
  for (byte i = 0; i < SENSORS; i++) {
    oled.fastLineH(CHARTS_SIZE_Y * (i + 1), 0, DISPLAY_SIZE_X);
  }

  // пишем текст у вертикальных осей
  oled.setScale(1);
  oled.textMode(BUF_ADD);
  for (byte i = 0; i < SENSORS; i++) {
    oled.setCursorXY(0, TEXT_POSITION_IN_CHART_Y + CHARTS_SIZE_Y * i);
    oled.print(chartNames[i]);
  }

  memset(history, 0xff, sizeof(history));
  memset(historyCo2, 0xffff, sizeof(historyCo2));
  memset(currentHistoryIndex, 255, sizeof(currentHistoryIndex));
}

void lcdLoop(word currentSensorValues[]) {
  for (byte sensor = 0; sensor < SENSORS; sensor++) {
    if (sensor == SENSOR_CO2 && currentSensorValues[sensor] == 0xffff) {
      continue;
    }
    addValue(sensor, currentSensorValues[sensor]);
    oled.clear(Y_AXIS_POSITION_X + 1, sensor * CHARTS_SIZE_Y + 1, DISPLAY_SIZE_X, (sensor + 1) * CHARTS_SIZE_Y - 1);
    drawValues(sensor);
    oled.setCursorXY(TEXT_POSITION_IN_CHART_X, TEXT_POSITION_IN_CHART_Y + sensor * CHARTS_SIZE_Y);
    oled.print(String(currentSensorValues[sensor]) + chartText[sensor]);
  }

  oled.update();
}

void addValue(byte sensor, word value) {
  if (currentHistoryIndex[sensor] == MAX_HISTORY_SIZE - 1) {
    for (byte i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
      if (sensor == SENSOR_CO2) {
        historyCo2[sensor - SENSOR_CO2][i] = historyCo2[sensor - SENSOR_CO2][i + 1];
      } else {
        history[sensor][i] = history[sensor][i + 1];
      }
    }
  } else {
    currentHistoryIndex[sensor]++;
  }
  if (sensor == SENSOR_CO2) {
    historyCo2[sensor - SENSOR_CO2][currentHistoryIndex[sensor]] = value;
  } else {
    history[sensor][currentHistoryIndex[sensor]] = value;
  }
}

void drawValues(byte sensor) {
  // найдем минимальное и максимальное значение в истории
  for (byte i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
    if (sensor == SENSOR_CO2) {
      if (historyCo2[sensor - SENSOR_CO2][i + 1] == 0xffff) break;

      if (historyCo2[sensor - SENSOR_CO2][i] * 0.9 < historyRange[sensor][RANGE_MIN]) {
        historyRange[sensor][RANGE_MIN] = historyCo2[sensor - SENSOR_CO2][i] * 0.9;
        // Serial.print("min changed to: ");
        // Serial.println(historyRange[sensor][RANGE_MIN]);
      }
      if (historyCo2[sensor - SENSOR_CO2][i] * 1.1 > historyRange[sensor][RANGE_MAX]) {
        historyRange[sensor][RANGE_MAX] = historyCo2[sensor - SENSOR_CO2][i] * 1.1;
        // Serial.print("max changed to: ");
        // Serial.println(historyRange[sensor][RANGE_MAX]);
      }
    } else {
      if (history[sensor][i + 1] == 0xff) break;

      if (history[sensor][i] * 0.9 < historyRange[sensor][RANGE_MIN]) {
        historyRange[sensor][RANGE_MIN] = history[sensor][i] * 0.9;
      }
      if (history[sensor][i] * 1.1 > historyRange[sensor][RANGE_MAX]) {
        historyRange[sensor][RANGE_MAX] = history[sensor][i] * 1.1;
      }
    }
  }

  float base_y = CHARTS_SIZE_Y * (sensor + 1);

  for (byte i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
    if (sensor == SENSOR_CO2) {
      if (historyCo2[sensor - SENSOR_CO2][i + 1] == 0xffff) break;
    } else {
      if (history[sensor][i + 1] == 0xff) break;
    }
    word value_range = historyRange[sensor][RANGE_MAX] - historyRange[sensor][RANGE_MIN];
    // Serial.println("========");
    // Serial.println(value_range);
    float values_in_pixel = CHARTS_SIZE_Y / (float)value_range;
    // Serial.print("values_in_pixel: ");
    // Serial.println(values_in_pixel);
    // Serial.print("historyCo2[sensor - SENSOR_CO2][i]: ");
    // Serial.print(i);
    // Serial.print(" ");
    // Serial.println(historyCo2[sensor - SENSOR_CO2][i]);

    word value_diff_with_min_prev, value_diff_with_min_next;
    if (sensor == SENSOR_CO2) {
      value_diff_with_min_prev = historyCo2[sensor - SENSOR_CO2][i] - historyRange[sensor][RANGE_MIN];
      value_diff_with_min_next = historyCo2[sensor - SENSOR_CO2][i + 1] - historyRange[sensor][RANGE_MIN];
    } else {
      value_diff_with_min_prev = history[sensor][i] - historyRange[sensor][RANGE_MIN];
      value_diff_with_min_next = history[sensor][i + 1] - historyRange[sensor][RANGE_MIN];
      // Serial.print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
    // Serial.print("historyRange[sensor][RANGE_MIN]: ");
    // Serial.println(historyRange[sensor][RANGE_MIN]);
    // Serial.print("historyRange[sensor][RANGE_MAX]: ");
    // Serial.println(historyRange[sensor][RANGE_MAX]);
    // Serial.print("value_diff_with_min_prev: ");
    // Serial.println(value_diff_with_min_prev);
    byte shift_prev_y = value_diff_with_min_prev * values_in_pixel;
    byte shift_next_y = value_diff_with_min_next * values_in_pixel;
    // Serial.print("shift_prev_y: ");
    // Serial.println(shift_prev_y);

    oled.line(Y_AXIS_POSITION_X + i + SHIFT_GRAPHICS_FROM_Y_AXIS_X, base_y - shift_prev_y, Y_AXIS_POSITION_X + i + 1 + SHIFT_GRAPHICS_FROM_Y_AXIS_X, base_y - shift_next_y);
  }
}
