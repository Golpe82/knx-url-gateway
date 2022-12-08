#include "./KnxControl.hpp"
// TODO: clean up and refactor this code
///////
// write this + knxfloat to number in a extra module
int ConvertNumberToKNXFloat( float number )
{
  // do range check
  if (number <= -671088.64) {
    return 0xffff;
  }

  if (number >= 670760.96) {
    return 0x7fff;
  }

  number = (number * 100.0);
  int number_i = (int)number;
  std::cout<<number_i<<std::endl;

  int sign = 0;
  int exponent = 0;
  int mantisse = 0;

  if (number < 0) {
    sign = 1;
    number_i = std::abs(number_i);
  }

  mantisse = number_i;

  while (mantisse > 2047) {
    mantisse >>= 1;
    exponent++;
  }

  if (sign == 1)
  {
    std::cout<<"too hard in python  use C"<<std::endl;
    int comp;
    comp = ~mantisse + 1;
    comp &= 0x7ff;
    mantisse = comp;
  }

  return (sign << 15) | (exponent << 11) | mantisse;
}
//////

bool is_valid_dpt(std::string dpt, std::vector<std::string> kDatapointtypes){
    bool is_valid_dpt = false;
    int count = 0;
    size_t pos = std::string::npos;

    do
    {
        pos = kDatapointtypes[count].find(dpt);
        is_valid_dpt = kDatapointtypes[count] == dpt;
        count++;
    } while (count < kDatapointtypes.size() && !is_valid_dpt);

    return is_valid_dpt;
}

int get_dpt (std::string str, std::vector<std::string> datapoint_types) {
    bool is_dpst = str.find("DPST-")!=std::string::npos;
    bool is_dpt = str.find("DPT-")!=std::string::npos;
    std::string dpt_str_full = str.substr(str.find("-") + 1);
    std::cout << "Datapoint type string: "<< dpt_str_full << std::endl;
    std::string dpt = "0";

    if (is_dpst) {
        int pos = dpt_str_full.find("-");
        if (pos != std::string::npos)
        {
            dpt = dpt_str_full.substr(0 , pos);
            std::cout << "DPT: "<< dpt << std::endl;
            std::string dpst = dpt_str_full.substr(pos + 1);
            std::cout << "DPST: "<< dpst << std::endl;
        }
        else {
            std::cout << "Wrong string for datapoint type " << str << std::endl;
        }
    }
    else if (is_dpt)
    {
        int pos = dpt_str_full.find("-");
        if (pos == std::string::npos)
        {
            dpt = str.substr(str.find("-") + 1);
            std::cout << "DPT: "<< dpt << std::endl;
        }
        else {
            std::cout << "Wrong string for datapoint type " << str << std::endl;
        }
    }
    else {
        std::cout << "No DPT or DPST found" << std::endl;
    }
    //check if in constants
    if (!is_valid_dpt(dpt, datapoint_types)){
        std::cout << "Datapoint type not in dpt constants " << dpt << std::endl;
    }

    return stoi(dpt);
}

bool knxControl::SetData(std::vector<std::string> req_str, std::string dpt_str)
{
  bool not_set = true;
  std::cout << req_str[0] << std::endl;
  std::cout << req_str[1] << std::endl;
  std::cout << req_str[2] << std::endl;
  std::cout << req_str[3] << std::endl;
  std::string value_request = req_str[3];

  if (not_set)
  {
    // set value
    if (value_request == "-an")
      value_[0] = kBool[1];
    else if (value_request == "-aus")
      value_[0] = kBool[0];
    else if (value_request == "-minus")
      value_[0] = kDimm[1];
    else if (value_request == "-plus")
      value_[0] = kDimm[0];
    else if (value_request.find("-send_celsius=") != std::string::npos)
    {
      std::string value_string = value_request.substr(value_request.find("=") + 1);
      std::cout << "Setting celsius value: " << value_string << std::endl;
      value_.resize(2); //+ 2 octets for DPT9
      // 27°C
      // value_[0] = 0x0D;
      // value_[1] = 0x46;
      float value_float = std::stof(value_string);
      int knx_float = ConvertNumberToKNXFloat(value_float);
      value_[0] = (knx_float&0xFF00)>>8;
      value_[1] = (knx_float&0x00FF);
    }
    else
      std::cout << "unvalid value" << std::endl;

    // set groupaddress
    main_gr_ = (uint8_t)stoi(req_str[0]);
    midd_gr_ = (uint8_t)stoi(req_str[1]);
    addr_ = (uint8_t)stoi(req_str[2]);
    ga_ = {main_gr_, midd_gr_, addr_};

    dpt_ = get_dpt(dpt_str, kDatapointtypes);
    not_set = false;
  }
  return not_set;
}

bool knxControl::SendFrame()
{
  std::ofstream telegram(kSerial, std::ofstream::out);

  for (int i = 0; i < kLlon.size(); i++)
  {
    telegram << kLlon[i];
    std::cout << std::hex << +kLlon[i];
  }
  std::cout << " Link layer of Kberry switched on" << std::endl;

  if (dpt_== 1 || dpt_==3)
    body_[10] = value_[0];

  else if (dpt_==9)
  {
    body_[10] = kBool[0];
    std::cout << "Expanding frame body " << std::endl;
    body_.resize(13); // 2 octets more for DPT9
    body_[8] = 0x03; // APDU + data
    body_[11] = value_[0];
    body_[12] = value_[1];
  }

  body_[6] = (ga_[0] << 3) | ga_[1]; // destination high byte conform to knx standard
  body_[7] = ga_[2];                 // destination low byte
  length_ = header_[1] = header_[2] = body_.size() + 1;
  header_[4] = control_byte_ ^= 1 << 5; // toggle control byte

  // calculate and set checksumm
  chksum_ = 0x00;

  for (int i = 0; i < body_.size(); i++)
    chksum_ += body_[i];
  
  tail_[0] = chksum_ += control_byte_;

  // build frame
  frame_.clear();
  frame_.reserve(header_.size() + body_.size() + tail_.size());
  frame_.insert(frame_.end(), header_.begin(), header_.end());
  frame_.insert(frame_.end(), body_.begin(), body_.end());
  frame_.insert(frame_.end(), tail_.begin(), tail_.end());

  // and send it to the knx bus
  for (int i = 0; i < frame_.size(); i++)
  {
    telegram << frame_[i];
    std::cout << std::hex << + frame_[i];
  }
  std::cout << " Odd frame sent" << std::endl;

  // repeat last sended frame to make next frame even (link layer off)
  header_[4] = control_byte_ ^= 1 << 5;
  chksum_ = 0x00;

  for (int i = 0; i < body_.size(); i++)
    chksum_ += body_[i];
  
  tail_[0] = chksum_ += control_byte_;
  frame_.clear();
  frame_.insert(frame_.end(), header_.begin(), header_.end());
  frame_.insert(frame_.end(), body_.begin(), body_.end());
  frame_.insert(frame_.end(), tail_.begin(), tail_.end());

  for (int i = 0; i < frame_.size(); i++)
  {
    telegram << frame_[i];
    std::cout << std::hex << + frame_[i];
  }
  std::cout << " Even frame sent" << std::endl;
  
  lloff_[4] = control_byte_ ^= 1 << 5;

  chksum_ = 0x00;

  for (int i = 0; i < lloff_.size(); i++)
    chksum_ += lloff_[i];
  lloff_[13] = chksum_ += control_byte_;

  for (int i = 0; i < frame_.size(); i++)
  {
    telegram << frame_[i];
    std::cout << std::hex << +lloff_[i];
  }
  std::cout << " Link layer of Kberry switched off" << std::endl;

  // if(!telegram.good()){
  std::cout << "Goodbit=" << telegram.good();
  std::cout << "Eofbit=" << telegram.eof();
  std::cout << "Failbit=" << telegram.fail();
  std::cout << "Badbit=" << telegram.bad();
  std::cout << std::endl;
  //}

  return telegram.good();
}
