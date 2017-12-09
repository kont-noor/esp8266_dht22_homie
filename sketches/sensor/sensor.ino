#include <string>
#include <DHT.h>

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


class Sensor {
  public:
    Sensor(const int pin, const int update_interval)
    :    _dht(pin, DHT22)
        ,_data_read_time(0)
        ,_update_interval(update_interval)
        ,_temp(0)
        ,_humid(0)
    {
    }

    float temperature() {
      return _temp;
    }

    float humidity() {
      return _humid;
    }

    void read() {
      if (millis() - _data_read_time >= _update_interval * 1000UL || _data_read_time == 0) {
        _temp = _dht.readTemperature();
        _humid = _dht.readHumidity();
        _data_read_time = millis();
      }
    }
  private:

    float _temp;
    float _humid;
    unsigned long _update_interval;
    unsigned long _data_read_time;
    DHT _dht;
};

class Notifier {
  public:
    Notifier(const char* url, const char* topic) : _mqtt_url(url), _mqtt_topic(topic) {
      //topic, data, data is continuing
      _mqtt.onConnect([this]() {
        LOG("MQTT: Connected\r\n");
        LOG("Subscribe id: %d\r\n", _mqtt.subscribe(_mqtt_topic, 0));
      });
      _mqtt.onDisconnect([]() {
        LOG("MQTT: disconnected\r\n");
      });
      _mqtt.onData([](String topic, String data, bool cont) {
        LOG("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
      });

      _mqtt.onSubscribe([](int sub_id) {
        LOG("Subscribe topic id: %d ok\r\n", sub_id);
      });
      _mqtt.onPublish([this](int sub_id) {
        LOG("Published: temperature - %d; humidity - %d to topic %d \n", _t, _h, sub_id);
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

    #define BUF_SIZE 7
    #define COMMON_BUF_SIZE 45

    void Notify(float h, float t) {
      _t = t;
      _h = h;

      char temp_buf[BUF_SIZE]; 
      char hum_buf[BUF_SIZE]; 
      char buf[COMMON_BUF_SIZE];

      dtostrf(t,5,2,temp_buf);
      dtostrf(h,5,2,hum_buf);

      sprintf(buf, "{\"temperature\":%s,\"humidity\":%s}", temp_buf, hum_buf);

      _mqtt.publish(_mqtt_topic, buf, 0, 0);
      _mqtt.handle();
    }
  private:
    MQTTClient _mqtt;
    const char* _mqtt_url;
    const char* _mqtt_topic;
    float _t;
    float _h;
};

// For NodeMCU use D4 or other pin you connect DHT
Sensor s(2, 300);
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

  n = new Notifier(MQTT_HOST, MQTT_QUEUE);
}

void loop() {
  s.read();
  n->Notify(s.humidity(), s.temperature());
  delay(5000);
}
