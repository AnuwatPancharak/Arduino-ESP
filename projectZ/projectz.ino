#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "SimpleTimer.h"
#include <BlynkSimpleEsp32.h>
#include <TridentTD_LineNotify.h>



#define LINE_TOKEN "wUH4IXt8LePfAjXwvOrSsZ8vSpEGxrtuwjox1AKmrdQ"//Token ที่จะใช้ส่งข้อมูลไปแจ้งเตือนในไลน์

SimpleTimer timer;//ตัวแปรเวลา


const char *ssid = "ANUWAT5";//ชื่อ SSIDของ Wifi ที่จะเชื่อม
const char *password = "1234512345";// รหัสผ่านของWIFI
char auth[] = "qLJFm3fAd109ONRymeTnY525KOI2XJhr";// token ของ app blynk
char server[] = "oasiskit.com";//ip server ที่เชื่อม
int port = 8080;//พอร์ต

String Location = "Khu Khan, TH";// ชื่อจุดที่ต้องการรับค่ามา
String API_Key = "8bc0adf232ae67de2ca0435ebc353c80";//api ที่ใช้เชื่อมกับwebsite ไว้ดึงข้อมูลมา

void statecheck()//ฟังชั่นเช็คสถานะ
{
  if (WiFi.status() == WL_CONNECTED) //เช็คการเชื่อมต่อไวไฟ
  {
    WiFiClient client;
    HTTPClient http; 
    http.begin(client , "http://api.openweathermap.org/data/2.5/weather?q=" + Location + "&APPID=" + API_Key);//url ที่ต้องการดึงข้อมูล
    
    int httpCode = http.GET(); //ส่งคำขอข้อมูล
    if (httpCode > 0) //เช็คโค้ด เมื่อมีค่ามากกว่า0 จะเข้าฟังชั่นนี้
    {
      String payload = http.getString(); //เก็บค่า
      DynamicJsonBuffer jsonBuffer(512);
      
      JsonObject& root = jsonBuffer.parseObject(payload);//เช็ตค่าและเช็ค
      if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        return;
      }
      float temp = (float)(root["main"]["temp"]) - 273.15; // เก็บค่าอุณหภูมิ
      int humidity = root["main"]["humidity"]; //เก็บค่าความชื้น
      float pressure = (float)(root["main"]["pressure"]) / 1000; //เก็บค่าแรงลม
      float wind_speed = root["wind"]["speed"]; //เก็บค่าความเร็วลม
      int wind_degree = root["wind"]["deg"]; //เก็บค่าองศา
      //ปริ้นต์ฟข้อมูลออกหน้าSerial
      Serial.printf("Temperature = %.2f°C\r\n", temp);
      Serial.printf("Humidity = %d %%\r\n", humidity);
      Serial.printf("Pressure = %.3f bar\r\n", pressure);
      Serial.printf("Wind speed = %.1f m/s\r\n", wind_speed);
      Serial.printf("Wind degree = %d°\r\n\r\n", wind_degree);

      //ส่งค่าไปในแอปBlynk
      Blynk.virtualWrite(V1, temp);
      Blynk.virtualWrite(V2, humidity);
      Blynk.virtualWrite(V3, pressure);
      Blynk.virtualWrite(V4, wind_speed);
      Blynk.virtualWrite(V5, wind_degree);

    }
    http.end();
  }
}



void setup(void)
{
  Serial.begin(9600);

  WiFi.begin(ssid, password);//เชื่อมต่อ

  Serial.print("Connecting.");//ส่งค่าออกlog
  

  while ( WiFi.status() != WL_CONNECTED )//เช็คสถานะไวไฟ
  {
    delay(500);
    Serial.print(".");
  }
  Blynk.config(auth, server, port); //กำหนด Token key , ชื่อ Server และ port
  Blynk.connect(); //เชื่อมต่อไปยัง Blynk
  Serial.println("connected");
  
  timer.setInterval(30000, statecheck);//ตั้งเวลา30วิให้ข้อมูลอัพเดท1ครั้ง
}

BLYNK_CONNECTED()//ฟังชั่นแสดงเมื่อเชื่อมต่อกับแอปได้
{
  LINE.setToken(LINE_TOKEN);//เชื่อมต่อกับโทเค่นไลน์นั้นๆ
  LINE.notify("เชื่อมต่อไวไฟสำเร็จ");//ส่งข้อความไปที่ไลน์
  Serial.println("App Blynk ทำงาน!");//ส่งค่าออกlog
}

void loop()
{
  Blynk.run();//ฟังชั่นืทำงานของแอปBlynk
  timer.run();//ฟังชั่นนับเวลาทุกๆ 30วิ
}