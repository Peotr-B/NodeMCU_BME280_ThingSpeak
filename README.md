# Scriptor42
 
 5мая25
 @Scriptor42
 Scriptor42@list.ru

Проект "NodeMCU_BME280_ThingSpeak" на основе статьи:

https://microkontroller.ru/esp8266-projects/peredacha-s-pomoshhyu-nodemcu-esp8266-dannyh-temperatury-i-vlazhnosti-na-thingspeak/

Дисклеймер
Данный проект является частью подготовки к созданию системы Умный Дом (IoT)

Необходимые компоненты
    1. NodeMCU ESP8266
    2. Датчик BME280
      2.1. Интерфейс: I2C и SPI.
      2.2. Напряжение питания: от 1,8 В до 5,0 В.
      2.3. Диапазон температур: от -40 до +85°C.
      2.4. Диапазон влажности: от 0% до 100% отн. влажности.
      2.5. Диапазон давления: от 300 гПа до 1100 гПа.
     Датчик влажности и датчик давления можно включать/отключать 
	 независимо друг от друга.
	 I2C адрес 0x76
WiFi.localIP: 
ВНИМАНИЕ
Добавил индикацию давления (в mmHg)!

Процесс

1. 5мая25
Добавил аварийную сигнализацию температуры на Thingspeak: при превышении 26°C
на плате загорается Lamp Indikator

2. 23мая25
https://projectalt.ru/publ/arduino_esp8266_i_esp32/programmirovanie/esp8266_nodemcu_otpravka_soobshhenij_v_whatsapp/11-1-0-26
ESP8266 NodeMCU: отправка сообщений в WhatsApp
При превышении аварийного значения температуры (для отладки выбрал 26°С) 
каждые 15 минут на WhatsApp посылается сообщение вида:
Повышенная температура = 26.33



