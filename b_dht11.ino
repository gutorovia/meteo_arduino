// DHT11 OUT -> D2

#define DHT11_DPIN 2

// инициализация датчика
DHT dht11(DHT11, DHT11_DPIN);

float getTemperature() {
  return dht11.readTemperature();
}

float getHumidity() {
  return dht11.readHumidity();
}
