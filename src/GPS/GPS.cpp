#include "GPS.h"

GPS::GPS() {
    lastGPSMsgTime = millis();

    memset(read_G_name, 0, sizeof(read_G_name));
}

bool GPS::begin(HardwareSerial *serial, int baud, uint8_t RxPin, uint8_t TxPin) {
    GPS_Serial = serial;
    GPS_Serial->begin(baud, SERIAL_8N1, RxPin, TxPin);
    return true;
}

void GPS::GetLocation(std::string& la, std::string& lo) {
    la = Latitude;  // 直接将纬度和经度字符串赋值给输入参数
    lo = Longitude;
}

void GPS::ClearLocation(){
    Latitude.clear();
    Longitude.clear();
}

bool GPS::GPS_Prepared() {
    if (!Longitude.empty() && !Latitude.empty()) return true;
    return false;
}

// void GPS::loop() {
//     int i = 0;
//     bool flag = true;
//     long currentTime = millis();
//     if (currentTime - lastGPSMsgTime > GPS_DetachTime) {
//         if (GPS_Serial->available()) {
//             char c = GPS_Serial->read();
//             if (c == '$') {
//                 info_line.clear();  // 清空之前的内容
//                 read_name.clear();
//                 i = 0;
//                 do {
//                     c = GPS_Serial->read();
//                     if (c != '\n' && c >= 0 && c <= 127 ) {
//                         info_line += c;  // 直接将字符追加到 std::string 中
//                     }
//                 } while (c != '\n');
//                 analysis_pos();
//             }   
//         }
//         lastGPSMsgTime = currentTime;
//     }
// }

void GPS::loop() {
    int i = 0;
    bool flag = true;
    long currentTime = millis();
    if (currentTime - lastGPSMsgTime > GPS_DetachTime) {
        while (GPS_Serial->available() && flag) {
            char c = GPS_Serial->read();
            if(c == '$'){
                read_name.clear();
                info_line.clear();
                for(i = 0;i < 5;i++){
                    c = GPS_Serial->read();
                    read_name += c;
                }
                if(strcmp(read_name.c_str(), pre_G_name[0]) == 0){
                    flag = false;
                    info_line = read_name;
                    do {
                        c = GPS_Serial->read();
                        if (c != '\n' && c >= 0 && c <= 127 ) {
                            info_line += c;  // 直接将字符追加到 std::string 中
                        }
                    } while (c != '\n');
                }
            }
        }
        analysis_pos();
        lastGPSMsgTime = currentTime;
    }
}

// void GPS::loop() {
//     int i = 0;
//     long currentTime = millis();
//     if (currentTime - lastGPSMsgTime > GPS_DetachTime) {
//         info_line = "GNGGA,132506.000,2233.87430,N,11407.13740,E,1,13,1.0,103.3,M,-2.8,M,*5E";
//         analysis_pos();
//         lastGPSMsgTime = currentTime;
//     }
// }


void GPS::analysis_pos() {
    char ch;
    int cnt = 0;
    int j;
    Serial.printf("GPS_info_line:%s",info_line.c_str());
    Serial.println();
    for (j = 0; j < 5; j++) {
        read_G_name[j] = info_line[j];  // 保持 char[] 复制
    }
    if (strcmp(read_G_name, pre_G_name[0]) == 0) {
        while (true) {
            ch = info_line[j++];
            if (ch == ',') cnt++;
            if (cnt == 2) {
                Latitude.clear();  // 清空之前的内容
                do {
                    ch = info_line[j++];
                    if (ch == ',') cnt++;
                    else {
                        Latitude += ch;  // 追加到 Latitude
                    }
                } while (cnt < 4);
            }
            if (cnt == 4) {
                Longitude.clear();  // 清空之前的内容
                do {
                    ch = info_line[j++];
                    if (ch == ',') cnt++;
                    else {
                        Longitude += ch;  // 追加到 Longitude
                    }
                } while (cnt < 6);
                Serial.printf("Longitude:%s,Latitude:%s",Longitude.c_str(),Latitude.c_str());
                Serial.println();
                return;
            }
        }
    }

}
