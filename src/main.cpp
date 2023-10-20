// https://dzen.ru/video/watch/623afc66980c9708a2f81cdd

// Часы с погодой от yandex. ESP8266 + дисплей ILI9341
/// https://forum.arduino.ru/t/chasy-s-pogodoj-ot-yandex-esp8266-displej-ili9341/10690

#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <StreamString.h>
#include "YandexDisk.h"

#include "GyverNTP.h"
GyverNTP ntp(3);

const char WIFI_SSID[] PROGMEM = "ais1977";
const char WIFI_PSWD[] PROGMEM = "www254201";

const char OAUTH[] PROGMEM = "y0_AgAAAAANZsCBAAqohAAAAADvN98Q2CqNIHshR0WKt6Jb3eFJteaZZ8A";

YandexDisk<SPIFFS> yd(FPSTR(OAUTH));

static void halt(const __FlashStringHelper *msg)
{
  Serial.println(msg);
  Serial.flush();
  ESP.deepSleep(0);
}

static void testAppend(String stroka)
{
  StreamString ss;

  if (yd.download(F("/data.csv"), *(Stream *)&ss))
  {
    ss.println(stroka);
    if (yd.upload(F("/data.csv"), *(Stream *)&ss, true))
    {
      Serial.println(F("File updated successfully"));
    }
    else
    {
      Serial.println(F("Error uploading file!"));
    }
  }
  else
  {
    Serial.println(F("Error downloading file!"));
    yd.upload(F("/data.csv"), *(Stream *)&ss, true);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // ntp.setHost("pool.ntp.org");
  ntp.begin();

  if (!SPIFFS.begin())
  {
    if ((!SPIFFS.format()) || (!SPIFFS.begin()))
      halt(F("SPIFFS error!"));
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(FPSTR(WIFI_SSID), FPSTR(WIFI_PSWD));
  Serial.print(F("Connecting to WiFi"));
  while (!WiFi.isConnected())
  {
    Serial.print('.');
    delay(500);
  }
  Serial.print(F(" OK (IP "));
  Serial.print(WiFi.localIP());
  Serial.println(')');

  Serial.println("time");
  while (!ntp.updateNow())
  {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");

  Serial.println(ntp.timeString());

  // Serial.flush();
  // ESP.deepSleep(0);
}

void loop()
{
  ntp.tick();
  if (ntp.second() == 30)
  {
    Serial.println("tik");
    testAppend(ntp.dateString() + ntp.timeString());
  }

  delay(100);
  // Serial.println(ntp.second());
}
