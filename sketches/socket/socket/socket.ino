#include <string>
#include <DHT.h>

#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>

#ifdef DEBUG_ESP_PORT
#undef DEBUG_ESP_PORT
#endif

//#define DEBUG_ESP_PORT Serial

#ifdef DEBUG_ESP_PORT
#define LOG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#endif

#ifndef LOG
#define LOG(...)
#endif

class Relay {
  public:
    Relay(const int pin) : _pin(pin) {
        pinMode(_pin, OUTPUT);
    }

    void On(){
      digitalWrite(_pin, HIGH);
      delay(1000);
      digitalWrite(_pin, LOW);
      delay(1000);
    }

    void Off(){
      digitalWrite(_pin, HIGH);
      delay(200);
      digitalWrite(_pin, LOW);
      delay(200);
      digitalWrite(_pin, HIGH);
      delay(200);
      digitalWrite(_pin, LOW);
    }
  private:

  const int _pin;
};

class Notifier {
  public:
    Notifier(const char* url, const char* topic, Relay *relay) : _mqtt_url(url), _mqtt_topic(topic), _relay(relay) {
      //topic, data, data is continuing
      _mqtt.onConnect([this]() {
        LOG("MQTT: Connected\r\n");
        LOG("Subscribe id: %d\r\n", _mqtt.subscribe(_mqtt_topic, 0));
      });
      _mqtt.onDisconnect([]() {
        LOG("MQTT: disconnected\r\n");
      });
      _mqtt.onData([this](String topic, String data, bool cont) {
        LOG("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
        if (data == "OFF"){
          _relay->Off();
        } else if (data == "ON") {
          _relay->On();
        }
      });

      _mqtt.onSubscribe([](int sub_id) {
        LOG("Subscribe topic id: %d ok\r\n", sub_id);
      });
      _mqtt.onPublish([this](int sub_id) {
        LOG("Published  to the topic %d \n", sub_id);
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

    void Run() {
      _mqtt.handle();
    }
  private:
    MQTTClient _mqtt;
    const char* _mqtt_url;
    const char* _mqtt_topic;
    Relay *_relay;

};


Notifier *n;

void setup() {
  #ifdef DEBUG_ESP_PORT
  Serial.begin(115200);
  #endif

  WiFi.begin("SSID", "PASSWORD");

  LOG("\nConnecting to WiFi\n");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG_ESP_PORT
    Serial.print(".");
    #endif
  }
  LOG("\nConnected\n");

  Relay *relay = new Relay(D6);
  n = new Notifier("mqtt://IP:PORT", "socket/pump", relay);
}

void loop() {
  n->Run();
}

