/*4мая25
Метеостанция на NodeMCU ESP8266 и датчике BME280
на основе "Передача с помощью NodeMCU ESP8266 данных температуры и влажности на ThingSpeak
https://microkontroller.ru/esp8266-projects/peredacha-s-pomoshhyu-nodemcu-esp8266-dannyh-temperatury-i-vlazhnosti-na-thingspeak/

28мая25
https://kotyara12.ru/iot/cloud_services/
kotyara12.ru
В бесплатном аккаунте сервис позволяет создавать до четырех каналов, в каждом из каналов 
можно хранить до 8 полей с различными данными. Еще в бесплатном аккаунте существует ещё
одно ограничение – нельзя публиковать данные чаще одного раза в 15 секунд (рекомендуется
пауза в 20 секунд), это как бы “защита” от превышения лимита 8 200 сообщений в день. 
В принципе, все мои “публикаторы” легко вписываются в данный лимит, мне хватает. 
Пользуюсь сервисом довольно давно, достаточно удобен и прост, если не считать MatLab. 
Есть несколько программы – клиентов для android, но гораздо меньше, чем для MQTT.

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
Индикацию давления перевёл в mmHg!

23мая25
https://projectalt.ru/publ/arduino_esp8266_i_esp32/programmirovanie/esp8266_nodemcu_otpravka_soobshhenij_v_whatsapp/11-1-0-26
ESP8266 NodeMCU: отправка сообщений в WhatsApp

14июн25
Разработал библиотеки для сокрытия информации по логинам и паролям на основе:
https://www.instructables.com/Build-a-Custom-ESP8266-Arduino-WiFi-Library/
и упорядочил скетч
*/
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>

#include <My_WiFi.h>
My_WiFi my_wifi; // создаём экземпляр нашего класса
const char* ssid = my_wifi.ssid();
const char* pass = my_wifi.passcode();

#include <My_WhatsApp.h>
My_WhatsApp my_whatsapp; // создаём экземпляр нашего класса
String phoneNumber = my_whatsapp.phoneNumber();
const char* apiKeyW = my_whatsapp.apiKeyW();

#include <My_ThingSpeak.h>
My_ThingSpeak my_thingspeak; // создаём экземпляр нашего класса
//const char* apiKey = my_thingspeak.apiKeyTh();
String apiKey = my_thingspeak.apiKeyTh();

const char* server = "api.thingspeak.com";

Adafruit_BME280 bme;

int LED = D3;
int C;  //Upravlenie znacheniem avarijnoy signalizacii

/* Управление сообщениями на Ватсапп */
int TW = 0;
unsigned long previousMillis = 0;
const long interval = (15 * 60000); //интервал равен 15-ти минутам

/*Временные задержки для посылки в ThingSpeak*/
unsigned long previousMillisTh = 0;
const long intervalTh = (20 * 1000); //интервал равен 20-ти секундам

float temperature, humidity, pressure, altitude;

String TWat = "Повышенная температура = ";
String TWatZ;

WiFiClient client;

//Подпрограмма отправки сообщения в Ватсапп
void sendMessage(String message){

 // Data to send with HTTP POST
 String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKeyW + "&text=" + urlEncode(message);
 WiFiClient client; 
 HTTPClient http;
 http.begin(client, url);

 // Specify content-type header
 //Отправка запроса HTTP post и сохранение кода ответа
 http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
 // Send HTTP POST request
 //Если код ответа равен 200, это означает, что запрос post прошел успешно. 
 //В противном случае что-то пошло не так.
 int httpResponseCode = http.POST(url);
 if (httpResponseCode == 200){
 Serial.print("Message sent successfully");
 }
 else{
 Serial.println("Error sending the message");
 Serial.print("HTTP response code: ");
 Serial.println(httpResponseCode);
 }

 // Free resources
 //Освобождение ресурсов
 http.end();
}

void ConnecTOWifi(void) ;
byte tries = 20;  // Попыткок подключения к точке доступа
 
void setup() 
{
       Serial.begin(115200);
       delay(100);
	   
	     pinMode(LED, OUTPUT);
       C = 26; //avarijnoe znachenie temperatury
	   
        bme.begin(0x76);  
		
	ConnecTOWifi();
  delay(2000);	//2-секундная задержка
	 
 //Send Message to WhatsAPP
 //Отправка контрольного сообщения на WhatsAPP
      sendMessage("Привет от ESP8266 NodeMCU!");
     // sendMessage("Hello from ESP8266 NodeMCU!");

    //  delay(10000);

Serial.print("phoneNumber: ");
Serial.println(phoneNumber);

Serial.print("apiKeyW: ");
Serial.println(apiKeyW);

Serial.print("ssid: ");
Serial.println(ssid);

Serial.print("pass: ");
Serial.println(pass);

Serial.print("apiKey: ");
Serial.println(apiKey);

delay(10000);

 }
 
void loop() 
{
	
	// Reconnect to Wi-Fi
    if(WiFi.status() != WL_CONNECTED)
		ConnecTOWifi();
	
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
	pressure = bme.readPressure() / 100.0F * 0.75;
  TWatZ = TWat+temperature;
	  
    if(temperature > C)
	{
      digitalWrite(LED, HIGH);
      unsigned long currentMillis = millis();
	  
	  if (currentMillis - previousMillis >= interval) 
	  {
		  previousMillis = currentMillis;
		  TW = 0;;
		  }

    if(!TW)
	{
    //  sendMessage("Повышенная температура!");
      sendMessage(TWatZ);
      TW = 1;
      }
      
      }
    else
	{
      digitalWrite(LED, LOW);
    //  TW = 0;
      }
  Serial.print("TW = ");
  Serial.println(TW);

    if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
      {  
		unsigned long currentMillisTh = millis();
		  if (currentMillisTh - previousMillis >= intervalTh) 
		    {
		      previousMillisTh = currentMillisTh;

          String postStr = apiKey;
          postStr +="&field1=";
          postStr += String(temperature);
          postStr +="&field2=";
          postStr += String(humidity);
		      postStr +="&field3=";
          postStr += String(pressure);
          postStr += "\r\n\r\n";
 
          client.print("POST /update HTTP/1.1\n");
          client.print("Host: api.thingspeak.com\n");
          client.print("Connection: close\n");
          client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
          client.print("Content-Type: application/x-www-form-urlencoded\n");
          client.print("Content-Length: ");
          client.print(postStr.length());
          client.print("\n\n");
          client.print(postStr);
		
		    }
      }
    client.stop();
 		
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(humidity);
    Serial.println("%, Pressure: ");
		Serial.print(pressure);
    Serial.println("mmHg. Send to Thingspeak.");
 
    Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}

void ConnecTOWifi()
{   Serial.print("Connecting to ");             // Печать "Подключение к:"
    Serial.println(ssid);                       // Печать "Название Вашей WiFi сети"
	WiFi.mode(WIFI_STA); // nodemcu as station
	WiFi.begin(ssid, pass);                 // Подключение к WiFi Сети
//	Serial.print ("connecting to wifi");
	while (--tries && WiFi.status() != WL_CONNECTED)
	{
		delay(500);                               // Пауза 500 мкс
        Serial.print(".");                        // Печать "."
		Serial.print(tries);
	}
	
	if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println();
    Serial.println("Non Connecting to WiFi..");
    tries=20;
  }
  else
  {
    // Иначе удалось подключиться отправляем сообщение
    // о подключении и выводим адрес IP
    Serial.println("");
    Serial.println("WiFi connected");
   	Serial.print("IP Address:");
	  Serial.println(WiFi.localIP());
	  Serial.print("MacAddress:");
	  Serial.println(WiFi .macAddress());
  }

}