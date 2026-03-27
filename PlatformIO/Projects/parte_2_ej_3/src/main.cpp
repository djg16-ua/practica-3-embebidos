#include <Arduino.h>

#define PIN_R 27
#define PIN_G 26
#define PIN_B 25

struct ParamsLED
{
  int pin;
  int delayMs;
};

void tareaLED(void *parameter)
{
  ParamsLED *params = (ParamsLED *)parameter;
  int pin = params->pin;
  int delayMs = params->delayMs;

  pinMode(pin, OUTPUT);

  while (true)
  {
    digitalWrite(pin, HIGH);
    vTaskDelay(delayMs / portTICK_PERIOD_MS);
    digitalWrite(pin, LOW);
    vTaskDelay(delayMs / portTICK_PERIOD_MS);
  }
}

ParamsLED paramsRojo = {PIN_R, 1000};
ParamsLED paramsVerde = {PIN_G, 2000};
ParamsLED paramsAzul = {PIN_B, 5000};

void setup()
{
  Serial.begin(115200);

  xTaskCreate(tareaLED, "TareaRojo", 2048, &paramsRojo, 1, NULL);
  xTaskCreate(tareaLED, "TareaVerde", 2048, &paramsVerde, 1, NULL);
  xTaskCreate(tareaLED, "TareaAzul", 2048, &paramsAzul, 1, NULL);
}

void loop()
{
}