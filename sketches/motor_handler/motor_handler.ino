#include <string>

#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>
#include "settings.h"

#ifdef DEBUG_ESP_PORT
#undef DEBUG_ESP_PORT
#endif

#define DEBUG_ESP_PORT Serial

#ifdef DEBUG_ESP_PORT
#define LOG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#endif

#ifndef LOG
#define LOG(...)
#endif

class Notifier {
  public:
    Notifier(const char* url, const char* income, const char* outgo) : _mqtt_url(url), _mqtt_income(income), _mqtt_outgo(outgo) {
      //topic, data, data is continuing
      _mqtt.onConnect([this]() {
        LOG("MQTT: Connected\r\n");
        LOG("Subscribe id: %d\r\n", _mqtt.subscribe(_mqtt_income, 0));
      });
      _mqtt.onDisconnect([]() {
        LOG("MQTT: disconnected\r\n");
      });
      _mqtt.onData([this](String topic, String data, bool cont) {
        // TODO: handle motor here
        LOG("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
        Notify();
      });

      _mqtt.onSubscribe([](int sub_id) {
        LOG("Subscribe topic id: %d ok\r\n", sub_id);
      });
      _mqtt.onPublish([this](int sub_id) {
        LOG("Published: smth to topic %d \n", sub_id);
      });

      LOG("\nInit mqtt %s\n", _mqtt_url);

      if(_mqtt.begin(_mqtt_url))
      {
        LOG("MQTT Init OK\n");
      }
      else
      {
        LOG("MQTT Init FAILED\n");
      }
    }

    #define COMMON_BUF_SIZE 45

    void Notify() {
      char buf[COMMON_BUF_SIZE];

      sprintf(buf, "{\"status\":%s}", _status);

      _mqtt.publish(_mqtt_outgo, buf, 0, 0);
      _mqtt.handle();
    }
  private:
    MQTTClient _mqtt;
    const char* _mqtt_url;
    const char* _mqtt_income;
    const char* _mqtt_outgo;
    uint8_t _status;
};

Notifier *n;
int ledPin = 1;

void setup() {
  #ifdef DEBUG_ESP_PORT
  Serial.begin(115200);
  #endif
  pinMode(ledPin, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  LOG("\nConnecting to WiFi\n");

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    #ifdef DEBUG_ESP_PORT
    Serial.print(".");
    #endif
  }
  LOG("\nConnected\n");
  for(int i = 0; i < 4; i ++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

  n = new Notifier(MQTT_HOST, MQTT_INCOME_QUEUE, MQTT_OUTGO_QUEUE);
}

void loop() {
}
