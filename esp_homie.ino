#include <DHT.h>

const int TEMPERATURE_INTERVAL = 300;
unsigned long lastTemperatureSent = 0;

const int PIN_DHT22 = D4;    // Broche - Pin DHT22
DHT dht(PIN_DHT22, DHT22);

//HomieNode temperatureNode("temperature", "temperature");
//HomieNode humidityNode("humidity", "humidity");

void loopHandler() {
  //if (millis() - lastTemperatureSent >= TEMPERATURE_INTERVAL * 1000UL || lastTemperatureSent == 0) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    // Affiche les mesures dans le journal - send values to the logger

    //char buffer [50];
    //int n;

    Serial.println("");
    //n = sprintf(buffer, "temperature %f", t);
    Serial.print("Temperature: ");
    Serial.println(t);
    //n = sprintf(buffer, "humidity %f", h);
    Serial.print("Humidity: ");
    Serial.println(h);

    //Serial.write(int(t));
    //Serial << '\n';
    
    lastTemperatureSent = millis();
  //}
}

void setup() {
  Serial.begin(115200);

  //Homie_setFirmware("demo-dht22", "1.0.0");
  //Homie.setLoopFunction(loopHandler);

  //Homie.setup();
  //loopHandler();
}

void loop() {
  loopHandler();
  delay(1000);
}
