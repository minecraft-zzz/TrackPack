#include <Arduino.h>
#include <BLE/BLEManager.h>
#include <RFID/RFID.h>
#include <GPS/GPS.h>
#include <MQTT/MQTT.h>

#define RFID_RXPIN 8  // 定义RX引脚 连接RFID模块
#define RFID_TXPIN 9  // 定义TX引脚 连接RFID模块

#define GPS_RXPIN 16  // 定义RX引脚 连接GPS模块
#define GPS_TXPIN 17  // 定义TX引脚 连接GPS模块

#define BLESerial Serial
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

const char* ssid     = "Redmi_K20Pro";  //WIFI账户和密码，需要手动填写
const char* password = "12345678";
const char* MQTT_SERVER  = "60.204.251.213";
const int   MQTT_PORT    = 1883;
const char* MQTT_USRNAME = "ESP32";
const char* MQTT_PASSWD  = "ESP32***";
const char* TOPIC = "ESP/Control/Light";
const char* Publish_RFID_Topic = "ESP/Sensor/RFID";
const char* Publish_GPS_Topic = "ESP/Sensor/GPS";
const char* Publish_Temperature_Topic = "ESP/Sensor/Temperature";
const char* CLIENT_ID    = "ESP32";  //当前设备的clientid标志

void customBLECallback(std::string receviedMessage);
void callback(char* topic, byte* payload, unsigned int length); //MQTT回调函数
BLEManager BLE(SERVICE_UUID,CHARACTERISTIC_UUID_RX,CHARACTERISTIC_UUID_TX);
MQTT mqtt(ssid,password,MQTT_SERVER,MQTT_PORT,MQTT_USRNAME,MQTT_PASSWD,CLIENT_ID,TOPIC);
RFID rfid(1000);
GPS gps;

void setup(){
    Serial.begin(9600);
    Serial.print("test0");
    BLE.init("TrackPack BLE");
    BLE.setCallback(customBLECallback);
    Serial.print("testBLE");
    rfid.begin(&Serial1,38400,RFID_RXPIN,RFID_TXPIN);
    Serial.print("testRFID");
    gps.begin(&Serial2,9600,GPS_RXPIN,GPS_TXPIN);
    Serial.print("testGPS");
    mqtt.begin(callback);
    Serial.print("testMQTT");
}

void loop(){
    //Serial.print("new loop");
    rfid.loop();
    mqtt.loop();
    while(!rfid.ReceivedData.empty()){
        std::string data = rfid.ReceivedData.front();
        Serial.printf("ReceivedData:%s",data.c_str());
        Serial.println();
        if(data[0] == 'U') {
            BLE.sendBLEMessage(data.substr(5,data.length()-9));
            mqtt.publish(Publish_RFID_Topic,data.c_str());
        }
        if(data[0] == 'W' && data.substr(1,4) == "<OK>") BLE.sendBLEMessage("Write Success");
        rfid.ReceivedData.pop_front();
    }
    gps.loop();
    if(gps.GPS_Prepared()){
        std::string la;
        std::string lo;
        gps.GetLocation(la,lo);
        BLE.sendBLEMessage(la);
        BLE.sendBLEMessage(lo);
        gps.ClearLocation();
        mqtt.publish(Publish_GPS_Topic,la.c_str());
        mqtt.publish(Publish_GPS_Topic,lo.c_str());
    }
}

void customBLECallback(std::string receivedMessage) {
    Serial.print("Custom callback received: ");
    Serial.println(receivedMessage.c_str());
    if(receivedMessage == "W"){
        rfid.setWriteMode(true,"4444555566667777");
    }
    else{
        rfid.setWriteMode(false,"");
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);   // 打印主题信息
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]); // 打印主题内容
  }
  Serial.println();
}

// #include <Arduino.h>
// #include <BLE/BLEManager.h>
// #include <RFID/RFID.h>
// #include <GPS/GPS.h>
// #include <MQTT/MQTT.h>

// const char* ssid     = "Redmi_K20Pro";  //WIFI账户和密码，需要手动填写
// const char* password = "12345678";
// const char* MQTT_SERVER  = "123.60.134.9";
// const int   MQTT_PORT    = 1883;
// const char* MQTT_USRNAME = "ESP32";
// const char* MQTT_PASSWD  = "ESP32***";
// const char* TOPIC = "ESP/Control/Light";
// const char* Publish_RFID_Topic = "ESP/Sensor/RFID";
// const char* Publish_GPS_Topic = "ESP/Sensor/GPS";
// const char* Publish_Temperature_Topic = "ESP/Sensor/Temperature";
// const char* CLIENT_ID    = "ESP32";  //当前设备的clientid标志

// void callback(char* topic, byte* payload, unsigned int length);

// MQTT mqtt(ssid,password,MQTT_SERVER,MQTT_PORT,MQTT_USRNAME,MQTT_PASSWD,CLIENT_ID,TOPIC);


// void setup(){
//     Serial.begin(9600);
//     mqtt.begin(callback);
// }


// void loop(){
//     mqtt.loop();

// }

// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("Message arrived [");
//   Serial.print(topic);   // 打印主题信息
//   Serial.print("] ");
//   for (int i = 0; i < length; i++) {
//     Serial.print((char)payload[i]); // 打印主题内容
//   }
//   Serial.println();
// }

// #include <Arduino.h>

// void setup(){
//     Serial.begin(9600);
//     Serial.println("test0");
// }

// void loop(){
//     Serial.println("test1");
// }

// #include <Arduino.h>

// void setup() {
//   Serial.begin(38400);
//   Serial1.begin(38400,SERIAL_8N1,8,9);

// }

// void loop() {
//   Serial.println("Starting a new loop iteration");
//   // 增加一些额外的延迟，确保模块准备好接收命令
//   delay(200);
//   // 发送 "Multi EPC" 命令
//   Serial1.println("U\r");
//   // 等待一小段时间确保数据传输完成
//   delay(100);
//   Serial.println("Reading from RFID module:");
//   // 读取并解析返回消息
//   while (Serial1.available()) {
//     char c = Serial1.read();
//     Serial.print(c);
//   }
//   Serial.println();
//   delay(1000);  // 等待一秒，然后进行下一次读取
// }