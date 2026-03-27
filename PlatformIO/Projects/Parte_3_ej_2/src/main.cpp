#include <Arduino.h>

#define PIN_R 27
#define PIN_G 26
#define PIN_B 25

// RTC_DATA_ATTR hace que la variable sobreviva al deepsleep
RTC_DATA_ATTR int numReinicios = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  // Solo contar reinicios por deepsleep
  esp_reset_reason_t motivo = esp_reset_reason();
  if (motivo == ESP_RST_DEEPSLEEP)
  {
    numReinicios++;
    Serial.print("Reinicio por deepsleep numero: ");
    Serial.println(numReinicios);
  }
  else
  {
    numReinicios = 0;
    Serial.println("Primer arranque, contador reseteado");
  }

  // Encender verde y hacer trabajo
  digitalWrite(PIN_G, HIGH);
  for (int i = 0; i < 10; i++)
  {
    Serial.println("Estoy haciendo trabajo");
    delay(100);
  }
  digitalWrite(PIN_G, LOW);

  // Encender rojo y entrar en deepsleep
  digitalWrite(PIN_R, HIGH);
  Serial.println("Entrando en modo sueño...");
  delay(500);

  esp_sleep_enable_timer_wakeup(15 * 1000000);
  esp_deep_sleep_start();
}

void loop()
{
}