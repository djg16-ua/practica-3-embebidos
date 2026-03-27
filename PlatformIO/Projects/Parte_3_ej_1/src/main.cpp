#include <Arduino.h>

#define PIN_R 27
#define PIN_G 26
#define PIN_B 25

void setup()
{
  Serial.begin(115200);
  delay(1000); // esperar a que el serial esté listo

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

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
  delay(500); // pequeña pausa para que se vea el rojo

  esp_sleep_enable_timer_wakeup(15 * 1000000); // 15 segundos en microsegundos
  esp_deep_sleep_start();

  Serial.println("No se deberia llegar aquí");
}

void loop()
{
  // no se ejecuta nunca, el deepsleep resetea el MCU al despertar
}