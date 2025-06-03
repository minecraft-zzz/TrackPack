#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>
#include <Arduino.h>

class BLEManager {
public:
    BLEManager(std::string serviceUUID, std::string charUUID_RX, std::string charUUID_TX);
    
    void init(std::string BLEName);
    void sendBLEMessage(std::string message);
    void loop();
    
    void setCallback(void (*callback)(std::string));

private:
    std::string serviceUUID;
    std::string charUUID_RX;
    std::string charUUID_TX;
    BLECharacteristic *pCharacteristic;
    bool deviceConnected;  // 要修改的成员
    std::string send_message;
    long lastMsg;

    void (*userCallback)(std::string);  // 用户自定义回调函数指针

    // 内部回调类
    class MyServerCallbacks : public BLEServerCallbacks {
    public:
        MyServerCallbacks(BLEManager *bleManager) : bleManager(bleManager) {}  // 接收 BLEManager 实例
        void onConnect(BLEServer* pServer);
        void onDisconnect(BLEServer* pServer);
    
    private:
        BLEManager *bleManager;  // 保存 BLEManager 实例的指针
    };

    class MyCallbacks : public BLECharacteristicCallbacks {
    public:
        MyCallbacks(BLEManager *bleManager) : bleManager(bleManager) {}  // 接收 BLEManager 实例
        void onWrite(BLECharacteristic* pCharacteristic);
    
    private:
        BLEManager *bleManager;  // 指向 BLEManager 的指针
    };
};

#endif // BLEMANAGER_H
