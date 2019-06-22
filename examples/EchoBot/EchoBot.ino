#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <BaleBot.h>

#ifndef STASSID
#define STASSID "SSID"
#define STAPSK  "PASSWORD"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
char* token = "TOKEN";

BaleBot bot(token);

void setup() {
    Serial.begin(115200);
    EEPROM.begin(32);// 32 bit == 4 byte == int
    
    Serial.println();
    Serial.print("connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    bot.begin();
    delay(500);

}

void loop() {
  if(bot.getUpdates()){
    Serial.println(bot.Message.text);
    bot.sendMessage(bot.Message.text,bot.Message.chat_id);
  }
}
