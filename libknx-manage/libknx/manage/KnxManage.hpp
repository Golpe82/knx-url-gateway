#ifndef KNXMANAGE_HPP_
#define KNXMANAGE_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

class knxManage{
    private:
        const std::string kLookUp{"/usr/local/gateway/ga.csv"}; 
        std::ifstream read_csv_;
        std::string action_str_;
        std::string datapointtype_str_;  
    public:
        std::string GetActionStr()const{return action_str_;}
        std::string GetDatapointtypeStr()const {return datapointtype_str_;}
        unsigned char ReadLookUp(std::string groupaddress_str);
        void print(int n);
        std::string xmlResp(std::string value_str);
};
#endif
