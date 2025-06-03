#include "BLEManager.h"

BLEManager::BLEManager(std::string serviceUUID, std::string charUUID_RX, std::string charUUID_TX)
    : serviceUUID(serviceUUID), charUUID_RX(charUUID_RX), charUUID_TX(charUUID_TX), pCharacteristic(nullptr), deviceConnected(false), send_message("waiting..."), lastMsg(0), userCallback(nullptr) {}

void BLEManager::init(std::string BLEName) {
    const char *ble_name = BLEName.c_str();
    BLEDevice::init(ble_name);

    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks(this));  // 传递 BLEManager 实例

    BLEService *pService = pServer->createService(serviceUUID);

    // 创建一个 (读) 特征值
    pCharacteristic = pService->createCharacteristic(
        charUUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());

    // 创建一个 (写) 特征值，并传递 BLEManager 实例
    BLECharacteristic *pCharacteristicRx = pService->createCharacteristic(
        charUUID_RX,
        BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristicRx->setCallbacks(new MyCallbacks(this));  // 将当前实例传递给回调

    pService->start();
    pServer->getAdvertising()->start();
    Serial.println("Waiting for a client connection to notify...");
}

void BLEManager::sendBLEMessage(std::string message) {
    send_message = message;

    if (deviceConnected && send_message.length() > 0) {
        const char *newValue = send_message.c_str();
        Serial.print("BLE: ");
        Serial.println(newValue);
        pCharacteristic->setValue(newValue);
        pCharacteristic->notify();  // 立刻发送数据
        send_message.clear();
    }
}

void BLEManager::loop() {
    // 保持 loop 供定时操作，主要逻辑已经在 sendBLEMessage 里实现
}

void BLEManager::setCallback(void (*callback)(std::string)) {
    userCallback = callback;  // 设置自定义的回调函数
}

// 服务器连接回调
void BLEManager::MyServerCallbacks::onConnect(BLEServer *pServer) {
    Serial.println("Connected successfully!");
    bleManager->deviceConnected = true;  // 修改 deviceConnected 的值
}

void BLEManager::MyServerCallbacks::onDisconnect(BLEServer *pServer) {
    Serial.println("Disconnected, restarting advertising.");
    bleManager->deviceConnected = false;  // 修改 deviceConnected 的值
    pServer->getAdvertising()->start();
}

// 特性回调
void BLEManager::MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received message: ");
        for (int i = 0; i < rxValue.length(); i++) {
            Serial.print(rxValue[i]);
        }
        Serial.println();
        Serial.println("*********");

        // 调用用户自定义回调（如果存在）
        if (bleManager->userCallback) {  // 使用 BLEManager 实例访问 userCallback
            bleManager->userCallback(rxValue);  // 执行用户定义的回调函数
        }
    }
}
