#include <SPI.h>
#include <Wire.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "arduino_secrets.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char* UBIDOTS_SERVER  = "industrial.ubidots.com";
const char* TOKEN           = "BBUS-LWgQtTBNYyzWaDwsrCKPatrhDHJUs0";
const char* DEVICE_LABEL    = "nano-iot-test";
const char* TOPIC_PUBLISH   = "/v1.6/devices/nano-iot-test";
const char* TOPIC_SUBSCRIBE = "/v1.6/devices/nano-iot-test/power/lv";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MAX30105 particleSensor;

int wifiStatus = WL_IDLE_STATUS;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// --- BPM ---
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg = 0;

// --- LED ---
unsigned long previousMillisLED = 0;
bool ledState = false;
long intervalLED = 0;

// --- Pantalla y envio ---
unsigned long previousMillisDisplay = 0;
unsigned long previousMillisMQTT = 0;
const long intervalDisplay = 5000;
const long intervalMQTT    = 5000;

// --- Suspension ---
bool suspendido = false;

void mostrarSuspendido()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 8);
  display.println("suspendido");
  display.display();
  digitalWrite(LED_BUILTIN, LOW);
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Mensaje recibido: ");
  String valor = "";
  for (int i = 0; i < length; i++)
    valor += (char)payload[i];
  Serial.println(valor);

  // Alternar suspension con cada mensaje
  suspendido = !suspendido;

  if (suspendido)
    mostrarSuspendido();
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

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 8);
  display.println("inicializando...");
  display.display();

  // Conectar WiFi
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("Conectando a: ");
    Serial.println(ssid);
    wifiStatus = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println("Conectado a WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);

  // Inicializar sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 was not found.");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  // Inicializar MQTT
  mqttClient.setServer(UBIDOTS_SERVER, 1883);
  mqttClient.setCallback(callback);
}

void loop()
{
  if (!mqttClient.connected())
    connectMQTT();

  mqttClient.loop();

  // Si esta suspendido no hace nada mas
  if (suspendido)
    return; // por raro que parezca, esto vuelve al inicio del loop

  unsigned long currentMillis = millis();
  long irValue = particleSensor.getIR();
  bool hayDedo = irValue > 50000;

  // --- Calcular BPM ---
  if (hayDedo && checkForBeat(irValue))
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute > 20 && beatsPerMinute < 255)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // --- LED segun BPM ---
  if (!hayDedo || beatAvg < 70)
  {
    intervalLED = 0;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (beatAvg < 110)
  {
    intervalLED = 1000;
  }
  else
  {
    intervalLED = 500;
  }

  if (intervalLED > 0)
  {
    if (currentMillis - previousMillisLED >= intervalLED)
    {
      previousMillisLED = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
    }
  }

  // --- Pantalla cada 5 segundos ---
  if (currentMillis - previousMillisDisplay >= intervalDisplay)
  {
    previousMillisDisplay = currentMillis;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    if (!hayDedo)
    {
      display.setTextSize(2);
      display.setCursor(0, 8);
      display.println("ERROR DEDO");
    }
    else
    {
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Pulsaciones:");
      display.setTextSize(2);
      display.setCursor(0, 12);
      display.print(beatAvg);
      display.println(" BPM");
    }

    display.display();
  }

  // --- Enviar a Ubidots por MQTT cada 5 segundos ---
  if (currentMillis - previousMillisMQTT >= intervalMQTT)
  {
    previousMillisMQTT = currentMillis;

    if (hayDedo)
    {
      String payload = "{\"pulsaciones\": " + String(beatAvg) + "}";
      mqttClient.publish(TOPIC_PUBLISH, payload.c_str());
      Serial.print("Publicado BPM: ");
      Serial.println(beatAvg);
    }
  }
}