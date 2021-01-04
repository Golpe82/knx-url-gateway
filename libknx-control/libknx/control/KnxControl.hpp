#ifndef KNXCONTROL_HPP_
#define KNXCONTROL_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h> 
#include <iomanip>//for send frame

class knxControl{
    private:
        const std::vector<std::string> kDatapointtypes{"1","2","3","4","5","6","7","8","9","10", "11","12","13","14","15","16","17","18"};
        const std::vector<uint8_t> kSa{0x00,0x00}; //frame source address  
        const std::vector<uint8_t> kBool{0x80,0x81}; //boolean values
        const std::string kSerial{"/dev/ttyAMA0"};
        uint8_t main_gr_;
        uint8_t midd_gr_;
        uint8_t addr_;  
        uint8_t value_;
        uint8_t control_byte_{0x73};
        uint8_t chksum_{0x00};
        std::vector<uint8_t> ga_{0xFF,0xFF,0xFF};
        int dpt_;
        //frame for link layer off:
        std::vector <uint8_t> lloff_{kFt1_2[0],0x09,0x09,kFt1_2[0],0xFF,0xF6,0x00,0x08,0x01,0x34,0x10,0x01,0xF0,0xFF,kFt1_2[1]};
        uint8_t length_;
        //Data.req frame
        const std::vector<uint8_t> kFt1_2{0x68,0x16}; //start and end byte of FT1.2 protocol
        std::vector <uint8_t> header_{kFt1_2[0],0xFF,0xFF,kFt1_2[0],0xFF};
        std::vector <uint8_t> tail_{0xFF,kFt1_2[1]};
        std::vector <uint8_t> body_{0x11,0x00,0xBC,0xE0,kSa[0],kSa[1],0xFF,0xFF,0x01,0x00,0xFF};
        std::vector <uint8_t> frame_; //hole Data.req frame
        
        //frame for link layer on:
        const std::vector <uint8_t> kLlon{kFt1_2[0],0x09,0x09,kFt1_2[0],0x73,0xF6,0x00,0x08,0x01,0x34,0x10,0x01,0x00,0xB7,kFt1_2[1]};

    public:
    //KNX MANAGEMENT CLASS
        uint8_t GetMainGr()const{return main_gr_;}
        uint8_t GetMiddGr()const{return midd_gr_;}
        uint8_t GetAddr()const{return addr_;}
        uint8_t GetValue()const{return value_;}
        std::vector<uint8_t> GetGa()const{return ga_;}
        int GetDpt()const{return dpt_;}
        bool SetData(std::vector<std::string>req_str, std::string dpt_str); //converts data strings
   
        bool SendFrame(); //calculates frame and sends it to the serial interface to the KNX bus  
};
#endif
