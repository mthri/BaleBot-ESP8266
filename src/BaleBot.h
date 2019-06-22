/*
    we use ArduinoJson version 6.11
 */
#ifndef BaleBot_h
#define BaleBot_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> 
#include <EEPROM.h>

#define host "tapi.bale.ai"
#define ssl 443

struct BaleMessage{
    int update_id;
    int message_id;
    int form_id;
    String first_name;
    String username;
    int chat_id;
    String text;
};

class BaleBot{
    private:
        char* token;
        int last_update_id;
        bool Debug = false;
        String sendRequest(String url);
        // we use eeprom to save last update_id
        void EEPROM_Write(int update_id);
        int EEPROM_Read(void);
        void find_last_update_id(void);
    public:
        BaleBot(char* _token);
        BaleMessage Message;
        bool getUpdates(void);
        bool sendMessage(String text,int chat_id);
        void manualSet_updateId(int update_id);
        void begin(void);
        void _Debug(bool status);
};

#endif