#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <ArduinoJson.h>

const char* ssid = "Tbag";
const char* password = "Dbcooper";

WebSocketsServer webSocket = WebSocketsServer(81);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            Serial.printf("[%u] Connected!\n", num);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(41,42);
    
    // Initialize SHT31
    if (!sht31.begin(0x44)) {
        Serial.println("Couldn't find SHT31");
        while (1) delay(1);
    }

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Start WebSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    
    static unsigned long lastRead = 0;
    if (millis() - lastRead > 2000) {  // Read every 2 seconds
        float temp = sht31.readTemperature();
        float hum = sht31.readHumidity();
        
        if (!isnan(temp) && !isnan(hum)) {
            StaticJsonDocument<200> doc;
            doc["temperature"] = temp;
            doc["humidity"] = hum;
            
            String jsonString;
            serializeJson(doc, jsonString);
            
            webSocket.broadcastTXT(jsonString);
        }
        
        lastRead = millis();
    }
} 