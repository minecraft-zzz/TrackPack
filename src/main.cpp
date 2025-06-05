#include <Arduino.h>
#include <vector>
#include <map>
#include <BLE/BLEManager.h>
#include <RFID/RFID.h>
#include <GPS/GPS.h>
#include <MQTT/MQTT.h>
#include <TAG/TAG.h>

#define RFID_RXPIN 8  // 定义RX引脚 连接RFID模块
#define RFID_TXPIN 9  // 定义TX引脚 连接RFID模块

#define GPS_RXPIN 16  // 定义RX引脚 连接GPS模块
#define GPS_TXPIN 17  // 定义TX引脚 连接GPS模块

#define MAX_TAG_NUM = 50

#define BLESerial Serial
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define RFID_Scan_Frequency 1000

// const char* ssid     = "Redmi_K20Pro";  //WIFI账户和密码，需要手动填写
// const char* password = "12345678";
// const char* MQTT_SERVER  = "60.204.251.213";
// const int   MQTT_PORT    = 1883;
// const char* MQTT_USRNAME = "ESP32";
// const char* MQTT_PASSWD  = "ESP32***";
// const char* TOPIC = "ESP/Control/Light";
// const char* Publish_RFID_Topic = "ESP/Sensor/RFID";
// const char* Publish_GPS_Topic = "ESP/Sensor/GPS";
// const char* Publish_Temperature_Topic = "ESP/Sensor/Temperature";
// const char* CLIENT_ID    = "ESP32";  //当前设备的clientid标志

void customBLECallback(std::string receviedMessage);
BLEManager BLE(SERVICE_UUID,CHARACTERISTIC_UUID_RX,CHARACTERISTIC_UUID_TX);
void responseToBLEMessage(std::string receivedMessage);

void solveTags(std::string data);
// void callback(char* topic, byte* payload, unsigned int length); //MQTT回调函数
// MQTT mqtt(ssid,password,MQTT_SERVER,MQTT_PORT,MQTT_USRNAME,MQTT_PASSWD,CLIENT_ID,TOPIC);
RFID rfid(RFID_Scan_Frequency);
GPS gps;

std::map<Tag, int> Tags;
void solveTags(std::string data);
void maintianTags();
long lastTime;

void setup(){
    Serial.begin(9600);
    BLE.init("TrackPack BLE");
    BLE.setCallback(customBLECallback);
    rfid.begin(&Serial1,38400,RFID_RXPIN,RFID_TXPIN);
    gps.begin(&Serial2,9600,GPS_RXPIN,GPS_TXPIN);
    Tags.clear();
    lastTime = millis();
    // mqtt.begin(callback);

}

void loop(){
    //Serial.print("new loop");
    rfid.loop();
    // mqtt.loop();
    while(!rfid.ReceivedData.empty()){
        std::string data = rfid.ReceivedData.front();
        Serial.printf("ReceivedData:%s",data.c_str());
        Serial.println();
        if(data[0] == 'U') solveTags(data);
        // if(data[0] == 'U') {
        //     BLE.sendBLEMessage(data.substr(5,data.length()-9));
        //     mqtt.publish(Publish_RFID_Topic,data.c_str());
        // }
        // if(data[0] == 'W' && data.substr(1,4) == "<OK>") BLE.sendBLEMessage("Write Success");
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
        // mqtt.publish(Publish_GPS_Topic,la.c_str());
        // mqtt.publish(Publish_GPS_Topic,lo.c_str());
    }
    maintianTags();
}

void customBLECallback(std::string receivedMessage) {
    Serial.print("Custom callback received: ");
    Serial.println(receivedMessage.c_str());
    responseToBLEMessage(receivedMessage);
    // if(receivedMessage == "W"){
    //     rfid.setWriteMode(true,"4444555566667777");
    // }
    // else{
    //     rfid.setWriteMode(false,"");
    // }


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

/*UUID的结构是这样的 U + 4位EPC_PC + ?位EPC + 4位EPC_CRC
第一个U是从模块收到信息附带的，提示这是检测多张标签的结果
EPC_PC是长度位，计算方式是取EPC_PC二进制的前5位，比如3000和3400对应的前五位都是 00110，然后把前面补0成0000 0110，对应的值是6，也就是EPC长度为6个字
CRC是校验码，自动得出
*/
void solveTags(std::string data){
    Serial.println("solveTags0");
    if(data.length() <= 5) return;
    Serial.println("solveTags1");
    Tag tag;
    std::string label = data.substr(1,data.length()-1);
    tag.UUID = label;
    Serial.printf("label_length:%d,rfid_cal_result:%d\n",label.length(),rfid.calculateEpcLength(data));
    if(label.length() != (rfid.calculateEpcLength(data) + 2) * 4) return;
    Serial.println("solveTags2");
    if(Tags.find(tag) != Tags.end()){
        Tags[tag] = LIVE_TIME;
    }
    else{
        Tags[tag] = LIVE_TIME;
        BLE.sendBLEMessage(NewLabelUpperHalf + getUpperHalf(label));
        BLE.sendBLEMessage(NewLabelLowerHalf + getLowerHalf(label));
    }
}

void maintianTags(){
    long currentTime = millis();
    if(currentTime - lastTime > OneSecond){
        Serial.println("*************************************");
        Serial.println("current Tags:");
        for(auto it = Tags.begin(); it != Tags.end(); ++it){
            it->second--;
            if(it->second == 0){
                BLE.sendBLEMessage(LostLabelUpperHalf + getUpperHalf(it->first.UUID));
                BLE.sendBLEMessage(LostLabelLowerHalf + getLowerHalf(it->first.UUID));
                Tags.erase(it);
            }
            else{
                Serial.printf("TagID:%s,LiveTime:%d \t\n", it->first.UUID.c_str(), it->second);
            }
        }
        Serial.println("*************************************");
        lastTime = currentTime;
    }
}

void responseToBLEMessage(std::string receivedMessage){
    if(receivedMessage == FirstTimeConnection){
        for(const auto& pair: Tags){
            BLE.sendBLEMessage(FirstTimeConnectionUpperHalf + getUpperHalf(pair.first.UUID));
            BLE.sendBLEMessage(FirstTimeConnectionLowerHalf + getLowerHalf(pair.first.UUID));
        }
    }
}
