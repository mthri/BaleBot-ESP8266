#include "BaleBot.h"
//You can find this at this site https://www.grc.com/fingerprints.htm
const char fingerprint[] PROGMEM = "57 DE C0 B0 5F CB 17 8C A4 F2 31 FC 18 38 33 67 CC 01 53 33";
BaleBot::BaleBot(char* _token){
  token = _token;
}
void BaleBot::_Debug(bool status){
  this->Debug = status;
}
void BaleBot::begin()
{
    this->find_last_update_id();
}
bool BaleBot::getUpdates(){
  String result,url;
  int message_id;
  url = String(host)+"/bot"+String(token)+"/getUpdates?limit=1&offset="+String(last_update_id+1);
  result = this->sendRequest(url);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, result);
  JsonObject obj = doc.as<JsonObject>();
  message_id = obj["result"][0]["message"]["message_id"];
  if(message_id == NULL) {
    if(Debug)
      Serial.println("no update");
    return false;
  }

  Message.update_id = obj["result"][0]["update_id"];
  Message.message_id = message_id;
  Message.form_id = obj["result"][0]["message"]["from"]["id"];
  Message.first_name = obj["result"][0]["message"]["from"]["first_name"].as<String>();
  Message.username = obj["result"][0]["message"]["from"]["username"].as<String>();
  Message.chat_id = obj["result"][0]["message"]["chat"]["id"];
  Message.text = obj["result"][0]["message"]["text"].as<String>();
  if(Debug)
    Serial.println("we have new message");
  last_update_id++;
  this->EEPROM_Write(last_update_id);
  return true;
}
bool BaleBot::sendMessage(String text,int chat_id){
  text.replace(" ","%20"); 
  String url = String(host)+"/bot"+String(token)
  +"/sendMessage?chat_id="+String(chat_id)
  +"&text="+text;

  if(this->sendRequest(url))
    return true;
  else
    return false;
}
String BaleBot::sendRequest(String url){
  WiFiClientSecure client;
  client.setFingerprint(fingerprint); 
  if (!client.connect(host, ssl)) {
      return "connection failed";
    }   
    client.print(String("GET ") +"https://"+ url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: ESP8266\r\n" +
                 "Connection: close\r\n\r\n"); 
    
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") 
      break;  
  }
  String response = client.readStringUntil('\n');
  return response;
}
void BaleBot::manualSet_updateId(int update_id){
  this->EEPROM_Write(update_id);
}
void BaleBot::EEPROM_Write(int update_id){
  EEPROM.put(0,update_id);
  EEPROM.commit();
}
int BaleBot::EEPROM_Read(){
  int number;
  EEPROM.get(0,number);
  return number;
}
void BaleBot::find_last_update_id(){
  Serial.println("Pleas wait to find last update_id, i'm working on ...");
  DynamicJsonDocument doc(1024);
  JsonObject obj;
  String result,url;
  int update_id,new_update_id,_update_id,message_id;
  update_id = EEPROM_Read();
  if(Debug)
    Serial.println("last update_id in eeprom is "+String(update_id));
  if(update_id < 0)
    new_update_id = 0;
  else
    new_update_id = update_id;

  while (true)
  {
    url = String(host)+"/bot"+String(token)+"/getUpdates?limit=1&offset="+String(++new_update_id);
    result = this->sendRequest(url);
    deserializeJson(doc, result);
    obj = doc.as<JsonObject>();
    _update_id= obj["result"][0]["update_id"];
    message_id = obj["result"][0]["message"]["message_id"];
    if(Debug){
      Serial.println("message_id is "+String(message_id)+" and update_id is "+String(_update_id));
      Serial.println(url);
      Serial.println(result);
    }
    if(message_id == NULL)
      break;
    new_update_id = _update_id;
  }
  if(new_update_id-1 != update_id)
    this->EEPROM_Write(new_update_id-1);

  this->last_update_id = new_update_id-1;
  Serial.println("Done \nBot is ready to use :D");
}