#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <WiFiClientSecure.h>

const char* ssid = "Tbag";
const char* password = "Dbcooper";

WebSocketsServer webSocket = WebSocketsServer(81);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Add SSL Certificate (self-signed for testing)
const char cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDkzCCAnugAwIBAgIUBxM3WJf2bP12kAfqhmhhjZWv0ukwDQYJKoZIhvcNAQEL
BQAwWTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM
GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDESMBAGA1UEAwwJbG9jYWxob3N0MB4X
DTIwMDUwMTAwMDAwMFoXDTIxMDUwMTAwMDAwMFowWTELMAkGA1UEBhMCQVUxEzAR
BgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5
IEx0ZDESMBAGA1UEAwwJbG9jYWxob3N0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A
MIIBCgKCAQEA1ZvJzsV7xPB7YWyT7mQQJ5hY1B0/jR6QZOS9c7SG0lUB+UyTGjMS
F6uF6ZzJ8oCpYw3zKlDdH9GBXqpD+YUH8aU85RTqaEyC4lWzEoE+QXBQhxZrWDsK
ZCPH4Oa0VP6jX5T4Ynf6Z0Uj8dOYDLN1fN5U3MnK5Y5hoPTHJD5hYsZ3KMhEPEL1
Y1J9UqyQYbfWpz9U2P/9QwDGxXzXwh5qGYJz+aV/KXJK3l8TQ5s4RGMiBZJSVBuQ
5RbFvY8U5UXFPmJ1kKFxsqGHWmR8pQ4r9QqRwLMnZhQ5R0v8iUOQVZdO1RqEnp+x
HNxQ3vqDVn+wHzqPjqEFQo0jh5hYYQIDAQABo1MwUTAdBgNVHQ4EFgQU5PgRc+Kk
0QvgH8LTXAdX3ZqO+8wwHwYDVR0jBBgwFoAU5PgRc+Kk0QvgH8LTXAdX3ZqO+8ww
DwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAI7hxMY3UWgLqEPOE
yJJ5QkwqY0cXbBg6YR5ZQhXvWHlAEuYV5EwxU9BqY0sBzWxRX4ZkTHGe3NTxNV6D
Gu7Y1caAUG5FgX6l6wNzQYR8mHgM1u7QpvdH+3rsLmqz+JjwYH8KUqEWVX8BxdXx
J7mEcU9OZh7QbH1mVp8FrVZtxMx7UyBZMR0P3YuPHFsYGzEPhcI5UE9vR/YKpE9/
7h4pKvBgVBmDrdXR9QvRBHUBDj8hXVZh4XhQ3vZUJqGxfjPQqnlbD1kQBmg5qZh6
XDY5m5KQ9jv8UBi+lHHG0UBkPZ8XjjPJVP6ybQdCwbBLf8yKqCUhZ/BgpQ==
-----END CERTIFICATE-----
)EOF";

const char private_key[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQDVm8nOxXvE8Hth
bJPuZBAnmFjUHT+NHpBk5L1ztIbSVQH5TJMaMxIXq4XpnMnygKljDfMqUN0f0YFe
qkP5hQfxpTzlFOpoTILiVbMSgT5BcFCHFmtYOwpkI8fg5rRU/qNflPhid/pnRSPx
05gMs3V83lTcycrljmGg9MckPmFixncoyEQ8QvVjUn1SrJBht9anP1TY//1DAMbF
fNfCHmoZgnP5pX8pckrfXxNDmzhEYyIFklJUG5DlFsW9jxTlRcU+YnWQoXGyoYda
ZHylDiv1CpHAsydmFDlHS/yJQ5BVl07VGoSen7Ec3FDe+oNWf7AfOo+OoQVCjSOH
mFhhAgMBAAECggEBALt/iFQwLFCg4tHeNqIJ6+0r4kPRJrZQ9aB9NSbgk2a6gMx5
O+5mnqwWwTZPJLGR3JqmMSGQxQn8sYc4L8ZJw9N2/30vKBkFXsE7GnbO4FJL4qHJ
KUq5NHbbriJJjGdkFl+dmwr3qK2LFQnqUkjWWvBnl4aoVDuJdZ1BpcFrXYovfvsh
Yx7dZKy7tqEv9MEXleZLW4fHX5kHiHwCXARGVULUhXZxQtoYXl0QUi3kGkF4kdQr
tOvsUG8+4zzqS5BZ1ZbK+9H5b5QrTYOxPxKr5jQ5QRXhJoCqLnM6gVqQKl/2qPq5
L5xUI8tXkhIEgWpEAXOVl/ftxKZx+Yai7dzxdhkCgYEA/Kt5tVvYYY5hYYqHYpz5
IJJKX/hGq1YJQ3VzO5c7K1fA5p+NGI8O6wB8jk5K+8YGzu/dQxeXv1/sTi7I5Hq7
EJGYiuCXpQAzYvj5UqkR7CJoatTiWHYgF4X5YPvXGE7NHviEXMyVn4rh8hX6KjyQ
X6+4dpBx5TQVxg1K2+UCgYEA2JQ1rtxqM8XkxYKAH6e2C5PF1+2K+MxFk8G+EHZK
Q/Jc2qACnKS5upVOoaqH7kO7jQ8SvHYXFPZGjgqn8LKkq3V+wO+K0SX6VB8VwYKo
Q5i6+ZKMGhpWFI6+jW/+JqvHhzW7pBcXrFjzrWD8glXg4ZNMsYS5EGXtBXkVij0C
gYEAu5+l7QLjftCu4KjxCCxN4dDZl0LRHZ1+qsp6T4Y8+eN8vKXWR4c5kU7bzI+D
YKQJ5RX5K5E6dYvVlJn4uWJQ1Yu9qYKmY5f7yY5MuYnCR8yaGXwRJjkD5GFvhyqS
vf7NvMwMvUuESZ2R4vvAcZvq9qJjYRHZnvQBL9HbcWECgYBVdXGJ4DvCXCPx6+b0
kGrMQv6igHhGzpWxX9v3GUxdwQpMzGBmYQ6qw3UHgQHxPsvH8wZmGYFyVbNwjY5s
BqVpVXyGD7vhwMFl8GYlF3cydJ3TvJJD0dB5r4Y1dXI5B5GyUmC8YvFmHqoy5H+Z
Gjk9oWQydLvsQqR8fRRKUQKBgQCBems9V/rTAtkk1nXBOYHP6GtxKYeY9J6cqwQf
UXpHQOgM6p6d1YqRw3XB3igRD1wQLGtHxjsLmE0COY3g5E85A9PzLd3WGBoPZ7eX
wR3G6Y4tkLkR6qVq4KLxGmA6hPd8QxbdNvX+b2mj0Vx8Sg6A5QKqOY6sNB5M3prK
Og==
-----END PRIVATE KEY-----
)EOF";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Client Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[%u] Client Connected - HTTP Origin: %s\n", num, webSocket.remoteIP(num).toString().c_str());
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] Received text: %s\n", num, payload);
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
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("WebSocket server started on port: ");
    Serial.println(81);

    // Enable mDNS responder
    if(!MDNS.begin("esp32sensor")) {
        Serial.println("Error setting up MDNS responder!");
    }
    
    // Configure WebSocket server
    webSocket.enableHeartbeat(15000, 3000, 2);
    webSocket.setAuthorization(""); // Allow any client
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