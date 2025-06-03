#ifndef RFID_H
#define RFID_H

#include <stdint.h>
#include <list>
#include <vector>
#include <Arduino.h>

#define RX_BUFFER_LENGTH 256
#define MAX_BUFFER_LENGTH 10
#define LABEL_ID =d32
#define RFID_Prepare_Delay_Time 100

class RFID{

    private:
        HardwareSerial *_serial;
        uint8_t _buffer[RX_BUFFER_LENGTH] = {0};
        boolean WriteMode = false;
        std::string WriteLabelId = "";
        long lastDetectTime;
        long IntervalTime;

        int calculateEpcLength(std::string str);
    
    public:
        std::list<std::string> ReceivedData;

        RFID(long time);

        bool begin(HardwareSerial *serial = &Serial1, int baud = 38400,uint8_t RxPin = 8,uint8_t TxPin = 9);
        void DetectLabels();
        void WriteLabel(char bank,char epc,char length,std::string LabelId);
        void ReadLabel(char bank,char start,char length);
        void ReceiveMessage(HardwareSerial *RFID_serial);
        void loop();
        void setWriteMode(boolean write,std::string id);

    enum RFID_Memory_Map : char {
        RFID_Reserved = '0',
        RFID_EPC = '1',
        RFID_TID = '2',
        RFID_User = '3',
    };

    enum EPC_Memory_Map : char{
        EPC_CRC = '0',
        EPC_PC = '1',
        EPC_ID = '2',
    };

    enum RFID_COMMAND : char{
        RFID_Write = 'W',
        RFID_Read = 'R',
        RFID_Query = 'Q',
        RFID_QueryAll = 'U',
    };

    enum RFID_ErrorCode : char{
        ERR_CommandError = 'X',
        ERR_OtherError = '0',
        ERR_OuterOfMemory = '3',
        ERR_MemoryLocked = '4',
        ERR_LowPower = 'B',
        ERR_GeneralError = 'F',
    };


};

#endif