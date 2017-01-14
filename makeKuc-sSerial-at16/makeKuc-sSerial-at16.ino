#include <SoftwareSerial.h>

//define
SoftwareSerial mySerial(5, 6); /* RX:D5, TX:D6 */
const int mVoutPin = 0;
uint32_t mTimerTmp;
uint32_t mReceive_Start=0; 
int mTemp=0;

//=============================================================================//
int DIN_H = 18;
int DIN_L = 19;
// LED Count num
int mLED_MAX = 1;
const int mWait_US= 10;
//=============================================================================//
int mMode=0;
int mColorCt=0;
//define
const int mModeNormal= 0;
const int mModeHigh   =2;
const int mModeLow   =1;
uint32_t mRGB_last =0;
const unsigned char mColorMap[9][3] = {
  {0xff,0xa5,0x00 }, // #ffa500 Orange
  {0xd2,0x69,0x1e }, // #d2691e chocolate
  {0xff,0x00,0x00 }, // #800000 red 
  {0x80,0x00,0x80 }, // #800080 purple
  {0x00,0xff,0x00 }, // #008000 green
  {0xff,0xff,0x00 }, // #ffff00 Yellow
  {0x00,0x00,0xff }, // #000080 
  {0x00,0xff,0xff }, // #00ffff aqua
  {0x40,0x40,0x40 }, // #404040 ｗhite
  //{0x80,0x80,0x00 }, // #808000 olieve
};
//
void setup() { 
  Serial.begin(9600);
  mySerial.begin( 9600 );
  Serial.println("# Start-setup"); 
  pinMode(mVoutPin ,INPUT );             
  pinMode(DIN_H, OUTPUT);     
  pinMode(DIN_L, OUTPUT);     
  LED_Init();
}

/*
void proc_rgbDisplay(){
for(int n=0;n<LED_MAX;n++)
  LED_Color_RGB(0xf0,0x00,0x00);
LED_Set();
delay(800); 
for(int n=0;n<LED_MAX;n++)
  LED_Color_RGB(0x00,0xf0,0x00);
LED_Set();
  delay(800); 
for(int n=0;n<LED_MAX;n++)
  LED_Color_RGB(0x00,0x00,0xf0);
LED_Set();
  delay(800);
}
*/

//=========================================================//
// init
//=========================================================//
void LED_Init() {
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delay(10); 
}
//=========================================================//
// set
//=========================================================//
void LED_Set() {
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delay(1); 
}

//=========================================================//
// H(1)
//=========================================================//
void LED_Hi_Bit() {
  digitalWrite(DIN_H, HIGH);
  digitalWrite(DIN_L, HIGH);  
  delayMicroseconds(mWait_US );
 // delayMicroseconds(1);
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delayMicroseconds(mWait_US );
}
//=========================================================//
// L(0)
//=========================================================//
void LED_Low_Bit() {
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, LOW);
  delayMicroseconds(mWait_US );
  digitalWrite(DIN_H, LOW);
  digitalWrite(DIN_L, HIGH);
  delayMicroseconds(mWait_US );
}
//
void LED_Color_RGB(byte led_r, byte led_g, byte led_b) {
  for (int k = 0; k <= 7; k++) {  //青
    if ((bitRead(led_b, 8 - k)) == 1) {
      LED_Hi_Bit();
    }
    else {
      LED_Low_Bit();
    }
  }
  for (int k = 0; k <= 7; k++) {  //緑
    if ((bitRead(led_g, 8 - k)) == 1) {
      LED_Hi_Bit();
    }
    else {
      LED_Low_Bit();
    }
  }
  for (int k = 0; k <= 7; k++) {  //赤
    if ((bitRead(led_r, 8 - k)) == 1) {
      LED_Hi_Bit();
    }
    else {
      LED_Low_Bit();
    }
  }
}
//
long convert_Map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// reading LM60BIZ
int getTempNum(){
  int iRet=0;
  float fSen  = 0;
  unsigned long reading  = 0;  
  int iDiv=3; 
  for (int i=0; i< iDiv; i++) {
    int  iTmp = analogRead(mVoutPin);
//Serial.print("get.iTmp=");
//Serial.println(iTmp);
    reading  += iTmp; 
    //delay(100);
  }
  int SValue= reading / iDiv;
  int voltage=convert_Map(SValue, 0, 1000, 0,3300);  // V
//Serial.print("SValue=");
//Serial.print(SValue);
//Serial.print(" , voltage=");
//Serial.println(voltage);
  int iTemp = (voltage - 424) / 6.25; //電圧値を温度に変換, offset=425
  int adjust= -3;
  iRet= iTemp; 
  iRet = iRet + adjust;  //adjust-value.
  return iRet;  
}
//
void proc_display(int iCt, int iMode){
    if(iMode==mModeNormal){
        for(int n=0;n< mLED_MAX;n++){
          LED_Color_RGB(mColorMap[iCt][0], mColorMap[iCt][1], mColorMap[iCt][2] );
          delay(1800);
        }
        LED_Set(); 
        for(int n=0;n< mLED_MAX;n++){
          LED_Color_RGB(0x00, 0x00, 0x00);
        }
        LED_Set();
        //delay(400);       
    } else if(iMode==mModeHigh){
        //HIGH
        for(int j=0; j < 10; j++){
            for(int n=0;n< mLED_MAX;n++){
              LED_Color_RGB(0x00, 0x00, 0x00);
            }
            LED_Set();
            delay(100);            
            for(int n=0;n< mLED_MAX;n++){
              LED_Color_RGB(mColorMap[iCt][0], mColorMap[iCt][1], mColorMap[iCt][2] );
              delay(100);
            }
            LED_Set();
        }
        for(int n=0;n< mLED_MAX;n++){
          LED_Color_RGB(0x00, 0x00, 0x00);
        }
        LED_Set();
    }else if(iMode==mModeLow){
        for(int j=0; j < 2; j++){
            for(int n=0;n< mLED_MAX;n++){
              LED_Color_RGB(0x00, 0x00, 0x00);
            }
            LED_Set();
            delay(200);            
            for(int n=0;n< mLED_MAX;n++){
              LED_Color_RGB(mColorMap[iCt][0], mColorMap[iCt][1], mColorMap[iCt][2] );
              delay(800);
            }
            LED_Set();
        }
        for(int n=0;n< mLED_MAX;n++){
          LED_Color_RGB(0x00, 0x00, 0x00);
        }
        LED_Set();
    }
    
    mMode=mMode+1 ;
    if(mMode > 2 ) {mMode=0; }
}

//
void loop() {
  //delay( 100 );
  if (millis() > mTimerTmp) {
      mTimerTmp = millis()+ 500;
      int itemp= getTempNum();
      String sTemp=String(itemp);
      //sSerial
     int iD1=int(itemp );
     char cD1[8+1];
     sprintf(cD1 , "d1%06d", iD1);       
     mySerial.print( cD1 );
Serial.print("cD1=");
Serial.println(cD1  );      
  }
  //proc_rgbDisplay(); 
  if (millis() > mRGB_last) {
      proc_display(mColorCt, mMode);
      mRGB_last = millis()+ 4000;
      mColorCt    =mColorCt +1;
      if(mColorCt >=9 ){ mColorCt=0; }
  }
}





