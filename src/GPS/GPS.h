#ifndef GPS_H
#define GPS_H
#define GPS_DetachTime 5000

#include <Arduino.h>
#include <string>

class GPS {
    private:
        HardwareSerial* GPS_Serial;
        char pre_G_name[3][6] = {"GNGGA", "GNGLL", "GNRMC"};
        // 读到的定位编码名称
        char read_G_name[6];
        // 一行定位信息
        std::string info_line;
        std::string Latitude;
        std::string Longitude;
        std::string read_name;
        long lastGPSMsgTime;
        long lastBLETime;

    public:
        GPS();
        bool begin(HardwareSerial *serial, int baud, uint8_t RxPin, uint8_t TxPin);
        void GetLocation(std::string& la, std::string& lo); // 接收引用，避免拷贝
        void ClearLocation();
        void loop();
        bool GPS_Prepared();
        void analysis_pos();
};

#endif
