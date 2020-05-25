#include <M5Stack.h>

#define TINY_GSM_MODEM_UBLOX
#include <TinyGsmClient.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ArduinoJson.h>

#define MQTT_SERVER "beam.soracom.io"
#define MQTT_SERVERPORT 1883
#define READ_TIMEOUT 5000

const char TOPIC[] PROGMEM = "iotbutton/status";

TinyGsm modem(Serial2); /* 3G board modem */
TinyGsmClient ctx(modem);
Adafruit_MQTT_Client mqtt(&ctx, MQTT_SERVER, MQTT_SERVERPORT);
Adafruit_MQTT_Subscribe buttonStatusIndicator = Adafruit_MQTT_Subscribe(&mqtt, TOPIC);

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println(F("M5Stack + 3G Module"));

  M5.Lcd.print(F("modem.restart()"));
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  modem.restart();
  M5.Lcd.println(F("done"));

  M5.Lcd.print(F("getModemInfo:"));
  String modemInfo = modem.getModemInfo();
  M5.Lcd.println(modemInfo);

  M5.Lcd.print(F("waitForNetwork()"));
  while (!modem.waitForNetwork()) M5.Lcd.print(".");
  M5.Lcd.println(F("Ok"));
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0);

  M5.Lcd.print(F("gprsConnect(soracom.io)"));
  modem.gprsConnect("soracom.io", "sora", "sora");
  M5.Lcd.println(F("done"));
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0);

  M5.Lcd.print(F("isNetworkConnected()"));
  while (!modem.isNetworkConnected()) M5.Lcd.print(".");
  M5.Lcd.println(F("Ok"));
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0);

  M5.Lcd.print(F("My IP addr: "));
  IPAddress ipaddr = modem.localIP();
  M5.Lcd.println(ipaddr);

  mqtt.subscribe(&buttonStatusIndicator);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0);

  // スリープからの復帰をAボタンに割り当て
  M5.setWakeupButton(BUTTON_A_PIN);

  M5.Lcd.print(F("Startup complete!"));
  delay(2000);
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    M5.Lcd.setCursor(0,0);
    M5.Lcd.printf("Good Morning!");
    delay(10000);
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(0,0);
  }
  if (M5.BtnC.wasPressed()) {
    M5.Lcd.setCursor(0,0);
    M5.Lcd.printf("Good Night!");
    delay(10000);
    setDeepSleep();
  }

  connectMqtt();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(READ_TIMEOUT))) {
    if (subscription == &buttonStatusIndicator) {
      String lastread = String((char*)buttonStatusIndicator.lastread);
      lastread.trim();
      StaticJsonDocument<200> jsonDoc;

      DeserializationError err = deserializeJson(jsonDoc, lastread);
      if (err) {
        Serial.print(F("deserializeJson() failed: "));
        return;
      }
      // JSONデータを割りあて
      const char* message = jsonDoc["message"];
      unsigned int color = strToHex(jsonDoc["color"]);
      unsigned int statusType = jsonDoc["status"];

      if (statusType == 0) {
        continue;
      }
      if (statusType == 1) {
        M5.Lcd.setBrightness(200);
        showMessage(color, message);
      } else if (statusType == 2) {
        M5.Lcd.setBrightness(200);
        showMessage(color, message);
      } else if (statusType == 3) {
        showMessage(color, message);
        delay(30000);
        M5.Lcd.clear(BLACK);
        M5.Lcd.setBrightness(0);
        setDeepSleep();
      }
    }
  }
}

void showMessage(uint16_t bgColor, const char* message) {
  M5.Lcd.fillScreen(bgColor);
  M5.Lcd.setCursor(30, 100);
  M5.Lcd.setTextSize(4);
  M5.Lcd.println(F(message));
}

uint16_t strToHex(const char* str) {
  return (uint16_t) strtoull(str, 0, 16);
}


void setDeepSleep() {
  M5.Power.begin();
  if (!M5.Power.canControl()) {
    //can't control.
    return;
  }
  // 12時間DeepSleep
  M5.Power.deepSleep(SLEEP_HR(12));
}
void connectMqtt() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }

  while ((ret = mqtt.connect()) != 0) {
    M5.Lcd.println(F(mqtt.connectErrorString(ret)));
    M5.Lcd.println(F("Retrying MQTT connection in 5 seconds ..."));
    mqtt.disconnect();
    delay(5000);
  }
}
