#include <Arduino.h>

#define PIN_R 27
#define PIN_G 26
#define PIN_B 25

void tareaRojo(void *parameter)
{
  pinMode(PIN_R, OUTPUT);
  while (true)
  {
    digitalWrite(PIN_R, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(PIN_R, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void tareaVerde(void *parameter)
{
  pinMode(PIN_G, OUTPUT);
  while (true)
  {
    digitalWrite(PIN_G, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    digitalWrite(PIN_G, LOW);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void tareaAzul(void *parameter)
{
  pinMode(PIN_B, OUTPUT);
  while (true)
  {
    digitalWrite(PIN_B, HIGH);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    digitalWrite(PIN_B, LOW);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);

  xTaskCreate(tareaRojo, "TareaRojo", 2048, NULL, 1, NULL);
  xTaskCreate(tareaVerde, "TareaVerde", 2048, NULL, 1, NULL);
  xTaskCreate(tareaAzul, "TareaAzul", 2048, NULL, 1, NULL);
}

void loop()
{
}