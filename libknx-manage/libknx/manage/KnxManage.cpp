#include "./KnxManage.hpp"

unsigned char knxManage::ReadLookUp(std::string groupaddress_str){
  std::vector<std::string>row;
  std::string line;
  std::string word;
  bool not_founded = true;

  read_csv_.open(kLookUp);
  if (read_csv_.is_open()) {
    while(std::getline(read_csv_, line)){ // read row and store it
      row.clear();  
      std::stringstream buffer(line); // used for breaking words 
      while (std::getline(buffer, word, ',')) { // read every column data of a row and store it
        row.push_back(word); // add all the column data of a row to a vector 
      } 
      // Look if groupaddress is in this DB-line
      if (row[1] == groupaddress_str) { 
        action_str_= row[0];
        datapointtype_str_=row[5];
        not_founded = false;
      }   
    } 
  }
  read_csv_.close();
  if (not_founded) return 2;  
  else return read_csv_.eof();
}

void knxManage::print(int n){
  std::cout<<std::endl;
  switch (n){
    case 1:   
      std::cout<<"the requested groupaddress is not in your KNX database"<<std::endl;
      break;
    case 2:
      std::cout<<"Error opening/closing KNX database file"<<std::endl;
      break;
    case 3: 
        std::cout<<"File flags after look-up operation:"<<std::endl;
        std::cout<<"Goodbit="<<read_csv_.good()<<std::endl;
        std::cout<<"Eofbit="<<read_csv_.eof()<<std::endl;
        std::cout<<"Failbit="<<read_csv_.fail()<<std::endl;
        std::cout<<"Badbit="<<read_csv_.bad()<<std::endl;
        std::cout<<"Open="<<read_csv_.is_open()<<std::endl;
        std::cout<<std::endl;
      break;
    case 4: 
      std::cout<<"This datapointtype is not implemented"<<std::endl;
      break;
    case 5: 
      std::cout<<"not found"<<std::endl;
      break;
    case 6: 
      std::cout<<"Error by converting data strings"<<std::endl;
      break;
    case 7: 
      std::cout<<"Error by setting data strings"<<std::endl;
      break;
    case 8: 
      std::cout<<"Error by sending frame to the KNX bus"<<std::endl;
      break;
    case 9: 
      std::cout<<"Error building knx frame"<<std::endl;
      break;
    default:
      std::cout<<"nothing to print"<<std::endl;
      break;
  }
}
std::string knxManage::xmlResp(std::string value_str){
  std::string xml;
  if(value_str=="-an"){
    xml= "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>\
    <SnomIPPhoneText>\
    <Title>LED Control Feature</Title>\
    <Prompt>Prompt Text</Prompt>\
    <Text>"+action_str_+" was switched "+value_str+"</Text>\
    <Led number=\"1\">On</Led>";
  }
  else xml= "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?>\
    <SnomIPPhoneText>\
    <Title>LED Control Feature</Title>\
    <Prompt>Prompt Text</Prompt>\
    <Text>"+action_str_+" was switched "+value_str+"</Text>\
    <Led number=\"1\">Off</Led>";
  return xml;
}