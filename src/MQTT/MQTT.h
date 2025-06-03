#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>

class MQTT{
    private:
        const char* ssid;
        const char* password;
        const char* mqttServer;
        const int mqttPort;
        const char* mqttUsername;
        const char* mqttPassword;
        const char* clientId;
        const char* topic;
        WiFiClient espClient;
        PubSubClient client;

    public:
        MQTT(const char* ssid, 
            const char* password, 
            const char* mqttServer, 
            int mqttPort,
            const char* mqttUsername, 
            const char* mqttPassword, 
            const char* clientId, 
            const char* topic);
        void begin(void (*callback)(char*, byte*, unsigned int));
        void reconnect();
        void publish(const char* topic,const char* message);
        void subscribe(const char* topic);
        void shutdown();
        void loop();

};