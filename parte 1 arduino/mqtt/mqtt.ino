#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char* UBIDOTS_SERVER = "industrial.ubidots.com";
const char* TOKEN          = "";
const char* DEVICE_LABEL   = "nano-iot-test";

const char* TOPIC_PUBLISH   = "/v1.6/devices/nano-iot-test";
const char* TOPIC_SUBSCRIBE = "/v1.6/devices/nano-iot-test/power/lv";

int status = WL_IDLE_STATUS;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long previousMillis = 0;
const long interval = 5000;

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Mensaje recibido en topic: ");
  Serial.println(topic);

  Serial.print("Valor: ");
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  Serial.println();
}

void connectMQTT()
{
  while (!mqttClient.connected())
  {
    Serial.print("Conectando a MQTT... ");

    if (mqttClient.connect(DEVICE_LABEL, TOKEN, ""))
    {
      Serial.println("OK");
      mqttClient.subscribe(TOPIC_SUBSCRIBE);
      Serial.println("Suscrito a: " + String(TOPIC_SUBSCRIBE));
    }
    else
    {
      Serial.print("Error: ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  while (status != WL_CONNECTED)
  {
    Serial.print("Conectando a WiFi: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println("Conectado a WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  randomSeed(analogRead(0));

  mqttClient.setServer(UBIDOTS_SERVER, 1883);
  mqttClient.setCallback(callback);
}

void loop()
{
  if (!mqttClient.connected())
    connectMQTT();

  mqttClient.loop();

  unsigned long currentMillis = millis();
}
