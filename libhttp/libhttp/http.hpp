#ifndef HTTP_HPP_
#define HTTP_HPP_

#include "../thirdparty/httplib.h"
#include <iostream>
#include <string>

class http{
    private:
        std::string maingroup_str_;
        std::string middlegroup_str_;
        std::string address_str_;
        std::string value_str_;
        std::string groupaddress_str_;

    public:   
        std::string GetMaingroupStr()const{return maingroup_str_;}
        std::string GetMiddlegroupStr()const{return middlegroup_str_;}
        std::string GetAddressStr()const{return address_str_;}
        std::string GetValueStr()const{return value_str_;}
        std::string GetGroupaddressStr()const{return groupaddress_str_;}
        std::vector <std::string> GetRequestStr()const{return std::vector <std::string>{maingroup_str_, middlegroup_str_, address_str_, value_str_};};
        bool SetDataStr(std::smatch match);
        void print(int n);
};
#endif
