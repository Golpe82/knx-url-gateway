#include "./KnxControl.hpp"

static const float min_value = -671088.64;
static const float max_value = 670760.96;
static const std::vector<uint8_t> invalid_float{0x7F, 0xFF};

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

void knxControl::SetValueToKNXString(std::string value_string)
{
  std::uint8_t dpt_length = 0x0E; // DPT-16
  std::vector<char> knx_string(dpt_length);
  if (value_string.size() < dpt_length)
    value_string.resize(dpt_length);

  for (int i = value_string.length(), x = dpt_length; x >= 0; i--, x--)
  {
    knx_string[x] = value_string[i];
  };

  value_.resize(dpt_length);
  std::cout << "KNX string: ";

  for (int i = 0; i <= knx_string.size(); i++)
  {
    value_[i] = knx_string[i];
    std::cout << value_[i];
  }
  std::cout << std::endl;
}

uint8_t knxControl::GetDatapointType(std::string str)
{
  std::string dpt_str = str.substr(0, str.size() - 1);
  std::string dpt_full_str = dpt_str.substr(dpt_str.find("-") + 1);
  std::string dpt = "0";
  bool is_dpt = str.find("DPT-") != std::string::npos;   // Datapoit type
  bool is_dpst = str.find("DPST-") != std::string::npos; // Datapoint subtype

  if (is_dpst)
  {
    int pos = dpt_full_str.find("-");

    if (pos != std::string::npos)
    {
      dpt = dpt_full_str.substr(0, pos);
      std::string dpst = dpt_full_str.substr(pos + 1);
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

  std::cout << "Datapoint type: " << dpt << std::endl;

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
  // TODO: refactor -send_celsius like -text. SetValueToKNXFloat
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
  else if (requested_value.find("-text=") != std::string::npos)
  {
    std::string value_string = requested_value.substr(requested_value.find("=") + 1);
    std::cout << "Requested text: " << value_string << std::endl;
    SetValueToKNXString(value_string);
  }
  else
    std::cout << "invalid value" << std::endl;
}

bool knxControl::SendFrame()
{
  std::ofstream telegram_stream(kSerial, std::ofstream::out);

  for (int i = 0; i < ft12_reset_telegram.size(); i++)
  {
    telegram_stream << ft12_reset_telegram[i];
    std::cout << " " << std::hex << +ft12_reset_telegram[i];
  }
  std::cout << " FT1.2 reseted" << std::endl;

  for (int i = 0; i < kLlon.size(); i++)
  {
    telegram_stream << kLlon[i];
    std::cout << " " << std::hex << +kLlon[i];
  }
  std::cout << " Link layer on" << std::endl;

  // set data bytes depending on  dpt
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
  else if (dpt_ == 16)
  {
    body_[10] = kBool[0];
    body_.resize(25);             // 14 octets more for DPT16
    body_[8] = 1 + value_.size(); // APDU + data

    for (int i = 0, octet = 11; i <= value_.size(); i++, octet++)
    {
      body_[octet] = value_[i];
    }
  }

  // calculate destination groupaddress bytes
  body_[6] = (ga_[0] << 3) | ga_[1]; // destination high byte
  body_[7] = ga_[2];                 // destination low byte
  length_ = header_[1] = header_[2] = body_.size() + 1;
  header_[4] = control_byte_ ^= 1 << 5; // toggle control byte

  // calculate and set checksumm
  chksum_ = 0x00;

  for (int i = 0; i < body_.size(); i++)
    chksum_ += body_[i];

  tail_[0] = chksum_ += control_byte_;

  // build frame
  telegram_.clear();
  telegram_.reserve(header_.size() + body_.size() + tail_.size());
  telegram_.insert(telegram_.end(), header_.begin(), header_.end());
  telegram_.insert(telegram_.end(), body_.begin(), body_.end());
  telegram_.insert(telegram_.end(), tail_.begin(), tail_.end());

  // and send it to the knx bus
  for (int i = 0; i < telegram_.size(); i++)
  {
    telegram_stream << telegram_[i];
    std::cout << " " << std::hex << +telegram_[i];
  }
  std::cout << " FT1.2 telegram sent" << std::endl;

  lloff_[4] = control_byte_ ^= 1 << 5;

  for (int i = 0; i < lloff_.size(); i++)
  {
    telegram_stream << lloff_[i];
    std::cout << " " << std::hex << +lloff_[i];
  }
  std::cout << " Link layer switched off" << std::endl;

  return 0;
}
