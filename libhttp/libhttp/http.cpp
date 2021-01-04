#include "./http.hpp"

bool http::SetDataStr(std::smatch match){
  bool not_setted= true;
  if(not_setted){
    value_str_=match[4];
    maingroup_str_=match[1];
    middlegroup_str_=match[2];
    address_str_=match[3]; 
    groupaddress_str_="\""+maingroup_str_+"/"+middlegroup_str_+"/"+address_str_+"\"";
    not_setted= false;
  }
  return not_setted;
}
void http::print(int n){
  switch (n){
    case 1:
      std::cout<<"Searching for groupaddress "<<groupaddress_str_<<std::endl;
      break;
    case 2:
      std::cout<<"Error setting data strings from HTTP-Request"<<std::endl;
      break;
    default:
      std::cout<<"Unknown error"<<std::endl;
      break;
    }
}