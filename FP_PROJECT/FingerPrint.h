#ifndef FINGER_PRINT
#define FINGER_PRINT 4

#include <Arduino.h>
#include <SoftwareSerial.h>

class FingerPrint {
  public:
  int dataCount = 0;
   FingerPrint(int m_tx, int m_rx);
   //high level operations
   int getImg();
   int img2Tz(byte charBufferId);
   int regModel();
   int store(int locationId);
   int search(int*);
   int empty();

   int downChar();
   int upChar(byte bufferId);

   //low level operations
   int sendHeader();
   int sendCommand(byte *command, byte size);
   int sendData(byte *data);
   int lastOperationPacketId();
   int lastReturnedId();
   int lastOperationCode();

   int checkForAck();
   int checkForAckAndId();
   int checkForData();

   //utilities
   static byte getPacketLength(int contentLength);
   static int getChecksum(byte pId, byte packageLength, byte content[], byte contentLength);
   static int getSum(byte *content, int size);

   byte tempData[20];
  private:
   byte data[2];

   
   
   byte header[2] = {0xEF, 0x01};
   byte address[4] = {0xFF, 0xFF, 0xFF, 0xFF};
   
   SoftwareSerial* module;
};

#endif

