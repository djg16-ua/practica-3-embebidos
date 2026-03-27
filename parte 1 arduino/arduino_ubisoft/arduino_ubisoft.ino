#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char* UBIDOTS_SERVER = "industrial.ubidots.com";
const char* API_KEY        = "BBUS-LWgQtTBNYyzWaDwsrCKPatrhDHJUs0";
const char* DEVICE_LABEL   = "nano-iot-test";
const char* VAR_LABEL      = "pulsaciones";

int status = WL_IDLE_STATUS;
WiFiSSLClient client;

unsigned long previousMillis = 0;
const long interval = 5000;

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
    Serial.print("Conectando a: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println("Conectado a WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  randomSeed(analogRead(0));
}

void sendToUbidots(int value)
{
  String body = "{\"" + String(VAR_LABEL) + "\": " + String(value) + "}";

  Serial.print("Conectando con Ubidots... ");

  if (client.connect(UBIDOTS_SERVER, 443))
  {
    Serial.println("OK");

    String request = "POST /api/v1.6/devices/" + String(DEVICE_LABEL) + "/ HTTP/1.1\r\n";
    request += "Host: industrial.ubidots.com\r\n";
    request += "X-Auth-Token: " + String(API_KEY) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + String(body.length()) + "\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";
    request += body;

    client.print(request);

    long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 10000)
      {
        Serial.println("Timeout");
        client.stop();
        return;
      }
    }

    Serial.println("Respuesta:");
    while (client.available())
    {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();
  }
  else
  {
    Serial.println("Error de conexion");
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    int pulsaciones = random(60, 121);
    sendToUbidots(pulsaciones);
  }
}