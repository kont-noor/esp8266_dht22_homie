#include <string>
#include <DHT.h>

#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>

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
        Serial.printf("MQTT: Connected\r\n");
        Serial.printf("Subscribe id: %d\r\n", _mqtt.subscribe(_mqtt_topic, 0));
      });
      _mqtt.onDisconnect([]() {
        Serial.printf("MQTT: disconnected\r\n");
      });
      _mqtt.onData([](String topic, String data, bool cont) {
        Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
      });

      _mqtt.onSubscribe([](int sub_id) {
        Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
      });
      _mqtt.onPublish([this](int sub_id) {
        Serial.printf("Published: temperature - %d; humidity - %d to topic %d \n", _t, _h, sub_id);
      });

      Serial.printf("\nInit mqtt %s\n", _mqtt_url);

      if(_mqtt.begin(_mqtt_url))
      {
        Serial.printf("MQTT Init OK\n");
      }
      else
      {
        Serial.printf("MQTT Init FAILED\n");
      }
    }

    #define BUF_SIZE 7
    #define COMMON_BUF_SIZE 20

    void Notify(float h, float t) {
      _t = t;
      _h = h;

      char temp_buf[BUF_SIZE]; 
      char hum_buf[BUF_SIZE]; 
      char buf[COMMON_BUF_SIZE];

      dtostrf(t,5,2,temp_buf);
      dtostrf(h,5,2,hum_buf);
      
      sprintf(buf, "%s %s", temp_buf, hum_buf);
      
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

Sensor s(D4, 300);
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

  n = new Notifier("mqtt://192.168.7.3:1883", "sensor/data");
}

void loop() {
  s.read();
  n->Notify(s.humidity(), s.temperature());
  delay(5000);
}
