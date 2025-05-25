/*4мая25
Метеостанция на NodeMCU ESP8266 и датчике BME280
на основе "Передача с помощью NodeMCU ESP8266 данных температуры и влажности на ThingSpeak
https://microkontroller.ru/esp8266-projects/peredacha-s-pomoshhyu-nodemcu-esp8266-dannyh-temperatury-i-vlazhnosti-na-thingspeak/

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
*/

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
 
String apiKey = "*****";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "*****";     // replace with your wifi ssid and wpa2 key
const char *pass =  "*****";
const char* server = "api.thingspeak.com";

// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "+7*****";
String apiKeyW = "*****";

Adafruit_BME280 bme;

int LED = D3;
int C;  //Upravlenie znacheniem avarijnoy signalizacii

/* Управление сообщениями на Ватсапп */
int TW = 0;
unsigned long previousMillis = 0;
const long interval = (15 * 60000); //интервал равен 15-ти минутам

/*Временные задержки для посылки в ThingSpeak*/
unsigned long previousMillisTh = 0;
const long intervalTh = (15 * 1000); //интервал равен 15-ти секундам

float temperature, humidity, pressure, altitude;

String TWat = "Повышенная температура = ";
//String TWat = "Temperatur = ";
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
 
void setup() 
{
       Serial.begin(115200);
       delay(100);
	   
	     pinMode(LED, OUTPUT);
       C = 26; //avarijnoe znachenie temperatury
	   
        bme.begin(0x76);  
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
       WiFi.begin(ssid, pass);
 
       while (WiFi.status() != WL_CONNECTED) 
     {
            delay(1000);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
	    Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
	 delay(10000);	//10-секундная задержка
	 
 //Send Message to WhatsAPP
 //Отправка контрольного сообщения на WhatsAPP
      sendMessage("Привет от ESP8266 NodeMCU!");
     // sendMessage("Hello from ESP8266 NodeMCU!");

    //  delay(10000);
      
 }
 
void loop() 
{
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
		
		/*
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" degrees Celcius, Humidity: ");
        Serial.print(humidity);
        Serial.println("%, Pressure: ");
		    Serial.print(pressure);
        Serial.println("mmHg. Send to Thingspeak.");
		*/
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