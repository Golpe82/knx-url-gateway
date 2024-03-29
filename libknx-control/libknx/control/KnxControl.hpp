#ifndef KNXCONTROL_HPP_
#define KNXCONTROL_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <iomanip>

class knxControl
{
private:
    const std::string kSerial{"/dev/ttyAMA0"};
    const std::vector<std::string> kDatapointtypes{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18"};
    const std::vector<uint8_t> kFt1_2{0x68, 0x16}; // start and end byte of FT1.2 protocol
    std::vector<uint8_t> ft12_reset_telegram{0x10, 0x40, 0x40, 0x16};
    uint8_t control_byte_{0x73}; // ft1.2 control byte even/odd
    // link layer on telegram:
    const std::vector<uint8_t> kLlon{
        kFt1_2[0], 0x09, 0x09, kFt1_2[0], control_byte_, 0xF6, 0x00, 0x08, 0x01, 0x34, 0x10, 0x01, 0x00, 0xB7, kFt1_2[1]};
    // link layer off telegram:
    std::vector<uint8_t> lloff_{
        kFt1_2[0], 0x09, 0x09, kFt1_2[0], 0xFF, 0xF6, 0x00, 0x08, 0x01, 0x34, 0x10, 0x01, 0xF0, 0xA7, kFt1_2[1]};
    // Data.req telegram
    std::vector<uint8_t> header_{kFt1_2[0], 0xFF, 0xFF, kFt1_2[0], 0xFF};
    std::vector<uint8_t> tail_{0xFF, kFt1_2[1]};
    std::vector<uint8_t> ga_{0xFF, 0xFF, 0xFF};
    const std::vector<uint8_t> kSa{0x00, 0x00}; // frame source address
    std::vector<uint8_t> value_{0xFF};
    std::vector<uint8_t> body_{0x11, 0x00, 0x9C, 0xE0, kSa[0], kSa[1], 0xFF, 0xFF, 0x01, 0x00, value_[0]};
    std::vector<uint8_t> telegram_; // whole Data.req frame

    const std::vector<uint8_t> kBool{0x80, 0x81}; // boolean values
    const std::vector<uint8_t> kDimm{0x8D, 0x85}; //+6,25% | -6,25%

    uint8_t main_gr_;
    uint8_t midd_gr_;
    uint8_t addr_;
    uint8_t chksum_{0x00};
    uint8_t length_;
    int dpt_ = -1;

    std::vector<uint8_t> ConvertNumberToKNXFloat(float number);
    void SetValueToKNXString(std::string value_string);
    uint8_t GetDatapointType(std::string str);
    void SetGroupaddress(std::string main_group, std::string mid_group, std::string address);
    void SetValue(std::string requested_value);
    bool is_valid_dpt(std::string dpt);

public:
    uint8_t GetMainGr() const { return main_gr_; }
    uint8_t GetMiddGr() const { return midd_gr_; }
    uint8_t GetAddr() const { return addr_; }
    std::vector<uint8_t> GetValue() const { return value_; }
    std::vector<uint8_t> GetGa() const { return ga_; }
    int GetDpt() const { return dpt_; }
    void SetData(std::vector<std::string> req_str, std::string dpt_str); // converts data strings
    bool SendFrame();                                                    // calculates frame and sends it to the KNX bus
};
#endif
