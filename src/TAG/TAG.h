#ifndef TAG_H
#define TAG_H

#include <stdint.h>
#include <list>
#include <map>
#include <Arduino.h>
#include <stdexcept>

#define UUID_LENGTH 24
#define LIVE_TIME 20
#define OneSecond 1000

struct Tag {
    std::string UUID;

    friend bool operator<(const Tag& a, const Tag& b) {
        return a.UUID < b.UUID;
    }
};

std::string hexToBytes(const std::string& hexStr);
std::string getUpperHalf(std::string ID);
std::string getLowerHalf(std::string ID);


#endif 