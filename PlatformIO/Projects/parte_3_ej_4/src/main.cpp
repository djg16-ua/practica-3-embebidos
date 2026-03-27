#include <Arduino.h>
#include <driver/rtc_io.h>

#define PIN_R      27
#define PIN_G      26
#define PIN_B      25
#define PIN_JUMPER  4

RTC_DATA_ATTR int numReinicios = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Inicializar pines RGB
  rtc_gpio_init((gpio_num_t)PIN_R);
  rtc_gpio_init((gpio_num_t)PIN_G);
  rtc_gpio_init((gpio_num_t)PIN_B);
  rtc_gpio_set_direction((gpio_num_t)PIN_R, RTC_GPIO_MODE_OUTPUT_ONLY);
  rtc_gpio_set_direction((gpio_num_t)PIN_G, RTC_GPIO_MODE_OUTPUT_ONLY);
  rtc_gpio_set_direction((gpio_num_t)PIN_B, RTC_GPIO_MODE_OUTPUT_ONLY);

  // Inicializar pin jumper
  rtc_gpio_init((gpio_num_t)PIN_JUMPER);
  rtc_gpio_set_direction((gpio_num_t)PIN_JUMPER, RTC_GPIO_MODE_OUTPUT_ONLY);

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

  // Verde mientras hace trabajo
  rtc_gpio_set_level((gpio_num_t)PIN_R, 0);
  rtc_gpio_set_level((gpio_num_t)PIN_G, 1);
  rtc_gpio_set_level((gpio_num_t)PIN_B, 0);

  for (int i = 0; i < 10; i++)
  {
    Serial.println("Estoy haciendo trabajo");
    delay(100);
  }

  // Rojo antes de dormir
  rtc_gpio_set_level((gpio_num_t)PIN_G, 0);
  rtc_gpio_set_level((gpio_num_t)PIN_R, 1);

  // Pin jumper en HIGH
  rtc_gpio_set_level((gpio_num_t)PIN_JUMPER, 1);

  // Mantener estados durante el deepsleep
  rtc_gpio_hold_en((gpio_num_t)PIN_R);
  rtc_gpio_hold_en((gpio_num_t)PIN_JUMPER);

  Serial.println("Entrando en modo sueño...");
  delay(500);

  esp_sleep_enable_timer_wakeup(15 * 1000000);
  esp_deep_sleep_start();
}

void loop()
{
}