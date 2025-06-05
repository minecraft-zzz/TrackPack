#include "RFID.H"

RFID::RFID(long time): IntervalTime(time){
    lastDetectTime = millis();
};

void RFID::setWriteMode(boolean write,std::string id){
    WriteMode = write;
    if(id.empty()) WriteLabelId.clear();
    else WriteLabelId = id;
}

void RFID::loop(){
    long currentTime = millis();
    if(currentTime - lastDetectTime > IntervalTime){
        #ifdef DEBUG
            Serial.print(currentTime);
            Serial.println(":new RFID loop");
        #endif
        //Serial.print(currentTime);
        Serial.println(":new RFID loop");
        Serial.printf("WriteMode:%d,WriteId:%s,",WriteMode,WriteLabelId.c_str());
        if(WriteMode){
            int UUID_length = 0;
            while(ReceivedData.empty()){
                ReadLabel(RFID_EPC,EPC_PC,'1');
                if(!ReceivedData.empty() && ReceivedData.front()[0] == 'R' && ReceivedData.front().length() >= 3) break;
                if(!ReceivedData.empty()) ReceivedData.pop_front();
                if(!WriteMode) break;
            }
            if(!ReceivedData.empty()){
                Serial.printf("ReceivedData:%s",ReceivedData.front().c_str());
                Serial.println();
                UUID_length = calculateEpcLength(ReceivedData.front());
                ReceivedData.pop_front();
                if(UUID_length == 4){
                    while(ReceivedData.empty()){
                        WriteLabel(RFID_EPC,EPC_ID,'4',WriteLabelId);
                        if(!ReceivedData.empty() && ReceivedData.front() == "W<OK>") break;
                        if(!ReceivedData.empty()) ReceivedData.pop_front();
                        if(!WriteMode) break;
                    }
                }
                else{
                    while(ReceivedData.empty()){
                        WriteLabel(RFID_EPC,EPC_PC,'1',"2400");
                        WriteLabel(RFID_EPC,EPC_ID,'4',WriteLabelId);
                        if(!ReceivedData.empty() && ReceivedData.front() == "W<OK>") break;
                        if(!ReceivedData.empty()) ReceivedData.pop_front();
                        if(!WriteMode) break;
                    }
                }
            }
            WriteMode = false;
            WriteLabelId.clear();
        }
        else{
            while(!ReceivedData.empty()){
                ReceivedData.pop_front();
            }
            DetectLabels();
        }
    Serial.printf("ReceivedData:%d",ReceivedData.size());
    Serial.println();
    lastDetectTime = currentTime;
    }
    
}

bool RFID::begin(HardwareSerial *serial,int baud,uint8_t RxPin,uint8_t TxPin){
    _serial = serial;
    _serial->begin(baud,SERIAL_8N1,RxPin,TxPin);
    return true;
}

void RFID::DetectLabels(){
    std::string Command;
    Command += RFID_QueryAll;
    Command += '\r';
    _serial->print(Command.c_str());
    Serial.printf("Command:%s",Command.c_str());
    Serial.println();
    ReceiveMessage(_serial);
}

void RFID::WriteLabel(char bank,char epc,char length,std::string LabelID){
    std::string Command;
    Command += RFID_Write;            // 添加第一个字符（RFID_Write 枚举值）
    Command += bank;
    Command += ',';                   // 添加逗号分隔符
    Command += epc;                   // 添加 EPC 字符
    Command += ',';                   // 添加逗号分隔符
    Command += length;                // 添加长度字符
    Command += ',';                   // 添加逗号分隔符
    Command += LabelID.c_str();       // 添加标签 ID
    Command += '\r';                  // 添加回车符
    _serial->print(Command.c_str());
    Serial.printf("Command:%s",Command.c_str());
    Serial.println();
    ReceiveMessage(_serial);
}

void RFID::ReadLabel(char bank,char start,char length){
    std::string Command;
    Command += RFID_Read;   // 添加读指令 'R'
    Command += bank;
    Command += ',';         // 添加逗号
    Command += start;       // 添加起始字符
    Command += ',';         // 添加逗号
    Command += length;      // 添加长度字符
    Command += '\r';        // 添加回车符
    _serial->print(Command.c_str());
    Serial.printf("Command:%s",Command.c_str());
    Serial.println();
    ReceiveMessage(_serial);
}

void RFID::ReceiveMessage(HardwareSerial* RFID_serial){
    delay(RFID_Prepare_Delay_Time);
    boolean first = true;
    std::string t = "";
    while(RFID_serial->available()){
        char c = RFID_serial->read();
        if(c == 10){
            if(!first) {
                if(t.length() > 2) ReceivedData.push_back(t);
                t = "";
            }
            first = !first;
            continue;
        }
        if(c == 13) continue;
        t += c;
    }

}

int RFID::calculateEpcLength(std::string str){
    std::string firstTwoChars = str.substr(1, 2);
    int hexValue = (int)strtol(firstTwoChars.c_str(), NULL, 16);
    int result = (hexValue >> 3) & 0x1F;
    return result;
}