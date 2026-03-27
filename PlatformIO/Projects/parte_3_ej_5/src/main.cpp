#include <Arduino.h>
#include <driver/rtc_io.h>
#include <sys/time.h>

#define PIN_R 27
#define PIN_G 26
#define PIN_B 25
#define PIN_WAKEUP 4

RTC_DATA_ATTR int numReinicios = 0;
RTC_DATA_ATTR time_t tiempoAntesSueno = 0;

void imprimirMotivo()
{
  esp_sleep_wakeup_cause_t causa = esp_sleep_get_wakeup_cause();

  switch (causa)
  {
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Despertado por: timer");
    break;
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Despertado por: pin GPIO4");
    break;
  default:
    Serial.println("Primer arranque");
    break;
  }

  struct timeval ahora;
  gettimeofday(&ahora, NULL);
  long tiempoDormido = ahora.tv_sec - tiempoAntesSueno;
  Serial.print("Tiempo dormido: ");
  Serial.print(tiempoDormido);
  Serial.println(" segundos");
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  rtc_gpio_init((gpio_num_t)PIN_R);
  rtc_gpio_init((gpio_num_t)PIN_G);
  rtc_gpio_init((gpio_num_t)PIN_B);
  rtc_gpio_set_direction((gpio_num_t)PIN_R, RTC_GPIO_MODE_OUTPUT_ONLY);
  rtc_gpio_set_direction((gpio_num_t)PIN_G, RTC_GPIO_MODE_OUTPUT_ONLY);
  rtc_gpio_set_direction((gpio_num_t)PIN_B, RTC_GPIO_MODE_OUTPUT_ONLY);

  esp_reset_reason_t motivo = esp_reset_reason();
  if (motivo == ESP_RST_DEEPSLEEP)
  {
    numReinicios++;
    Serial.print("Reinicio numero: ");
    Serial.println(numReinicios);
    imprimirMotivo();
  }
  else
  {
    numReinicios = 0;
    Serial.println("Primer arranque");
  }

  rtc_gpio_set_level((gpio_num_t)PIN_R, 0);
  rtc_gpio_set_level((gpio_num_t)PIN_G, 1);
  rtc_gpio_set_level((gpio_num_t)PIN_B, 0);

  for (int i = 0; i < 10; i++)
  {
    Serial.println("Estoy haciendo trabajo");
    delay(100);
  }

  rtc_gpio_set_level((gpio_num_t)PIN_G, 0);
  rtc_gpio_set_level((gpio_num_t)PIN_R, 1);
  rtc_gpio_hold_en((gpio_num_t)PIN_R);

  // Guardar tiempo antes de dormir
  struct timeval ahora;
  gettimeofday(&ahora, NULL);
  tiempoAntesSueno = ahora.tv_sec;

  esp_sleep_enable_timer_wakeup(15 * 1000000);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, 1);
  rtc_gpio_pulldown_en((gpio_num_t)PIN_WAKEUP);
  rtc_gpio_pullup_dis((gpio_num_t)PIN_WAKEUP);

  Serial.println("Entrando en modo sueno...");
  Serial.println("Toca GPIO4 con 3.3V para despertar");
  delay(500);

  esp_deep_sleep_start();
}

void loop()
{
}