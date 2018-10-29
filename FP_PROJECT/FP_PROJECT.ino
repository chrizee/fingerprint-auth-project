#include "FingerPrint.h"

FingerPrint fp(8, 7);

void setup() {
   //fp.getImg(2);
   Serial.begin(9600);
   //byte x[] = {2, 2, 2, 3, 8, 10, 8};
   //Serial.print(FingerPrint::getSum(x, sizeof(x)));
   /*Serial.print(FingerPrint::getSum(x, sizeof(x)), HEX);
   Serial.print("\n");
   Serial.print(FingerPrint::getPacketLength(sizeof(x))); */
}

void loop() {
  if (Serial.available()){
    char in = Serial.read();
    if (in == 'R'){
      //getTemplate();
      delay(10);
      int in1 = Serial.read()-48;
      int in2 = Serial.read()-48;
      int in3 = Serial.read()-48;
      int id = (in1*100)+(in2*10)+in3;
     // Serial.print(id);   //debug
      performRegister2(id);
    } else if (in == 'V'){
      verify();
    } else if (in == 'D') {
      upChar();
    } else if (in == 'E') {
      emptyModule();
    }
  }
}

//test functions
void getTemplate() {
  Serial.println(fp.getImg(), HEX);
  Serial.println("tz1");
  Serial.println(fp.img2Tz(1), HEX);
  Serial.println("/tz1");

  delay(200);
  
  Serial.println(fp.getImg(), HEX);
  Serial.println("tz2");
  Serial.println(fp.img2Tz(2), HEX);
  Serial.println("/tz2");

  delay(500); 
  
  Serial.println("rg");
  int r = fp.regModel();
  Serial.println(r, HEX);
  Serial.println("/rg"); 
  
  delay(500); 
  Serial.println("store");
  int st = fp.store(2);
  Serial.println(st, HEX);
  Serial.println("/store");

  /*int k = 5;
  while (k>0) {
    Serial.println("");
    for (int i=0; i<265; i++) {
      Serial.print(fp.tempData[i], HEX);
    }
    k--;
  } */
  delay(2000);
}

void search() {
  Serial.println(fp.getImg(), HEX);
  Serial.println("tz1");
  Serial.println(fp.img2Tz(1), HEX);
  Serial.println("/tz1");

  delay(200);
  
  Serial.println(fp.getImg(), HEX);
  Serial.println("tz2");
  Serial.println(fp.img2Tz(2), HEX);
  Serial.println("/tz2");

  delay(500); 
  
  Serial.println("rg");
  int r = fp.regModel();
  Serial.println(r, HEX);
  Serial.println("/rg"); 

  Serial.println("search");
  int id;
  Serial.println(fp.search(&id));
  Serial.println(id);
  Serial.println("/search");
}

void upChar() {
  Serial.println(fp.getImg(), HEX);
  Serial.println("tz1");
  Serial.println(fp.img2Tz(1), HEX);
  Serial.println("/tz1");

  delay(200);
  
  Serial.println(fp.getImg(), HEX);
  Serial.println("tz2");
  Serial.println(fp.img2Tz(2), HEX);
  Serial.println("/tz2"); 

  Serial.println("upChar");
  byte data[256];
  Serial.println(fp.upChar(0x02));
  for (int i=0; i<fp.dataCount; i++) Serial.print(fp.tempData[i],HEX);
  Serial.println();
  //for (int i=0; i<fp.dataCount; i++) Serial.print(data[i],HEX);
  Serial.println();
  Serial.println(fp.tempData[7]);
  Serial.println(fp.tempData[8]);
  Serial.println(fp.dataCount);
  Serial.println("/upChar");
}

void empty() {
  Serial.print("Empty");
  Serial.print(fp.empty());
  Serial.print("/Empty");
}

//real functions

void performRegister() {
   if (errorOccurred(fp.getImg(), "G1") !=0 ) return;
   if (errorOccurred(fp.img2Tz(1), "C1") !=0 ) return;

   if (errorOccurred(fp.getImg(), "G2") !=0 ) return;
   if (errorOccurred(fp.img2Tz(2), "C2") !=0 ) return;

   if (errorOccurred(fp.regModel(), "R") !=0) return;
   if (errorOccurred(fp.upChar(0x01), "U") !=0) return;

   //Serial.write(fp.tempData, 268);
    for (int i=0; i<fp.dataCount; i++) Serial.print(fp.tempData[i],HEX);
}

void performRegister2(int id) {
   if (errorOccurred(fp.getImg(), "G1") !=0 ) return;
   if (errorOccurred(fp.img2Tz(1), "C1") !=0 ) return;

   if (errorOccurred(fp.getImg(), "G2") !=0 ) return;
   if (errorOccurred(fp.img2Tz(2), "C2") !=0 ) return;

   if (errorOccurred(fp.regModel(), "R") !=0) return;
   if (errorOccurred(fp.store(id), "S") !=0) return;
   //Serial.print(id);
   sendFormatedResult(id);
}

void emptyModule() {
    fp.empty();
    Serial.print('A');
}

void receiveFromUpperComputer() {
    int i = 0; 
    while(Serial.available()) {
      fp.tempData[i] = Serial.read();
    }
}

void uploadToModule(){
    if (errorOccurred(fp.downChar(), "D") !=0) return;
}

void verify() {
    if (errorOccurred(fp.getImg(), "G1") !=0 ) return;
    if (errorOccurred(fp.img2Tz(1), "C1") !=0 ) return;

    /*if (errorOccurred(fp.getImg(), "G2") !=0 ) return;
    if (errorOccurred(fp.img2Tz(2), "C2") !=0 ) return;

    int r = fp.regModel();
    if (errorOccurred(r, "R") !=0){
      //Serial.print(r);
      return;
    } */
    //if (errorOccurred(fp.upChar(0x01), "U") !=0) return;
    int id;
    if(errorOccurred(fp.search(&id), "S") !=0 ) return;
    //Serial.print(id);
    sendFormatedResult(id);
}

void sendFormatedResult(int no) {
  if (no<10) {
    Serial.print("00");
  } else if (no<100) {
    Serial.print("0");
  } 
  Serial.print(no);
}

int errorOccurred(int status, String errorSym) {
  if (status == 0x00) {
    return 0;
  }else if (status == -1) {
    Serial.print("E"+errorSym+"0");
    return -1;
  } else if (status == 1) {
    Serial.print("E"+errorSym+"1");
    return -1;
  }else {
    Serial.print("E"+errorSym);
    Serial.print(status, HEX);
    return -1;
  }
}

