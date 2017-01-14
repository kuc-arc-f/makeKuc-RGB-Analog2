/*
makeKuc-sSerial-esp17
Analog+ SoftwareSerial , deepSleep mode
*/
//
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
extern "C" {
#include <user_interface.h>
}

SoftwareSerial mySerial(4, 5); /* RX:D4, TX:D5 */

const char* ssid = "";
const char* password = "";
//host-set
const char* host = "api.thingspeak.com";
String mAPI_KEY="your-KEY";

unsigned long mTimer=0;
int mNextHttp= 60000;

//Struct
struct stParam{
 String dat1;
};
struct stParam mParam;

//
void setup() {
  Serial.begin( 9600 );
  mySerial.begin( 9600 );
  Serial.println("#Start-setup-ssEsp");

  // wifi-start
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  mTimer= millis()+ 5000;
}

//
void set_Struct(String sHd ,String sDat){
      String sTmp= sDat.substring(2,8);
      if(sHd=="d1"){
//        sTmp= sDat.substring(2,8);
        float fNum= sTmp.toFloat();
        mParam.dat1     =  String( fNum );
      }
}
//
boolean Is_validHead(String sHd){
  boolean ret=false;
  if(sHd=="d1"){
    ret= true;
  }else if(sHd=="d2"){
    ret= true;
  }
  return ret;
}
//
void proc_http(String sTemp ){
//Serial.print("connecting to ");
//Serial.println(host);  
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      //String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp +"&field2=" + sHum;        
      String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp ;        
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" + 
        "Connection: close\r\n\r\n");
      delay(10);      
      int iSt=0;
      while(client.available()){
          String line = client.readStringUntil('\r');
Serial.print(line);
      }    
}
String mBuff="";
//
void loop() {
  int iSleepSec= 300;
   if (millis() > 30000 ){
Serial.print("millis.SleepStart.Over_30S: ");
Serial.println(millis() );           
       ESP.deepSleep( iSleepSec * 1000 * 1000);
  }
  delay( 100 );
  while( mySerial.available() ){
     char c= mySerial.read();
     mBuff.concat(c );
    if(  mBuff.length() >= 8 ){
        String sHead= mBuff.substring(0,2);
        boolean bChkHd= Is_validHead( sHead );
        if(bChkHd== true){
          Serial.println( "#Hd="+ sHead );
          set_Struct(sHead, mBuff );
          if (millis() > mTimer ){
            mTimer=mTimer+ mNextHttp;
            proc_http(mParam.dat1   ) ;
            Serial.print("millis.SleepStart: ");
            Serial.println(millis() );            
            ESP.deepSleep( iSleepSec * 1000 * 1000);
          }
//Serial.print("d1=");
Serial.println( "d1="+ mParam.dat1 );
Serial.print("millis.Loop: ");
Serial.println(millis() );            
//Serial.print("mBuff=");
//Serial.println( mBuff );            
        }
        mBuff="";
    }
  }//end_while  
}







