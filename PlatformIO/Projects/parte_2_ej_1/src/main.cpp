#include <Arduino.h>

void tareaSerial(void *parameter)
{
  while (true)
  {
    Serial.print("Hola soy la tarea ");
    Serial.print(pcTaskGetName(NULL));
    Serial.print(", me estoy ejecutando en el core ");
    Serial.println(xPortGetCoreID());
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void tareaLED(void *parameter)
{
  pinMode(BUILTIN_LED, OUTPUT);
  while (true)
  {
    digitalWrite(BUILTIN_LED, HIGH);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    digitalWrite(BUILTIN_LED, LOW);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);

  xTaskCreate(tareaSerial, "TareaSerial", 2048, NULL, 1, NULL);
  xTaskCreate(tareaLED, "TareaLED", 2048, NULL, 1, NULL);
}

void loop()
{
  // con FreeRTOS el loop se puede dejar vacio
}