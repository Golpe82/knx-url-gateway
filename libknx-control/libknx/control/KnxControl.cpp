#include "./KnxControl.hpp"

static const float min_value = -671088.64;
static const float max_value = 670760.96;
static const std::vector<uint8_t> invalid_float{0x7F, 0xFF};


void knxControl::ConvertStringToKNXString(std::string value_string, std::vector<char> knx_char_array) 
{
    // cut input string length to 14
    value_string.resize(14);

	  cout<<"String to KNXString conversion:\n";
    for (int x = 0; x < sizeof(value_string); x++) { 
        knx_char_array[x] = value_string[x]; 
    } 
}

std::vector<uint8_t> knxControl::ConvertNumberToKNXFloat(float number)
{
  bool number_is_invalid = (number <= min_value) || (number >= max_value);

  if (number_is_invalid)
  {
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    std::cout << "Number out of rage. Number " << number << std::endl;
    return invalid_float;
  }

  number = (number * 100.0);
  uint16_t number_i = (uint16_t)number;
  uint16_t sign = 0;

  if (number < 0)
  {
    sign = 1;
    number_i = abs(number_i);
  }

  uint16_t mantisse = number_i;
  uint16_t exponent = 0;

  while (mantisse > 2047)
  {
    mantisse >>= 1;
    exponent++;
  }

  if (sign == 1)
  {
    uint16_t complement = ~mantisse + 1;
    complement &= (uint16_t)0x7ff;
    mantisse = complement;
  }
  uint16_t knx_float = (sign << 15) | (exponent << 11) | mantisse;
  uint8_t mso = (uint8_t)(knx_float >> 8);
  uint8_t lso = (uint8_t)(knx_float & 0xFF);

  return {mso, lso};
}

uint8_t knxControl::GetDatapointType(std::string str)
{
  std::string dpt_str = str.substr(0, str.size() - 1);
  std::string dpt_full_str = dpt_str.substr(dpt_str.find("-") + 1);
  std::cout << "Datapoint type string: " << dpt_full_str << std::endl;
  std::string dpt = "0";
  bool is_dpt = str.find("DPT-") != std::string::npos;   // Datapoit type
  bool is_dpst = str.find("DPST-") != std::string::npos; // Datapoint subtype

  if (is_dpst)
  {
    int pos = dpt_full_str.find("-");

    if (pos != std::string::npos)
    {
      dpt = dpt_full_str.substr(0, pos);
      std::cout << "Datapoint type: " << dpt << std::endl;
      std::string dpst = dpt_full_str.substr(pos + 1);
      std::cout << "Datapoint subtype: " << dpst << std::endl;
    }
    else
    {
      std::cout << "Wrong string for datapoint type " << dpt_full_str << std::endl;
    }
  }
  else if (is_dpt)
  {
    int pos = dpt_full_str.find("-");
    if (pos == std::string::npos)
    {
      dpt = str.substr(str.find("-") + 1);
      std::cout << "DPT: " << dpt << std::endl;
    }
    else
    {
      std::cout << "Wrong string for datapoint type " << dpt_full_str << std::endl;
    }
  }
  else
  {
    std::cout << "No DPT or DPST found" << std::endl;
  }

  if (!is_valid_dpt(dpt))
  {
    std::cout << "Datapoint type not in dpt constants " << dpt << std::endl;
  }

  return (uint8_t)std::stoi(dpt);
}

bool knxControl::is_valid_dpt(std::string dpt)
{
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

void knxControl::SetData(std::vector<std::string> req_str, std::string dpt_str)
{
  SetGroupaddress(req_str[0], req_str[1], req_str[2]);
  SetValue(req_str[3]);
  dpt_ = GetDatapointType(dpt_str);
}

void knxControl::SetGroupaddress(std::string main_group, std::string mid_group, std::string address)
{
    main_gr_ = (uint8_t)std::stoi(main_group);
    midd_gr_ = (uint8_t)std::stoi(mid_group);
    addr_ = (uint8_t)std::stoi(address);
    ga_ = {main_gr_, midd_gr_, addr_};
}

void knxControl::SetValue(std::string requested_value)
{
    if (requested_value == "-an")
      value_[0] = kBool[1];
    else if (requested_value == "-aus")
      value_[0] = kBool[0];
    else if (requested_value == "-minus")
      value_[0] = kDimm[1];
    else if (requested_value == "-plus")
      value_[0] = kDimm[0];
    else if (requested_value.find("-send_celsius=") != std::string::npos)
    {
      std::string value_string = requested_value.substr(requested_value.find("=") + 1);
      std::cout << "Setting celsius value: " << value_string << std::endl;
      value_.resize(2); //+ 2 octets for DPT9
      float value_float = std::stof(value_string);
      std::vector<uint8_t> knx_float = ConvertNumberToKNXFloat(value_float);
      value_[0] = knx_float[0];
      value_[1] = knx_float[1];
    }
    else if (requested_value.find("-contact=") != std::string::npos)
    {
      std::string value_string = requested_value.substr(requested_value.find("=") + 1);
      std::cout << "SIP number value: " << value_string << std::endl;
      // std::vector<uint8_t>  value is an int type not 8bit Byte??
      value_.resize(14); //+ 14 octets for DPT14

      // std::vector<uint8_t>  value is an int type not 8bit Byte??
      std::vector<char> knx_char_array(14);
      ConvertStringToKNXString(value_string, knx_char_array);
      // copy KNX 14 x char into value array
      value_ = knx_char_array;
    }
    else
      std::cout << "invalid value" << std::endl;
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

  if (dpt_ == 1 || dpt_ == 3)
    body_[10] = value_[0];

  else if (dpt_ == 9)
  {
    body_[10] = kBool[0];
    body_.resize(13); // 2 octets more for DPT9
    body_[8] = 0x03;  // APDU + data
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
    std::cout << std::hex << +frame_[i];
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
    std::cout << std::hex << +frame_[i];
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
