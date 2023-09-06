
//Don't forget to change User_Setup.h inside TFT_eSPI library !

#include <TFT_eSPI.h>
#include <Wire.h>
#include <SPI.h>
#include "graphics.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"
#include <ESP8266WiFi.h>
#include <ESPping.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const String ssid[3]={"add 3 wifi", "names", "here"};
const String password[3]={"add 3 wifi", "passwords", "here"};

// Other stuff
const int touchPin = 16;
const int hourPin = 12;
int appState = 1;
int wifi = 0;
bool changeStatus = false;
int dstoffset = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Month names
String months[12]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

unsigned long delayTime;

TFT_eSPI tft = TFT_eSPI();   // Invoke library

void setup(void) {
  Serial.begin(115200);
  Serial.print("ST7789 TFT Bitmap Test");
  tft.begin();     // initialize a ST7789 chip
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  connectWiFi();

  // Timeclient
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.begin();
  }
  pinMode(touchPin, INPUT);
  Serial.println("-- Default Test --");
  delayTime = 25;
  Serial.println();
}

void loop() {

  if (digitalRead(touchPin) == 1 && appState != 3) {
    appState++;
    changeStatus = true;

  } else if (digitalRead(touchPin) == 1 && appState == 3) {
    appState = 1;
    changeStatus = true;

  } else {
    changeStatus = false;
  }
  Serial.println(digitalRead(touchPin));

  if (appState == 1) {
    if (changeStatus) {
      tft.fillScreen(TFT_BLACK);
    }
    drawSetupBME();
    drawUpdateBME();
    delay(delayTime);
  } else if (appState == 2) {
    if (changeStatus) {
      tft.fillScreen(TFT_BLACK);
    }
    if (WiFi.status() == WL_CONNECTED) {
      drawSetupClock();
      drawUpdateClock();
      delay(delayTime);
    } else {
      connectWiFi();
    }
    
  } else if (appState == 3) {
    if (changeStatus) {
      tft.fillScreen(TFT_BLACK);
    }
    drawSetupWifi();
    delay(delayTime);
  }
}

void drawSetupBME() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Temperature", 25, 20, 1);
  tft.drawString("Humidity", 25, 95, 1);
  tft.drawString("Elevation", 25, 170, 1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawBitmap(155, 0, bitmapallArray[2], 80, 80, TFT_WHITE);
  tft.drawBitmap(155, 80, bitmapallArray[1], 80, 80, TFT_WHITE);
  tft.drawBitmap(155, 160, bitmapallArray[0], 80, 80, TFT_WHITE);   
}

void drawUpdateBME() {
  tft.drawString(String(bme.readTemperature()) + "C", 25, 40, 2);
  tft.drawString(String(bme.readHumidity()) + "% RH", 25, 115, 2);
  tft.drawString(String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + "m", 25, 190, 2);
}

void drawSetupClock() {
  // boilerplate
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Tokyo", 10, 195, 1);
  tft.drawString("NYC", 102, 195, 1);
  tft.drawString("London", 160, 195, 1);
  tft.drawBitmap(-10, 110, epd_bitmap_allArray[2], 100, 100, TFT_WHITE);
  tft.drawBitmap(70, 110, epd_bitmap_allArray[1], 100, 100, TFT_WHITE);
  tft.drawBitmap(135, 100, epd_bitmap_allArray[0], 120, 120, TFT_WHITE);
}

void drawUpdateClock() {

  tft.setTextSize(1);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  timeClient.update();
  timeClient.setTimeOffset(-3600*7);
  time_t epochTime = timeClient.getEpochTime();
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  int monthDay = ptm->tm_mday;
  String currentMonthName = months[currentMonth-1];
  String formatMinutes;
  String formatHours[4];

  timeClient.setTimeOffset(-3600*7);
  if (timeClient.getHours() < 10) {
    formatHours[0] = "0" + String(timeClient.getHours());
  }
  else {
    formatHours[0] = String(timeClient.getHours());
  }

  timeClient.setTimeOffset(3600*9);
  if (timeClient.getHours() < 10) {
    formatHours[1] = "0" + String(timeClient.getHours());
  }
  else {
    formatHours[1] = String(timeClient.getHours());
  }

  timeClient.setTimeOffset(-3600*4);
  if (timeClient.getHours() < 10) {
    formatHours[2] = "0" + String(timeClient.getHours());
  }
  else {
    formatHours[2] = String(timeClient.getHours());
  }

  timeClient.setTimeOffset(3600);
  if (timeClient.getHours() < 10) {
    formatHours[3] = "0" + String(timeClient.getHours());
  }
  else {
    formatHours[3] = String(timeClient.getHours());
  }

  if (timeClient.getMinutes() < 10) {
    formatMinutes = "0" + String(timeClient.getMinutes());
  }
  else {
    formatMinutes = String(timeClient.getMinutes());
  }

  tft.drawString(formatHours[0] + ":" + formatMinutes, 5, 20, 7);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString(currentMonthName + " " + String(monthDay), 155, 25, 1);
  tft.drawString(String(currentYear), 155, 50, 1);

  // world clock times
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawCentreString(formatHours[1] + ":" + formatMinutes, 40, 105, 1);
  tft.drawCentreString(formatHours[2] + ":" + formatMinutes, 120, 105, 1);
  tft.drawCentreString(formatHours[3] + ":" + formatMinutes, 195, 105, 1);
}

void drawSetupWifi() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("Connection Info", 20, 20, 1);
  tft.drawString("Status:", 20, 45, 1);
  tft.drawString("IP: " + IpAddress2String(WiFi.localIP()), 20, 70, 1);
  
  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Now Online", 110, 45, 1);
  } else {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Not Online", 110, 45, 1);
    connectWiFi();
  }
  
}

String IpAddress2String(const IPAddress& ipAddress) {
  
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;

}

void connectWiFi() {
  // Connect to Wi-Fi
  for (int i = 0; i<3; i++) {
    WiFi.begin(ssid[i], password[i]);
    for (int x = 0; x < 20; x++) {
      if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      } else {
        i = 4;
      }
    }
  }
}