#include "FingerPrint.h"

FingerPrint::FingerPrint(int m_tx, int m_rx){
  this->module = new SoftwareSerial(m_tx, m_rx);
  this->module->begin(57600);
}


int FingerPrint::getImg(){
   //Serial.print("start");
   byte cmd[] = {0x01};
   this->sendCommand(cmd, 1);
   delay(200);
   this->checkForAck();
   int i=0;
   while (this->lastOperationCode() !=0) {
      //Serial.print(this->lastOperationCode(), HEX);
      this->sendCommand(cmd, 1);
      delay(500);
      this->checkForAck();
      i++;
   } 
   return this->lastOperationCode();
}

int FingerPrint::img2Tz(byte charBufferId){
  byte cmd[2] = {0x02, charBufferId};
  //Serial.print(cmd[1], HEX);
  this->sendCommand(cmd, 2);
  //Serial.print(cmd[0], HEX);
  
  delay(800);
  if(this->checkForAck()==1){
      return lastOperationCode();
  } else return -1;
}

int FingerPrint::regModel() {
  byte cmd[] = {0x05};
  this->sendCommand(cmd, 1);
  delay(1000);
  if(this->checkForAck()==1){
     return lastOperationCode();
  } else return -1;
}

int FingerPrint::store(int locationId) {
  byte upperByte = locationId&0xFF008;
  byte lowerByte = locationId&0x00FF;
  upperByte<<=upperByte;
  byte cmd[] = {0x06, 0x01, upperByte, lowerByte};
  //byte cmd[] = {0x06, 0x01, 0x00, locationId};
  this->sendCommand(cmd, 4);
  delay(1000);
  if(this->checkForAck()==1){
     return lastOperationCode();
  } else return -1;
}

int FingerPrint::search(int *id) {
  byte cmd[] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x0A3};
  this->sendCommand(cmd, 6); 
  delay(1000);
  if (this->checkForAckAndId()==1) {
    *id = lastReturnedId();
    return lastOperationCode();
  } else return -1;
}

int FingerPrint::empty() {
  byte cmd[] = {0x0D};
  this->sendCommand(cmd, 1);
  delay(500);
  if(this->checkForAck()==1){
     return lastOperationCode();
  } else return -1;
}

int FingerPrint::upChar(byte bufferId) {
  byte cmd[] = {0x08, bufferId};
  //2byte cmd[] = {0x0A};
  this->sendCommand(cmd, 2);
  delay(1000);
  if (this->checkForAck() == 1) {
    int returnCode = lastOperationCode();
    if (returnCode == 0){
      checkForData();
    return returnCode;
  } else {
    return -1;
  }
  }
 }

 int FingerPrint::downChar() {
     byte cmd[] = {0x09, 0x01};
     this->sendCommand(cmd, 2);
     delay(500);
     if (this->checkForAck() == 1) {
        int returnCode = lastOperationCode();
        if (returnCode == 0) {
          for(int i=0; i<268; i++) {
            this->module->write(tempData[i]);
          }
        }
        return returnCode;
     } else return -1;
 }

//low level operation
int FingerPrint::sendCommand(byte *command, byte len) {
  byte x=0;
  byte pLength = len+2; // FingerPrint::getPacketLength(len);
  //header
  this->sendHeader();

  //packet ID for command
  this->module->write(0x01);

  //length
  this->module->write(x);
  this->module->write(pLength);

  //Serial.prin
  //Set(x);rial.print(pLength);

  //command
  for (int i=0; i<len; i++) {
    this->module->write(command[i]);
    //Serial.print(command[i]);
  }

  //send checksum
  int cs = FingerPrint::getChecksum(0x01, pLength, command, len);
  if (cs < 0xFF) {
    this->module->write(x);
    //Serial.print(x);
  }
  this->module->write(cs);
  //Serial.print(cs);
  //Serial.print("\n");
}

int FingerPrint::lastOperationPacketId() {
  return this->tempData[6];
}

int FingerPrint::lastOperationCode(){
  return this->tempData[9];
}

int FingerPrint::lastReturnedId() {
  /*int upper = tempData[10];
  int lower = tempData[11];
  int id = (upper<<8) | (lower&0x00ff);*/
  return tempData[11];
}

int FingerPrint::sendHeader() {
  //send header
  this->module->write(0xEF);
  this->module->write(0x01);

  //send address
  this->module->write(0xFF);
  this->module->write(0xFF);
  this->module->write(0xFF);
  this->module->write(0xFF);
}

int FingerPrint::checkForAck(){
  if (this->module->available()){
    delay(10);
      this->module->readBytes(this->tempData, 12);
      return 1;
  } else {
    return -1;
  }
}

int FingerPrint::checkForAckAndId() {
  if (this->module->available()){
    delay(10);
      this->module->readBytes(this->tempData, 16);
      return 1;
  } else {
    return -1;
  }
}

int FingerPrint::checkForData(){
  if (this->module->available()){
    delay(10);
      //this->module->readBytes(this->tempData, 268);
      /*for (int i=0; i<268; i++) {
        this->tempData[i] = this->module->read();
      } */

      int i = 0;
      while(this->module->available()) {
        this->tempData[i] = this->module->read();
        i++;
      }
      delay(1000);
      while(this->module->available()) {
        this->tempData[i] = this->module->read();
        i++;
      } 
      this->dataCount = i;
      return 1;
  } else {
    return -1;
  }
}

//utilities
byte FingerPrint::getPacketLength(int contentLength){
  return contentLength+2;
}

int FingerPrint::getChecksum(byte pId, byte packageLength, byte content[], byte contentLength) {
  int sum = pId + packageLength + FingerPrint::getSum(content, contentLength);
  return sum;
}
int FingerPrint::getSum(byte *content, int size) {
  int sum = 0;
  for (int i=0; i<size; i++)
  {
    sum += content[i];
  } 
  return  sum;
}
