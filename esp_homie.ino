#include <string>
#include <DHT.h>

#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>

class Sensor {
  public:
    Sensor()
    :   dht(PIN_DHT22, DHT22),
        dataReadTime(0) 
    {
    }

    float temperature() {
      return temp;
    }

    float humidity() {
      return humid;
    }

    void read() {
      if (millis() - dataReadTime >= TEMPERATURE_INTERVAL * 1000UL || dataReadTime == 0) {
        temp = dht.readTemperature();
        humid = dht.readHumidity();
        dataReadTime = millis();
      }
    }
  private:
    const int TEMPERATURE_INTERVAL = 300;
    const int PIN_DHT22 = D4;    // Broche - Pin DHT22

    float temp;
    float humid;
    unsigned long dataReadTime;
    DHT dht;
};

class Notifier {
  public:
    Notifier() {
      //topic, data, data is continuing
      mqtt.onConnect([this]() {
        Serial.printf("MQTT: Connected\r\n");
        Serial.printf("Subscribe id: %d\r\n", mqtt.subscribe("/qos0", 0));
      });
      mqtt.onDisconnect([this]() {
        Serial.printf("MQTT: disconnected\r\n");
      });
      mqtt.onData([this](String topic, String data, bool cont) {
        Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
      });
    
      mqtt.onSubscribe([this](int sub_id) {
        Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
      });
      mqtt.onPublish([this](int sub_id) {
        Serial.printf("Published: temperature - %d.d; humidity - %d.%d to topic %d \n", _t, _h, sub_id);
      });

      Serial.printf("\nInit mqtt %s\n", mqtt_url);
    
      if(mqtt.begin(mqtt_url))
      {
        Serial.printf("MQTT Init OK\n");
      }
      else
      {
        Serial.printf("MQTT Init FAILED\n");
      }
    }

    #define BUF_SIZE 5

    void Notify(float h, float t) {
      _t = t;
      _h = h;
      char buf[BUF_SIZE]; 
      sprintf(buf, "%d.%d %d.%d", t, h);
      mqtt.publish("/qos0", buf, 0, 0);
      memset(buf, BUF_SIZE*sizeof(char), 0);
      delay(5000);
      mqtt.handle();
    }
  private:
    MQTTClient mqtt;
    const char* mqtt_url = "mqtt://test.mosquitto.org:1883";
    float _t;
    float _h;
};


Sensor s;
Notifier *n;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin("net", "pass");

  Serial.printf("\nConnecting to WiFi\n");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected\n");

  n = new Notifier();
}

void loop() {
  s.read();
  n->Notify(s.humidity(), s.temperature());
}
