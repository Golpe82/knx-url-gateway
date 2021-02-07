#include "./KnxControl.hpp"

bool knxControl::SetData(std::vector<std::string>req_str, std::string dpt_str){
  bool not_setted= true;
  std::cout << req_str[0] << std::endl;
  std::cout << req_str[1] << std::endl;
  std::cout << req_str[2] << std::endl;
  std::cout << req_str[3] << std::endl;
  if(not_setted){
    //set value
    if(req_str[3]=="-an") value_= kBool[1];
    else if(req_str[3]=="-aus") value_=kBool[0];
    else if(req_str[3]=="-minus") value_=kDimm[1];
    else if(req_str[3]=="-plus") value_=kDimm[0];
    //set groupaddress
    main_gr_= (uint8_t)stoi(req_str[0]);
    midd_gr_= (uint8_t)stoi(req_str[1]);
    addr_= (uint8_t)stoi(req_str[2]);
    ga_= {main_gr_, midd_gr_, addr_};
    //set datapointtype
    int count=0;
    size_t pos=std::string::npos;
    do{
      pos=dpt_str.find(kDatapointtypes[count],0);
      count++;
    }while((pos==std::string::npos)&&(count<kDatapointtypes.size()));
    dpt_=stoi(kDatapointtypes[--count]);
    not_setted= false;
  }
  return not_setted;
}

bool knxControl::SendFrame(){
  std::ofstream telegram(kSerial, std::ofstream::out);
  for(int i=0;i<kLlon.size();i++) {telegram<<kLlon[i]; std::cout<< std::hex << +kLlon[i];}
  std::cout<<std::endl;
  body_[10] = value_;
  body_[6] = (ga_[0]<<3)|ga_[1]; //destination high byte conform to knx standard
  body_[7] = ga_[2]; //destination low byte
  length_= header_[1]=header_[2]=body_.size()+1;
  header_[4]=control_byte_^=1<<5; //toggle control byte
  //calculate and set checksumm
  chksum_=0x00;
  for(int i=0;i<body_.size();i++) chksum_+=body_[i];
  tail_[0]=chksum_+=control_byte_;
  //build frame
  frame_.clear();
  frame_.reserve(header_.size()+body_.size()+tail_.size());
  frame_.insert(frame_.end(), header_.begin(),header_.end());
  frame_.insert(frame_.end(), body_.begin(),body_.end());
  frame_.insert(frame_.end(),tail_.begin(),tail_.end());
  //and send it to the knx bus
  for(int i=0;i<frame_.size();i++) {telegram<<frame_[i];std::cout<<std::hex << +frame_[i];}
  std::cout<<std::endl;

  //repeat last sended frame to make next frame even (link layer off)
  header_[4]=control_byte_^=1<<5;
  chksum_=0x00;
  for(int i=0;i<body_.size();i++) chksum_+=body_[i];
  tail_[0]=chksum_+=control_byte_;
  frame_.clear();
  frame_.insert(frame_.end(), header_.begin(),header_.end());
  frame_.insert(frame_.end(), body_.begin(),body_.end());
  frame_.insert(frame_.end(),tail_.begin(),tail_.end());
  for(int i=0;i<frame_.size();i++) telegram<<frame_[i];
  lloff_[4]=control_byte_^=1<<5;
  chksum_=0x00;
  for(int i=0;i<lloff_.size();i++) chksum_+=lloff_[i];
  lloff_[13]=chksum_+=control_byte_;
  for(int i=0;i<frame_.size();i++) {telegram<<frame_[i]; std::cout<<std::hex << +lloff_[i];}
  std::cout<<std::endl;
  //if(!telegram.good()){
    std::cout<<"Goodbit="<<telegram.good();
    std::cout<<"Eofbit="<<telegram.eof();
    std::cout<<"Failbit="<<telegram.fail();
    std::cout<<"Badbit="<<telegram.bad();
    std::cout<<std::endl;
  //}
  return telegram.good();
}
