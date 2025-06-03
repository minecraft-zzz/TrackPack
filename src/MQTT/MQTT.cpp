#include "MQTT.H"
#include <Arduino.h>

MQTT::MQTT(const char* ssid, const char* password, const char* mqttServer, int mqttPort,
                       const char* mqttUsername, const char* mqttPassword, const char* clientId, const char* topic)
    : ssid(ssid), password(password), mqttServer(mqttServer), mqttPort(mqttPort),
      mqttUsername(mqttUsername), mqttPassword(mqttPassword), clientId(clientId), topic(topic), client(espClient) {}

void MQTT::loop(){
    if(!client.connected()){
        reconnect();
    }
    client.loop();
}

void MQTT::begin(void (*callback)(char*, byte*, unsigned int)){
    #ifdef DEBUG
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);
    #endif
    WiFi.begin(ssid, password); 

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

void MQTT::reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(clientId, mqttUsername, mqttPassword)) {
            Serial.println("connected");
            client.subscribe(topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}


// 发布消息
void MQTT::publish(const char* topic, const char* message) {
    client.publish(topic, message);
}

// 订阅主题
void MQTT::subscribe(const char* topic) {
    client.subscribe(topic);
}

void MQTT::shutdown(){
    if (client.connected()) {
        client.disconnect();
        Serial.println("MQTT disconnected");
    }

    // 断开Wi-Fi连接
    WiFi.disconnect();
    Serial.println("WiFi disconnected");
}