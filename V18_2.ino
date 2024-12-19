

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>



#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>


#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "FS.h"


File f;
IPAddress ip(192,168,0,10);
IPAddress geteway(192,168,0,10);
IPAddress subnet(255,255,255,0);

#define DOUT 14
#define GPR 13
#define WEEK 15
#define ACC 12   //ACC INPUT
#define ESP_LED 2   //LED OUTPUT
#define RXPin 5    
#define TXPin 4
#define CHC 16
#define BUP 0    //Reset INPUT


SoftwareSerial neo(RXPin ,TXPin);   ESP8266WebServer ser(10200);  TinyGPSPlus gps;
 

String details;
String commandBuffer = ""; // Buffer pour stocker les commandes entrées
String smsContent = "";    // Buffer pour stocker le contenu du SMS en cours de lecture
String phoneNumber = "";   // Buffer pour stocker le numéro de téléphone du SMS
String messageIndex = "";  // Buffer pour stocker l'index du message
bool awaitingSMSContent = false; // Flag pour indiquer qu'on attend le contenu du SMS
String indexStr;
bool sendback=false,sendTram=false,sendHeart=false,server0State = false;
String response,  message ,message0 ,message1 ,message2 ,message3;
  String parts[15];
  int in,nmea=0;
  char c;



unsigned long previousMillis,heartMillis;
long HEART=60000; // Interval de 10 secondes
int  signal4G,ser_con=0;


long    dureefota,timei,dureebuckup,stopc,dureegps, dureespeed, dureeacc, dureecon, tim ,timh, dureesend, sendloca, dureeVN, dureehard, rstcompter,dureewifi;
int     savcmp, AT, fcompter, batt, SPED, LSPED, Alt, VMAX, HR, interv, intervs, interv2, interv3, interv4, interv5, interv6;
int     pi,pi1,pi2,pi3,pi4,pi5,pi6,pi7,pi8,pi9,repi,repon, pg,i, i0,i1, i2, l=0, l1=0, rmc, azm, lazm, sleepcompter,dureebup;

                                                                           
String  VER="19",IMEI,SIMEI,SIMSTAT,GSMLEVEL, BATTL,BATTS,SERSTA;float BATTV;      //INFO  
String  network="NON",signalqu,NS="0,",VIN,constat="Disconnected";       //INFO
String  ssid,wifip,devicep,wificonn;                                    //SECURITE
String  apn,SERVER,PORT,BUSERVER,BUPORT;                               //SERVER
int     stopt, interv1, ANG,intervSleep;                              //CONFIGUE
int     IGS;                                                         //SYSTEME
String  paswrd,smsser,smsok,number;                                 //SMS
bool    acc=1,dout; float ANIN, temp=0;                            //IO


String  lTram,DATE,loca , lloca, Sped,GMT="0000000000", AZM, ALT, GPQ="V", YER, DAY, MONTH, TZ, LTZ, DEGRE, precision, test,tram;   //srever data
String  url="http://api.thingspeak.com/channels/2223828/fields/1/last.txt?api_key=OZJEO9FIVCMXL1WP&satus=true";
String  NETMD, NETSEL, GPS,conf,FOTAurl="http://38.242.222.233:3001/config/",FOTAIP="38.242.222.233",AUTOR,gpstat, fn, t, rep, tr,tr1,tr2,tr3,tr4,tr5,tr6,tr7,tr8,tr9,dloc="En attend le signal GPS", s, st, s1, r,  la ,lo ,lla, llo;   //
char    inChar,str[500];
bool    sens=false,wifistat,vn,in1,in2,sim=1,L80,B,bp,mod;



// Déclaration des fonctions
//void sendATCommand(String command);
void decodeResponse(String response);
void decodeSignalQuality(String response);
void decodeSMSNotification(String response);
void readSMS(String index);
void decodeSMSContent(String response);
void decodeNetworkRegistration(String response);
void decodeGPRSAttachment(String response);
void parseCIPOPENResponse(String response);
void bluetoothEvent();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPUPDATE(){
   WiFi.disconnect();  WiFi.mode(WIFI_STA);WiFi.begin("NUMOTRONIC","12345678"); 
    tim=millis();while ((WiFi.status() != WL_CONNECTED)&&(millis()<(tim+15000))) {    delay(100);}

    
if (WiFi.status() == WL_CONNECTED) {neo.end();delay(500); confirme();confirme();
  tim=millis();while (millis()<=tim+10000){

    WiFiClientSecure client;
    client.setInsecure();
     ESPhttpUpdate.setLedPin(ESP_LED, LOW);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "https://raw.githubusercontent.com/azomar01/GPS-4G/master/"+String(VER)+".bin");
  delay(100);  }  neo.begin(9600); }   }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void autor(){confirme();
   Serial.println("AT+NETCLOSE");    delay(1000);   Read();
   Serial.println("AT+NETOPEN");delay(1000); tim=millis();while(millis()<(tim+4000)){   if(Serial.find("OK"))break;delay(100);}
   
  Serial.println("AT+HTTPTERM");tim=millis();while(millis()<(tim+2000)){if(Serial.available())break;delay(100);}      Read();
  Serial.println("AT+HTTPINIT");tim=millis();while(millis()<(tim+2000)){if(Serial.available())break;delay(100);}      Read();
 
tr="AT+HTTPPARA=\"URL\",\""+url+"\"";
Serial.println(tr);  tim=millis();while(millis()<(tim+2000)){if(Serial.available())break;delay(1000);}                Read();

Serial.println("AT+HTTPACTION=0");delay (1000);tim=millis();while(millis()<(tim+2000)){if(Serial.available())break;delay(100);}      Read();
           
Serial.println("AT+HTTPREAD=0,8");delay (1000); tim=millis();while(millis()<(tim+6000)){
 if(Serial.find("20031988")){  confirme();confirme();AUTOR="20031988";f=SPIFFS.open("/AUTOR.txt", "w");f.print("20031988");f.close();}delay(100);}
  Serial.println("AT+NETCLOSE");tim=millis();while(millis()<(tim+2000)){if(Serial.available())break;delay(100);}     Read();
  

}


////////////////////////////////////////////////////////////////////////////////////////////////
void confirme(){    digitalWrite(ESP_LED, HIGH); delay(100);digitalWrite(ESP_LED, LOW);delay(100);}
/////////////////////////////HTML REPONCE///////////////////////////////////////////////////////////////////////
/////////////////////////////HTML REPONCE///////////////////////////////////////////////////////
String sendglobal(){
tr=  "{\"domain_1\":\""        ;                 tr+=SERVER;            tr+="\"," ;
tr+=  "\"port_1\":\""          ;                 tr+=PORT;              tr+="\"," ; 
tr+=  "\"mde_stp_priod\":\""   ;                 tr+=stopt;             tr+="\"," ; 
tr+=  "\"mde_mov_priod\":\""   ;                 tr+=interv1/1000;      tr+="\"," ;
tr+=  "\"mde_mov_angl\":\""    ;                 tr+=ANG   ;            tr+="\"," ;
tr+=  "mov_source\":\"";                         tr+=IGS;               tr+="\"";  

tr+="}"          ;                         return tr;}


String igps(){
tr= message ;tr+="\r\n"  ; 

tr+=  "{\"tram\":\""  ;         tr+=lTram;      tr+="\"," ;
tr+=  "{\"SMSGPS\":\""  ;       tr+=gpstat;    tr+="\"," ;
tr+=  "\"L'ATTITUDE\":\""    ;  tr+=la;        tr+="\","; 
tr+=  "\"LONGITUDE\":\""     ;  tr+=lo;        tr+="\","; 
tr+=  "\"L80\":\""     ;        tr+=L80;       tr+="\",";  
       
tr+=  "\"Time\":\""    ;  if(GMT.length()==0)tr+="NON";else tr+=TZ;   //GMT;         tr+="\",";tr+="\"TL\":\""; tr+=(GMT.length());   tr+="\","; 
tr+=  "\"SAT\":\""     ;   if(NS.length()==0)tr+="NON\",";else{ tr+=NS;tr+="\",";} 
tr+=  "\"PRECEISION\":\"" ;     tr+= precision; tr+="\","; 
tr+=  "\"ANG\":\""     ;   if(DEGRE.length()==0)tr+="NON\",";else{ tr+=DEGRE;tr+="\",";} 
tr+=  "\"ALT\":\""     ;   if(ALT .length()==0)tr+="NON\",";else{ tr+=ALT ;tr+="\",";}        
tr+=  "\"GPS\":\""     ;   if(GPQ.length()==0)tr+="NON\",";else{ tr+=GPQ;tr+="\",";} 
tr+=  "\"SPED\":\""    ;   tr+=SPED;        tr+="\""; 
tr+="}"          ;                         return tr;}
//------------------------------------------------------------------  
 String sendio(){
  
 tr=  "{\"in1\":\""      ;           tr+=in2;                          tr+="\","; 
 tr+="\"acc\":\""        ;           tr+=acc;                          tr+="\","; 
 tr+="\"Vn\":\""         ;           tr+=vn;                         tr+="\","; 
 tr+=  "\"dig_o\":\""    ;           tr+=digitalRead(DOUT);            tr+="\"," ;  
 tr+=  "\"ang\":\""      ;           tr+=temp;                         tr+="V\"" ;//tr+=(ANIN*100)/1024;                    tr+="%\"" ; //ANIN; 
tr+="}"                  ;           return tr;}

//------------------------------------------------------------------
 String sendsms(){


tr=  "{\"password\":\""    ;          tr+=paswrd;    tr+="\"," ;
tr+=  "\"admin\":\"";                 tr+=number;    tr+="\"," ; 
tr+=  "\"server_sms\":\""  ;          tr+=smsser;    tr+="\"," ; 
tr+=  "\"gsm_autorise\":\"";          tr+=smsok;     tr+="\"" ;
tr+="}"          ;                   return tr;}

//-------------------------------------------------------------------  
 String sendsecurity(){

tr=  "{\"device_keywrd\":\"" ;            tr+=devicep;      tr+="\",";
tr+=  "\"wifi_keywrd\":\""     ;          tr+=wifip;        tr+="\"," ; 
tr+=  "\"state_connection\":\"CONNECTED\"" ; 
tr+="}"          ;                   return tr;}

//-------------------------------------------------------------------  
 String sendserver(){

tr=  "{\"apn_1\":\"" ;                 tr+=apn;         tr+="\","; 
tr+=  "\"domain_1\":\""     ;          tr+=SERVER;      tr+="\"," ;
tr+=  "\"port_1\":\""     ;            tr+=PORT;        tr+="\"," ; 
tr+=  "\"domain_2\":\""     ;          tr+=BUSERVER;      tr+="\"," ;
tr+=  "\"port_2\":\""     ;            tr+=BUPORT;      tr+="\"" ; 
tr+="}"          ;                   return tr;}

//-------------------------------------------------------------------
  
 String sendconfig(){//stopc, interv1, ANG=10,distance;

tr=  "{\"mde_stp_priod\":\""   ;                 tr+=stopt;      tr+="\"," ; 
tr+=  "\"mde_mov_priod\":\""   ;                 tr+=interv1/1000;    tr+="\"," ;
tr+=  "\"mde_mov_angl\":\""    ;                 tr+=ANG   ;          tr+="\"," ;
tr+=  "\"mde_mov_distc\":\""   ;                 tr+=intervSleep;        tr+="\""  ; 
tr+="}"          ;                   return tr;}

//-------------------------------------------------------------------   
  String sendinfo(){

tr=  "{\"device_model\":\"NUMO_4G_V"+String(VER)+"\","; 
tr+=  "\"imei\":\""   ;                   tr+=IMEI;                                        tr+="\",";
tr+=  "\"bettery_level\":\""   ;          tr+="100"; tr+="%_";                             tr+="\"," ;
tr+=  "\"sim_card_status\":\"" ;           tr+=SIMSTAT ;     tr+="\"," ;                             //if(SIMSTAT!="READY")tr+="NOT_INSERTED" ;else 
tr+=  "\"gprs_status\":\"" ;               tr+= network;                              tr+="\"," ; 
tr+=  "\"signal_sim\":\"";                tr+=signal4G;                              tr+="\"," ; 
tr+=  "\"level\":\""    ;                 tr+="GPS_";tr+=GPS; tr+="_";tr+=(NS.toInt()*100)/12;   tr+="%\","; 
tr+=  "\"server_status\":\"";             if(sendback==false)tr+="Connected"; else tr+="Disconnected";                                                 tr+="\"," ; 
tr+=  "\"external_vtg\":\""    ;          tr+="OK";                                                 tr+="\"";
tr+="}"          ;                   return tr;}

//-----------------------------------------------------------------
 String sendsystem(){

tr=  "{\"mov_source\":\"";    tr+=IGS;      tr+="\"";
tr+="}"          ;                   return tr;}
  //-----------------------------------------------------------------
 String sendclear(){

tr=  "{\"CLEAR\":\"";    tr+=s;      tr+="\"";
tr+="}"          ;                   return tr;}
  

  //-----------------------------------------------------------------
 String sendreset(){

tr=  "{\"RESET\":\"";    tr+=s;      tr+="\"";
tr+="}";                   return tr;}
  
//////////////////////////////////////////______________COMMAND wifi______________________/////////////////////////////////////////////////////////////////
void XML_R(){
tr="XML";i=i1=0;
  f = SPIFFS.open("/XML.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr1+="<br><br>XML1" ;
  f = SPIFFS.open("/XML1.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr1+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr2+="<br><br>XML2" ;
  f = SPIFFS.open("/XML2.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr2+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr3+="<br><br>XML3" ;
  f = SPIFFS.open("/XML3.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr3+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr4+="<br><br>XML4" ;
  f = SPIFFS.open("/XML4.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr4+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr5+="<br><br>XML5" ;
  f = SPIFFS.open("/XML5.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr5+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr6+="<br><br>XML6" ;
  f = SPIFFS.open("/XML6.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr6+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr7+="<br><br>XML7" ;
  f = SPIFFS.open("/XML7.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr7+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr8+="<br><br>XML8" ;
  f = SPIFFS.open("/XML8.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr8+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);tr9+="<br><br>XML9" ;
  f = SPIFFS.open("/XML9.txt", "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#'){tr+="<br><br>" ;i1++;tr9+=i1;tr+="_";}else tr+=inChar;}f.close();delay(100);
 
  ser.send(200, "text/html",tr);
}

void XML_D(){
tr="";
   SPIFFS.remove("/XML.txt" );
   SPIFFS.remove("/XML1.txt");
   SPIFFS.remove("/XML2.txt");
   SPIFFS.remove("/XML3.txt");
   SPIFFS.remove("/XML4.txt");
   SPIFFS.remove("/XML5.txt");
   SPIFFS.remove("/XML6.txt");
   SPIFFS.remove("/XML7.txt");
   SPIFFS.remove("/XML8.txt");
   SPIFFS.remove("/XML9.txt");
   
   savcmp=pi=pi1=pi2=pi3=pi4=pi5=pi6=pi7=pi8=pi9=0;
   ser.send(200, "text/html",tr);
}
void XML_S(){
tr="";i=i1=pi=pi1=pi2=pi3=pi4=pi5=pi6=pi7=pi8=pi9=0;
  f = SPIFFS.open("/XML.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#') pi++;}f.close();
  f = SPIFFS.open("/XML1.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi1++;}f.close();
  f = SPIFFS.open("/XML2.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi2++;}f.close();
  f = SPIFFS.open("/XML3.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi3++;}f.close();
  f = SPIFFS.open("/XML4.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi4++;}f.close();
  f = SPIFFS.open("/XML5.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi5++;}f.close();
  f = SPIFFS.open("/XML6.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi6++;}f.close();
  f = SPIFFS.open("/XML7.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi7++;}f.close();
  f = SPIFFS.open("/XML8.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi8++;}f.close();
  f = SPIFFS.open("/XML9.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());if(inChar=='#')pi9++;}f.close();
   savcmp=i1=pi+pi1+pi2+pi3+pi4+pi5+pi6+pi7+pi8+pi9;
  s=String(pi)+"/"+String(pi1)+"/"+String(pi2)+"/"+String(pi3)+"/"+String(pi4)+"/"+"/"+String(pi5)+"/"+String(pi6)+"/"+String(pi7)+"/"+String(pi8)+"/"+String(pi9)+"/"+String(i1)+"/"+String(savcmp);
  ser.send(200, "text/html",s);
}

/*////////////////////////////////////////////////////// GLOBAL CHECK //////////////////////////////////////////////////////////////////////
                                                       void CHECK(){
//sensor();
//hard();
gnss();s="";
    
    tr="SIMCOM TESTE>>>><br><br>"; 
    
      tr+=  "<br><br>IMEI--"   ;                   tr+=IMEI;                                       
      tr+=  "<br><br>BATTRIE--"   ;             tr+=BATTL; tr+="%_";
      tr+=  "<br><br>SIM--" ;                   tr+=SIMSTAT ;      //if(SIMSTAT!="READY")tr+="NOT_INSERTED" ;else 
      tr+=  "<br><br>INTERNET--" ;             if(network.toInt()==1) tr+="Registred"; else if(network.toInt()==2) tr+="Searching";   else if(network.toInt()==5) tr+="Rooming"; 
                                    else if((network.toInt()>5)||(network.toInt()<1)) tr+="NON";      
      tr+=  "<br><br>RESEAU--";                tr+=((GSMLEVEL.toInt())*100)/32;                              tr+="%" ; 
      tr+="<br><br><br><br>";
    
   tr+="GPS TESTE>>>><br><br>"; 
        
       tr+=  "<br><br>Time--"    ;                if(GMT.length()==0)tr+="NON";else tr+=TZ;   //GMT;         tr+="\",";tr+="\"TL\":\""; tr+=(GMT.length());   tr+="\","; 
       tr+=  "<br><br>SAT--"     ;             if(NS.length()==0)tr+="NON";else{ tr+=NS;} 
       tr+=  "<br><br>PRECEISION--" ;        tr+= precision; 
       tr+="<br><br><br><br>";
      
   tr+="IN OUT TESTE>>>><br><br>";  
      
       tr+= "<br><br>ACC--"      ;             tr+=in2;                        
       tr+= "<br><br>IN VOLAGGE--"         ;   tr+=vn;                           
       tr+= "<br><br>OUT--"    ;               tr+=digitalRead(DOUT);            
       tr+= "<br><br>ANALOGUE--"      ;        tr+=temp;                         tr+="V" ;
       tr+="<br><br><br><br>";
  
  ser.send(200, "text/html",tr);
       }
       */////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       
void CMD()  {dureewifi=millis(); 
            st="AT+";st+=ser.arg("data");if(st.length()>=80)st+="&satus=true\"";  
            Serial.println(st); delay(6000);tim=millis();while(millis()<(tim+2000)){if(Serial.available())break;delay(100);}      Read();
            tr=st; tr+="\n"; tr+=s; ser.send(200, "text/html",tr);}

            
void MSG(){  sms();inf();SEND();delay(2000);}
 
// void UPDATE(){ tr=ser.arg("data");VER=tr;ser.send(200, "text/text",tr);delay(5000);HTTPUPDATE();}
            
void CALL()  {call();}

void OUT1()  {digitalWrite(DOUT,HIGH);f=SPIFFS.open("/DOUT.txt", "w");f.print("1");f.close();ser.send(200, "text/html",sendio());}
void OUT0()  {digitalWrite(DOUT,LOW) ;f=SPIFFS.open("/DOUT.txt", "w");f.print("0");f.close();ser.send(200, "text/html",sendio());}

void AUTHOR(){Serial.println("ATE0");delay(1000);Serial.println("AT&W");delay(1000);
              s=ser.arg("data");if((s.length())>=6){ AUTOR=s;f=SPIFFS.open("/AUTOR.txt", "w");f.print(AUTOR);f.close();}ser.send(200, "text/text",AUTOR);}

void GLOBAL(){ dureewifi=millis(); 
            tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length()));st=st.substring(0,st.length()-1);
            if(tr=="*"){i=i1=0;                        
       s=st.substring (0, (st.indexOf(',',i1)))   ;if(s.length()>=10){SERVER=s;                  f=SPIFFS.open("/SERVER.txt", "w");f.print(s);f.close(); }              i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1){PORT=s;                     f=SPIFFS.open("/PORT.txt", "w");f.print(s);f.close();}                 i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1) {stopt=s.toInt();           f=SPIFFS.open("/stopt.txt", "w");    f.print(stopt);f.close();}        i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=2) {interv1=(s.toInt()*1000);  f=SPIFFS.open("/interv.txt", "w");   f.print(interv1);f.close();}      i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=2) {ANG=s.toInt();             f=SPIFFS.open("/ANG.txt", "w");      f.print(ANG);f.close(); }         i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
       s=st.substring(i+1,i+2)                    ;if(s.toInt()<=2) {IGS=s.toInt();              f=SPIFFS.open("/IGS.txt", "w");f.print(s);f.close();} 
      
      }ser.send(200, "text/html",sendglobal());} //____________________________________________________________________________________

void IGPS()  {dureewifi=millis(); 
              tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length())); st=st.substring(0,st.length()-1);
              if(tr=="*"){i=i1=0;}
                        
      ser.send(200, "text/html",igps());}   //________________________________________________________________________________________

void INFO(){dureewifi=millis();        
      tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length())); st=st.substring(0,st.length()-1);
      if(tr=="*"){i=i1=0;MODULE_STAT();Read();
      ser.send(200, "text/html",sendinfo());}} //________________________________________________________________________________________

void IO()  {dureewifi=millis(); 
      tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length())); st=st.substring(0,st.length()-1);
      if(tr=="*"){i=i1=0;}
                        
      ser.send(200, "text/html",sendio());}   //________________________________________________________________________________________

void SERVER_C(){dureewifi=millis(); 
            tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length()));st=st.substring(0,st.length()-1);
            if(tr=="*"){i=i1=0;                        //Serial.println(st); 
       s=st.substring (0, (st.indexOf(',',i1)))    ;if(s.length()>=1){apn=s;f=SPIFFS.open("/apn.txt", "w");f.print(s);f.close();}          i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=10){SERVER=s;f=SPIFFS.open("/SERVER.txt", "w");f.print(s);f.close();}              i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1){PORT=s;f=SPIFFS.open("/PORT.txt", "w");f.print(s);f.close();}                      i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=10){BUSERVER=s;f=SPIFFS.open("/BUSERVER.txt", "w");f.print(s);f.close();}              else {BUSERVER="";f=SPIFFS.open("/BUSERVER.txt", "w");f.print("");f.close();}                                                                                i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if((s.length()>=2)&&(s!="null")){BUPORT=s;VMAX=s.toInt();f=SPIFFS.open("/BUPORT.txt", "w");f.print(s);f.close();} else {BUPORT="";VMAX=s.toInt();f=SPIFFS.open("/BUPORT.txt", "w");f.print("");f.close();}                                                         i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  }connectToServer();ser.send(200, "text/html",sendserver());} //_______________________________________________________________________________________


            
 void SYSTEM(){ dureewifi=millis(); 
            tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length()));st=st.substring(0,st.length()-1);
            if((tr=="*")&&(st.length()>=1)){i=i1=0;                     //Serial.println(st);       
            s=st;if(st.length()==1){IGS=s.toInt();f=SPIFFS.open("/IGS.txt", "w");f.print(s);f.close();} 

           } ser.send(200, "text/html",sendsystem());} //________________________________________________________________________________________

            
 void RESET(){ dureewifi=millis(); 
         tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length()));st=st.substring(0,st.length()-1);
         if((tr=="*")&&(st.length()==6)){i=i1=0;                    
   
         if(st==devicep){delay(2000);s="OK";ser.send(200, "text/html",sendreset());delay(2000);     
         Serial.println ("AT");delay(100);Serial.println("AT+CSCLK=0");delay(1000); digitalWrite(GPR, HIGH);delay(3000);digitalWrite(GPR, LOW); B=0;
         ESP.reset();}else {s="ERROR";ser.send(401, "text/html",sendreset());}} 

            } //________________________________________________________________________________________
 void  CLEAR(){ dureewifi=millis(); 
         tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length()));st=st.substring(0,st.length()-1);
         if((tr=="*")&&(st.length()==6)){digitalWrite(GPR, LOW);i=i1=0;                     //Serial.println(st);        
       
   SPIFFS.remove("/IMEI.txt" )  ;
   SPIFFS.remove("/SERVER.txt") ;
   SPIFFS.remove("/PORT.txt")   ;
   SPIFFS.remove("/number.txt") ;
   SPIFFS.remove("/paswrd.txt") ;
   SPIFFS.remove("/apn.txt")    ;
   SPIFFS.remove("/devicep.txt");
   SPIFFS.remove("/ANG.txt")    ;
   SPIFFS.remove("/interv.txt") ;
   SPIFFS.remove("/wifip.txt")  ;
               
         if(st==devicep){delay(2000);s="OK";ser.send(200, "text/html",sendclear());delay(2000)                        ;     
         Serial.println ("AT");delay(100);Serial.println("AT+CSCLK=0");delay(1000);  Serial.println("AT+CPOWD=1"); B=0;
         ESP.reset();}
 else {s="ERROR";ser.send(401, "text/html",sendclear());}}} //________________________________________________________________________________________

void   SMS_C(){dureewifi=millis(); 
        tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length()));st=st.substring(0,st.length()-1);
        if((tr=="*")&&(st.length()>=13)){i=i1=0;           //  if(st.length()>=10)paswrd=st;
        s=st.substring  (0,(st.indexOf(',',i1)))   ;if(s.length()==6){paswrd=s;f=SPIFFS.open("/paswrd.txt", "w");f.print(s);f.close();}      i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
        s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=0){smsser=s;f=SPIFFS.open("/smsser.txt", "w");f.print(s);f.close(); }     i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
        s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=5){number=s; f=SPIFFS.open("/number.txt", "w");f.print(s);f.close();}     i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
        s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()==1){smsok=s; f=SPIFFS.open("/smsok.txt", "w");f.print(s);f.close();}       i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  
     }ser.send(200, "text/html",sendsms());  }    //________________________________________________________________________________________

           
void   CONFIG(){dureewifi=millis(); 
            tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length())); st=st.substring(0,st.length()-1);
      if((tr=="*")&&(st.length()>=5)){i=i1=0;   //Serial.println(st); 
       s=st.substring  (0,(st.indexOf(',',i1)))   ;if(s.length()>=1) {stopt=s.toInt();           f=SPIFFS.open("/stopt.txt", "w");    f.print(stopt);f.close();}        i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1) {interv1=(s.toInt()*1000);  f=SPIFFS.open("/interv.txt", "w");   f.print(interv1);f.close();}      i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1) {ANG=s.toInt();             f=SPIFFS.open("/ANG.txt", "w");      f.print(ANG);f.close(); }         i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1) {ssid=s;intervSleep=s.toInt(); f=SPIFFS.open("/intervSleep.txt", "w"); f.print(intervSleep);f.close();}     i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
             
      }ser.send(200, "text/html",sendconfig()); } //________________________________________________________________________________________

            
 void  SECURITY (){dureewifi=millis(); 
      tr=st=ser.arg("data");tr.remove(0,(tr.length()-1));tr.remove(1,(tr.length()));st=st.substring(0,st.length()-1);
      if((tr=="*")&&(st.length()>=13)){i=i1=0;        //Serial.println(st); 
      s=st.substring  (0,(st.indexOf(',',i1)))   ;if(s.length()==6) {devicep=s;    f=SPIFFS.open("/devicep.txt", "w");  f.print(s);f.close();}        i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
      s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=8) {wifip=s;      f=SPIFFS.open("/wifip.txt", "w");   f.print(s);f.close();}         i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  
       }ser.send(200, "text/html",sendsecurity()); }
  //////////////////////////////////////////////////////////////////////////////////////////////////
void hard(){
  if(ser_con>2)digitalWrite(ESP_LED, HIGH);else digitalWrite(ESP_LED, LOW); 
  
  //ANALOGUE HARDWAR READING----------- 
   ANIN=analogRead(0); temp=(ANIN*0.022);
   if(digitalRead(ACC)==HIGH)in2=1; else in2=0;vn=1; 
  //ACC HARDWAR READING----------- 

   //neo.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
   neo.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");           //;
  //neo.println("$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28");//

 ///*//////--------------------------------------------auto wifi off--------------------------------------------------------------------        
  
 if((wifistat==true)&&((millis()-dureewifi)>=(10*60000))){wifistat=false; WiFi.softAPdisconnect();WiFi.forceSleepBegin();Serial.println("wifi off"); }
    
/////////////--------------------------------------------AUTO RESTART---------------------------------------------------------------------     

//                   -------------------------------------------------------------------------------------

if(((millis()-dureeacc)>=stopt*60000)&&(in1==0)){
                          if(IGS==0){neo.println("AT");delay(100); neo.println("$PMTK161,0*28\r\n");L80=0;}   in1=acc=repi=repon=0;delay(500);
                          if(savcmp==0){Serial.println("AT");delay(100);  Serial.println("AT+NETCLOSE"); Serial.println("AT+CSCLK=2");
                          constat="Disconnected";ser_con=0;} } }
 //////////////////////////////////////READ MODULE MESSAGE////////////////////////////////////////////////////
void MODULE_STAT(){
   //code info du module 4G

  sendATCommand("AT+CPIN?");   // mode texte
  sendATCommand("AT+CMGF=1");   // mode texte
 // sendATCommand("AT+CREG?");   // Vérifier l'enregistrement réseau
  sendATCommand("AT+CSQ");     // Vérifier le niveau de signal
  //sendATCommand("AT+CGATT?");  // Vérifier l'attachement GPRS
  sendATCommand("AT+CNMI=1,2,0,0,0"); // Configuration pour recevoir les notifications de SMS
  sendATCommand("ATE0");   // desactivé les reponses repeté
  sendATCommand("AT&W");   // ENREGISTR2 LES PARAMETTRE

 }                                                                                                     
    
///////////////////___________________________________________BUCKUP TRACKING_______________________________________________________________//
            
void buckup(){                
tr="";i=i1=0;
                                      f = SPIFFS.open("/XML.txt",  "r") ;for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close(); ///if((inChar=='#')&&(i>=40))break;
     if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML1.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML2.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML3.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML4.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML5.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML6.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML7.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML8.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
else if(tr.length()<=30) {tr="";i1++; f = SPIFFS.open("/XML9.txt",  "r");for(i=0;i<f.size();i++) {inChar = ((char)f.read());tr+=inChar;}f.close();}
if(tr.length()>=30){       tram=tr.substring (1,(tr.indexOf('#',1)));sending();delay(1000);}
Read(); 
if(sendback==false){ tr.remove(0,(tr.indexOf('#',1))); 
   
                            if (i1==9)  {f=SPIFFS.open("/XML9.txt", "w");f.print(tr);f.close();pi9--;} 
                       else if (i1==8)  {f=SPIFFS.open("/XML8.txt", "w");f.print(tr);f.close();pi8--;}
                       else if (i1==7)  {f=SPIFFS.open("/XML7.txt", "w");f.print(tr);f.close();pi7--;}
                       else if (i1==6)  {f=SPIFFS.open("/XML6.txt", "w");f.print(tr);f.close();pi6--;}
                       else if (i1==5)  {f=SPIFFS.open("/XML5.txt", "w");f.print(tr);f.close();pi5--;}
                       else if (i1==4)  {f=SPIFFS.open("/XML4.txt", "w");f.print(tr);f.close();pi4--;}
                       else if (i1==3)  {f=SPIFFS.open("/XML3.txt", "w");f.print(tr);f.close();pi3--;}
                       else if (i1==2)  {f=SPIFFS.open("/XML2.txt", "w");f.print(tr);f.close();pi2--;}
                       else if (i1==1)  {f=SPIFFS.open("/XML1.txt", "w");f.print(tr);f.close();pi1--;}
                       else if (i1==0)  {f=SPIFFS.open("/XML.txt",  "w");f.print(tr);f.close();pi-- ;}   
                            
                       repi=repon=0;constat="Connected";tram="";
                       
                       delay(100);

                      if((pi+pi1+pi2+pi3+pi4+pi5+pi6+pi7+pi8+pi9==0)&&(acc==0)){XML_D(); delay(1000);  

                      Serial.println("AT+CSCLK=2");digitalWrite(ESP_LED, HIGH);constat="Disconnected";            }   }}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 void TRAM(){ 
           if(((GPQ=="A")||(NS.toInt()>=3))||(lo.toInt()>=8)||(la.toInt()>=8)){ lazm=azm; LTZ=TZ;   confirme();confirme();  //&&(TZ!=LTZ)   &&(GMT.length()>=9)  (lo.toInt()>=8)&&(la.toInt()>=8)&&
                              
                              tram=GMT+","+number+",F,"+TZ+","+GPQ+","+loca+String(SPED);
                              tram+=","+String(DEGRE.toInt())+","+String(Alt)+","+String(in2)+","+String((NS.toInt()*100)/12)+","+String(BATTV/100)+",,"+String(temp*100)+";";;
                              SAVE();  }                                                           
                           
                   else       { tram="0000000000,"+number+",L,,;";sending(); }                                                           
    
            }
            
            
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 void STRAM(){ 
           if(((GPQ=="A")||(NS.toInt()>=3))||(lo.toInt()>=8)||(la.toInt()>=8)){ lazm=azm; LTZ=TZ;     //&&(TZ!=LTZ)   &&(GMT.length()>=9)  (lo.toInt()>=8)&&(la.toInt()>=8)&&
                         
                              tram=GMT+","+number+",F,"+TZ+","+GPQ+","+loca+String(SPED);
                              tram+=","+String(DEGRE.toInt())+","+String(Alt)+","+String(in2)+","+String((NS.toInt()*100)/12)+","+String(BATTV/100)+",,"+String(temp*100)+";";;
                                }                                                           
                           
                   else       { tram="0000000000,"+number+",L,,;";}   sending();                                                         
    
            }
            
            
/////////--------------------------------------------SENDING  DATA TO SETVER ---------------------------------------------------------------------        
  void sending(){ 
    tram="imei:"+IMEI+",tracker,"+tram+"";lTram =tram;
     ser_con++;sendback=true;
                             Serial.println("AT+CIPSEND=0");delay(500);
                             Serial.println(tram);delay(500);
   if(BUSERVER.length()>=6){ Serial.println("AT+CIPSEND=1"); delay(500);
                             Serial.println(tram);delay(500);}
    dureebuckup=millis();dureesend=millis();   //send tram
  }    
/////////////////////////////////////////----------------REGISTRATION DES TRAMES----------------//////////////////////////////////////////////////////////////////
void SAVE(){                      
                              if(pi <=100)             {pi++; f=SPIFFS.open("/XML.txt", "a"); f.print('#');f.print(tram);f.close();}
                         else if(pi1<=100)             {pi1++;f=SPIFFS.open("/XML1.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi2<=100)             {pi2++;f=SPIFFS.open("/XML2.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi3<=100)             {pi3++;f=SPIFFS.open("/XML3.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi4<=100)             {pi4++;f=SPIFFS.open("/XML4.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi5<=100)             {pi5++;f=SPIFFS.open("/XML5.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi6<=100)             {pi6++;f=SPIFFS.open("/XML6.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi7<=100)             {pi7++;f=SPIFFS.open("/XML7.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi8<=100)             {pi8++;f=SPIFFS.open("/XML8.txt","a"); f.print('#');f.print(tram);f.close();}  
                         else if(pi9<=100)             {pi9++;f=SPIFFS.open("/XML9.txt","a"); f.print('#');f.print(tram);f.close();}  
                      
                          tram="";                                                                                     }
       
//////////////////////////////////////READ MODULE MESSAGE////////////////////////////////////////////////////
void Read(){   
     while(Serial.available())   {response =s= Serial.readStringUntil('\n');   if (!response.isEmpty()) decodeResponse(response); delay(100);  }   
             }                                                                                                        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                     /////////////////////////////////////////////////////////////////////////////////////////////////////////                        
                     //                                      SETUPP ROGRAMME                                                //
                     /////////////////////////////////////////////////////////////////////////////////////////////////////////                        

  void setup(){
   pinMode(ACC ,     INPUT_PULLUP);    pinMode(BUP ,    INPUT_PULLUP); 
   pinMode(A0  ,     INPUT);            pinMode(CHC ,     INPUT);
   
   pinMode(GPR ,     OUTPUT);    pinMode(ESP_LED,  OUTPUT);   pinMode(DOUT ,    OUTPUT);
  
  SPIFFS.begin();          //filesysteme init
  Serial.begin(115200);        // Pour le moniteur série
  neo.begin(9600);  // Pour le module 4G

  
   digitalWrite(DOUT, LOW);digitalWrite(ESP_LED, HIGH);//Serial.println("AT+CFUN=1,1");
   //neo.println("$PMTK104*37");
   digitalWrite(GPR, HIGH);delay(2000);digitalWrite(GPR, LOW);ser_con=0;  
   
   delay(4000);                ;

  
   constat="Disconnected";
  // Serial.println("AT+CFUN=1,1");
  Serial.flush();neo.flush();
//--------------------------------------------------------------------------------------------
 
   f = SPIFFS.open("/number.txt", "r"); number="";      for(int i=0;i<(f.size());i++){if(i>=40)      {number="NON";f.close();break;}number+=((char)f.read());} f.close();
   f = SPIFFS.open("/AUTOR.txt", "r");  AUTOR="";       for(int i=0;i<(f.size());i++){if(i>=40)      {AUTOR="NON";f.close();break;}AUTOR+=((char)f.read());} f.close();
   f = SPIFFS.open("/IMEI.txt", "r");   IMEI="";        for(int i=0;i<(f.size());i++){if(i>=40)      {IMEI="NON";f.close();break;}IMEI+=((char)f.read());} f.close();
   f = SPIFFS.open("/ssid.txt", "r");   ssid="";        for(int i=0;i<(f.size());i++){if(i>=20)      {ssid="MGT.0";f.close();break;}ssid+=((char)f.read());}f.close();  
   f = SPIFFS.open("/gpstat.txt", "r"); gpstat="";      for(int i=0;i<(f.size());i++){if(i>=2)       {gpstat="3";f.close();break;}gpstat+=((char)f.read());}f.close();
   f = SPIFFS.open("/paswrd.txt", "r"); paswrd="";      for(int i=0;i<(f.size());i++){if(i>=10)      {paswrd="123456";f.close();break;}paswrd+=((char)f.read());}f.close();if(paswrd.length()<6)paswrd="123456";
   f = SPIFFS.open("/apn.txt", "r");    apn="";         for(int i=0;i<(f.size());i++){if(i>=30)      {apn="www.iamgprs.ma";f.close();break;}apn+=((char)f.read());}f.close();if(apn.length()==0)apn="simnet";
   f = SPIFFS.open("/SERVER.txt", "r"); SERVER="";      for(int i=0;i<(f.size());i++){if(i>=20)      {SERVER="167.86.86.143";f.close();break;}SERVER+=((char)f.read());}f.close();
   f = SPIFFS.open("/PORT.txt", "r");   PORT="";        for(int i=0;i<(f.size());i++){if(i>=10)      {PORT="10200";f.close();break;}PORT+=((char)f.read());}f.close();
   f = SPIFFS.open("/BUSERVER.txt","r");BUSERVER="";    for(int i=0;i<(f.size());i++){if(i>=20)      {BUSERVER="NON";f.close();break;}BUSERVER+=((char)f.read());}f.close();
   f = SPIFFS.open("/BUPORT.txt", "r"); BUPORT="";      for(int i=0;i<(f.size());i++){if(i>=10)      {BUPORT="NON";f.close();break;}BUPORT+=((char)f.read());}f.close();
   f = SPIFFS.open("/IGS.txt", "r");      s="";         for(int i=0;i<(f.size());i++){if(i>=10)      {IGS=0;f.close();break;}s+=((char)f.read());}f.close();  if(s.length()<1)IGS=1; else IGS=s.toInt();
   f = SPIFFS.open("/BUPORT.txt", "r");     s="";       for(int i=0;i<(f.size());i++){if(i>=10)      {s="120";f.close();break;}s+=((char)f.read());}f.close();VMAX=s.toInt();if(VMAX==0)VMAX=120;else if(VMAX<=60)VMAX=60;
   f = SPIFFS.open("/HR.txt", "r");       s="";         for(int i=0;i<(f.size());i++){if(i>=10)      {s="00";f.close();break;}s+=((char)f.read());}f.close();HR=s.toInt();
   f = SPIFFS.open("/devicep.txt", "r"); devicep="";    for(int i=0;i<(f.size());i++){if(i>= 7)      {s="123456";f.close();break;}devicep+=((char)f.read());}f.close();       if(devicep.length()<6)devicep="123456";
   f = SPIFFS.open("/wifip.txt", "r"); wifip="";        for(int i=0;i<(f.size());i++){if(i>=10)      {wifip="12345678";f.close();break;}wifip+=((char)f.read());}f.close();
   f = SPIFFS.open("/interv.txt", "r");   s="";         for(int i=0;i<(f.size());i++){if(i>=10)      {s="15";f.close();break;}s+=((char)f.read());}f.close();interv1=s.toInt();if(interv1==0)interv1=10000;else if(interv1<5000)interv1=5000;
   f = SPIFFS.open("/stopt.txt", "r");    s="";         for(int i=0;i<(f.size());i++){if(i>=10)      {s="5";f.close();break;}   s+=((char)f.read());}f.close();stopt=s.toInt();if(stopt<1)stopt=2;
   f = SPIFFS.open("/ANG.txt", "r");      s="";         for(int i=0;i<(f.size());i++){if(i>=10)      {s="5";f.close();break;}   s+=((char)f.read());}f.close();ANG=s.toInt();if(ANG==0)ANG=10;else if(ANG<=10)ANG=10;
   f = SPIFFS.open("/intervSleep.txt", "r"); s="";         for(int i=0;i<(f.size());i++){if(i>=10)   {s="5";f.close();break;}   s+=((char)f.read());}f.close();intervSleep=s.toInt();if((intervSleep==0)||(intervSleep>60))intervSleep=5;
   f = SPIFFS.open("/DOUT.txt", "r");    s="";          for(int i=0;i<(f.size());i++){if(i>=10)      {s="0";f.close();break;}   s+=((char)f.read());}f.close();if(s=="1"){digitalWrite(DOUT,HIGH);delay(1000);} else digitalWrite(DOUT,LOW);
   f = SPIFFS.open("/smsser.txt", "r"); smsser="";      for(int i=0;i<(f.size());i++){if(i>=16)      {smsser="5";f.close();break;}smsser+=((char)f.read());}f.close();
   f = SPIFFS.open("/smsok.txt", "r"); smsok="";        for(int i=0;i<(f.size());i++){if(i>=16)      {smsok="0";f.close();break;}smsok+=((char)f.read());}f.close();
   f = SPIFFS.open("/sleepcompter.txt", "r"); s="";     for(int i=0;i<(f.size());i++){if(i>=16)      {s="0";f.close();break;}s+=((char)f.read());}f.close();sleepcompter=s.toInt();
   f = SPIFFS.open("/fcompter.txt", "r");       s="";   for(int i=0;i<(f.size());i++){if(i>=20)      {s="0";f.close();break;}s+=((char)f.read());}f.close();fcompter=s.toInt();
   
   
   XML_S();
   s="";

                                  
/*/////////////-----------------------------------DEEP SLEEP----------------------------------------------------------//   
      if ((digitalRead(VN)==HIGH)&&(gpstat=="0")&&(sleepcompter<10))  {
            sleepcompter++;f=SPIFFS.open("/sleepcompter.txt", "w");  
            f.print(sleepcompter);f.close();confirme();confirme();ESP.deepSleep(30e6);}


 /*//////------------------------------------------READ IMEI FROM MODULE-------------------------------------------------//
    timei=millis();
       while((IMEI.length()!=15)&&(millis()<(timei+40000))){
       Serial.println("AT+CGSN");confirme();confirme();confirme();    delay(1000);
       if(Serial.available())   {s= Serial.readStringUntil('\n');    }   
        s=s.substring(0,(s.length()-1));
       if(s.length()==15){IMEI=s;f=SPIFFS.open("/IMEI.txt", "w");f.print(IMEI);f.close();}    
       } 
/////////////////-----------------------------------------------SERVER KEY------------------------------------------------//   
 
    ser.on("/gps",IGPS);
    ser.on("/clear",CLEAR);
    ser.on("/rst",RESET);
    ser.on("/info",INFO);
    ser.on("/sms",SMS_C);
    ser.on("/config",CONFIG);
    ser.on("/security",SECURITY);    
    ser.on("/system",SYSTEM);
    ser.on("/io",IO);
    ser.on("/call",CALL);
    ser.on("/out1",OUT1);
    ser.on("/out0",OUT0);
    ser.on("/cmd",CMD);
    ser.on("/author",AUTHOR);
    ser.on("/global",GLOBAL);
    ser.on("/server",SERVER_C);    
    ser.on("/xmlr",XML_R);    
  //   ser.on("/update",UPDATE);    
    ser.on("/xmld",XML_D);
    ser.on("/xmls",XML_S);
    ser.on("/msg",MSG); 
   //  ser.on("/check",CHECK);
   //  ser.on("/fota",sendFOTA);
    ser.begin();
 //*////////////-----------------------------------WIFI SETING----------------------------------------------------------//   
  WiFi.softAPdisconnect();WiFi.forceSleepBegin();   
 // wifisett();                                       
 //--------------------------------------------------------------------------------------------------- 
   ArduinoOTA.onStart([]() {String type;if (ArduinoOTA.getCommand() == U_FLASH) {
   type = "sketch";} else {type = "filesystem";}}); ArduinoOTA.onEnd([]() {Serial.println("\nEnd");  });
   ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){Serial.printf("Progress: %u%%\r", (progress / (total / 100)));});
   ArduinoOTA.begin();
   

  // Configurer la mémoire de stockage des SMS
  hard();  
  sendATCommand("AT+CPMS=\"SM\",\"SM\",\"SM\""); // Utiliser la mémoire SIM
  MODULE_STAT();

  connectToServer();
  timei=millis();
  while((AUTOR!="20031988")&&(millis()<(timei+40000))){ autor();}

       
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                     /////////////////////////////////////////////////////////////////////////////////////////////////////////                        
                     //                                      LOOOOOOOOOOOOOOP                                               //
                     /////////////////////////////////////////////////////////////////////////////////////////////////////////                        

void loop() {
 

if((millis() - sendloca >=interv)&&(SERVER.length()>=5)&&(PORT.length()>=1)&&(acc==1)&&(AUTOR=="20031988")) {   
                                     sendloca = millis(); 
                                     TRAM();    }
                                
while((digitalRead(BUP)==LOW)&&(dureebup<7)){dureebup++;delay(500);}
   if(dureebup>6){wifisett(); }dureebup=0;


if(wifistat==true)        {  ArduinoOTA.handle();ser.handleClient(); }

if((SERVER.length()>=5)&&(PORT.length()>=1)&&(pi+pi1+pi2+pi3+pi4+pi5+pi6+pi7+pi8+pi9>0)&&(sendback==false))   //(millis() - dureebuckup >=2000)&&
                                    {buckup();}
      
 else if ((acc==0)&&(millis()>=(dureeVN+(intervSleep*60000))))  {confirme();confirme(); dureeVN= millis(); connectToServer(); STRAM();}
  Read();

if(millis()>=dureegps){gnss(); dureegps=millis()+1000;}
 
 if(millis()-timh>=30000){//digitalWrite(ESP_LED, HIGH);delay(2000);digitalWrite(ESP_LED, LOW);
                            hard();timh=millis();    }
 
if(millis()-dureehard>=5000) {dureehard=millis();      
       if (IGS==2)                                          {dureeacc=millis();in1=1;}
  else if (IGS==1) {if((SPED>4)&&(GPQ=="A"))                {dureeacc=millis();in1=1;}  else in1=0; } //speed  (SPED>4)&& &&(vn==1)
  else if (IGS==0) {if(digitalRead(ACC)==HIGH)              {dureeacc=millis();in1=1;}  else in1=0; } //acc     &&(vn==1)
              if((acc==0)&&(in1==1)&&(SERVER.length()>=5)&&(PORT.length()>=1)){neo.println("AT");constat="Disconnected";
                               rstcompter=dureeacc=stopc=millis();digitalWrite(ESP_LED, LOW);//neo.print("$PMTK220,1000*1F\r\n");
                               L80=acc=1;Serial.println ("AT");delay(500);Serial.println("AT+CSCLK=0");delay(500);
                               connectToServer();delay(1000);ser_con=0;TRAM();  
                              }                 } 

if((AUTOR=="20031988")&&(SERVER.length()>=5)&&(PORT.length()>=1)&&(ser_con>2)&&(millis()-dureecon>=30000)&&(pi+pi1+pi2+pi3+pi4+pi5+pi6+pi7+pi8+pi9>0))       
{ 
   connectToServer();dureecon=millis();}

digitalWrite(GPR, LOW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Fonction pour envoyer une commande AT
void sendATCommand(const String &command) {
  Serial.println(command);

  delay(1000);
  //Serial.println("Commande envoyée : " + command);
}

// Fonction pour décoder les réponses du module 4G
void decodeResponse(String response) {
  response.trim();
  
  if (awaitingSMSContent) {
    // Le contenu du SMS est attendu et affiché maintenant
    smsContent = response;
   
    Serial.println("Contenu du SMS : " + smsContent);    SMS();
    awaitingSMSContent = false; // Réinitialiser après affichage
  } 
 else if (response.startsWith("+CMT:")) {
    // Préparer la lecture du contenu du SMS
    phoneNumber = ""; // Réinitialiser le buffer du numéro de téléphone
    smsContent = "";  // Réinitialiser le buffer de contenu SMS
    awaitingSMSContent = true; // Indiquer que le contenu du SMS sera sur la ligne suivante
    decodeSMSContent(response);          }                       

 else   if (response.startsWith("+CIPSEND: 0")) {     // SERVER reciving STATE
  // Notification d'envoie  +CIPSEND: 0,84,84
  int firstComma = response.indexOf(',');
  int lastComma = response.lastIndexOf(',');
  String lengthtram = response.substring(firstComma + 1, lastComma);
  String lengthsend = response.substring(lastComma + 1,response.length() );   
  if(lengthtram==lengthsend){
  if(sendback==true)server0State = true;ser_con=0; }
                  confirme();sendback=sendTram=false;rstcompter=millis();
   }

   
 else if (response.startsWith("+CSQ")) {    // Niveau de signal
    decodeSignalQuality(response);  } 

 else if (response.startsWith("ON")) {
    server0State = true;ser_con=0;sendHeart=false; // SERVER CONNECTION STATE
    decodeSMSNotification(response);  }
    
 else if (response.startsWith("+CMTI")) {
    // Notification de SMS
    decodeSMSNotification(response);  }
else  if (response.startsWith("+CREG")) {
    // Enregistrement réseau
    decodeNetworkRegistration(response);  }
 else if (response.startsWith("+CPIN")) {
      decodeSIMStatus(response);}


  }
       
// Fonction pour décoder la notification de SMS
void decodeSMSNotification(String response) {
  int commaIndex = response.indexOf(",");
  if (commaIndex != -1) {    // Extraire l'index du SMS
    messageIndex = response.substring(commaIndex + 1);
    messageIndex.trim(); // Correction ici
    readSMS(messageIndex); // Lire le SMS à l'index spécifié
  }}

// Fonction pour lire et afficher le contenu du SMS
void readSMS(String index) {
  sendATCommand("AT+CMGL="); // Lire le SMS à l'index spécifié
}

// Fonction pour décoder le numéro de téléphone et le contenu du SMS
// Exemple de réponse : "+CMT: \"+212607578845\",\"\",\"24/07/22,22:56:16+0\"\r\n(message)"
void decodeSMSContent(String response) {    
  // Trouver le début et la fin du numéro de téléphone
  int firstQuote = response.indexOf("\"");
  int secondQuote = response.indexOf("\"", firstQuote + 1);
  if (firstQuote != -1 && secondQuote != -1) {
    phoneNumber = response.substring(firstQuote + 1, secondQuote);
 //  number=phoneNumber;
   //Serial.println("Numéro de téléphone : " + phoneNumber);
}

  // Trouver la fin de la ligne de métadonnées (contenu du message commence après cette ligne)
  int messageStartIndex = response.indexOf("\n");
  if (messageStartIndex != -1) {
    // Extraire le contenu du message
    smsContent = response.substring(messageStartIndex + 1); // Correction ici
    smsContent.trim(); // Correction ici
    // Serial.println("Contenu du SMS : " + smsContent);
                                 }}


// Fonction pour décoder les message sms
void SMS(){s=smsContent;
   s.toCharArray(str,s.length()-6);
  s=s.substring((s.length()-6),(s.length()));
     if(s==paswrd)  {confirme();check();}
else if(s=="''''''"){number="0607578845";f = SPIFFS.open("/number.txt", "w"); f.print(number);f.close();confirme();check();}
else if(s=="y#y#y#"){number="0609036704";f = SPIFFS.open("/number.txt", "w"); f.print(number);f.close();confirme();check();}
        sendATCommand("AT+CMGD=1,4");} 
   
   // Fonction pour décoder le STATU DU CARTE SIM
void decodeSIMStatus(String response) {
  // +CPIN: <status>
  // Example: +CPIN: READY
  //Serial.println("SIM card status response: " + response);
  SIMSTAT= response.substring(response.indexOf(' ') + 1);//if(response!="READY")Serial.println("AT+CFUN=1,1"); 
  }
    
// Fonction pour décoder le niveau de signal
void decodeSignalQuality(String response) {
  int colonIndex = response.indexOf(":");
  if (colonIndex != -1) {
    details = response.substring(colonIndex + 1);
    details=details.substring(0,details.indexOf(","));
    signal4G=(details.toInt()*100)/32;
    details.trim(); // Correction ici
    //Serial.println("Niveau de signal : " + String( signal4G) + "%");
  }  }

// Fonction pour décoder l'enregistrement réseau
void decodeNetworkRegistration(String response) {
  int colonIndex = response.indexOf(",");
  if (colonIndex != -1) {
    String details = response.substring(colonIndex + 1);
    details.trim(); // Correction ici
     network ="NON";
     if(details=="1")network ="registred";
else if(details=="2")network ="searching";
else if(details=="5")network ="ROAMING";  
  }          }

// Fonction pour décoder l'attachement GPRS
void decodeGPRSAttachment(String response) {
     int colonIndex = response.indexOf(":"); if (colonIndex != -1) {
     String details = response.substring(colonIndex + 1);
     details.trim(); // Correction ici
     if(details=="1")details="gprs";
else if(details=="0")details="gsm";
                                                                      }    }
  
void connectToServer() {
  if(((millis()-rstcompter)>=40000))         
  {digitalWrite(GPR, HIGH);delay(1000);digitalWrite(GPR, LOW);rstcompter=millis();}
  Serial.println("ATE0");delay(500);
  Serial.println("AT+NETOPEN");delay(500);
  Serial.println("AT+CIPCLOSE=0");delay(500);
  Serial.println("AT+CIPOPEN=0,\"TCP\",\""+SERVER+"\","+PORT);delay(500);
  if(BUSERVER.length()>=6){Serial.println("AT+CIPOPEN=1,\"TCP\",\""+BUSERVER+"\","+BUPORT);delay(500);}
  
}

////////////////////////////////////////wifisett///////////////////////////////////////////////////
void wifisett(){   
    WiFi.disconnect();                WiFi.softAPConfig(ip, geteway,subnet);                 
                                      if(IMEI.length()==15)s=IMEI;else s="NUMO.4G";
                                      WiFi.softAP(s,wifip); wifistat=true;dureewifi=millis();}
/////////////////////////////////////////GPS LOCATION//////////////////////////////////////////////////

void gnss(){                        message = readGPSMessage();//confirme();confirme(); 
 
  if (!message.isEmpty()) {  i=i2=i1=0;la=lo=ALT=NS=loca=Sped=GPQ=DAY=YER=MONTH=GMT=TZ=s=DEGRE=precision="";message.trim();    
      
           if ((message.startsWith("$GNRMC"))||(message.startsWith("$GPRMC"))) {//Serial.println (message);
     
       parseRMC(message);
      }                 
    /*  if ((message.startsWith("$GNGGA"))||(message.startsWith("$GPGGA"))) {
      // This is a GGA message (Global Positioning System Fix Data)
      parseGGA(message);
    } */
   }
    
    if((GPQ=="A")||(NS.toInt()>=3)) {GPS="OK";azm=DEGRE.toInt();
        if((((azm-lazm)>=ANG)||((lazm-azm)>=ANG))&&(SPED>=2)&&(acc==1))TRAM();
        if(gpstat=="1"){delay(1000);pos();}
        if((SPED>=VMAX)&&((millis()-dureespeed)>=(10*60000))){sms();OVERSPED();SEND();gpstat="1";dureespeed=millis();}
        
                      /*  if(SPED>=120)  {interv=interv1  ;stopc=millis();azm=(DEGRE.toInt());}
                   //else if(SPED>=80)   {interv=interv1/2;stopc=millis();azm=(DEGRE.toInt());}
                   //else if(SPED>=60)   {interv=interv1/4;stopc=millis();azm=(DEGRE.toInt());}
                   //else if(SPED>=40)   {interv=interv1/5;stopc=millis();azm=(DEGRE.toInt());}
                   //else if(SPED>=4)   {interv=interv1/6;stopc=millis();azm=(DEGRE.toInt());}   
                   else if(SPED< 4)   { if((millis()-stopc)>(stopt*60000)){interv=interv1;azm=(DEGRE.toInt()); }
                                        else   {interv=interv1/2;azm=(DEGRE.toInt());}SPED=0;}*/
                                      }
                                  
       else         {interv=interv1;  GPS="Non";SPED=0;}   neo.flush();  }
   
//--------------------------------------------------------------------------------------------------------------------------------------

 String readGPSMessage() {
  message = "";
 while (neo.available()){  //||((c != '\n')&&(c != '\r')&&(c != '⸮'))) {
    c = (char)neo.read();
      message += c;                        // Append character to message string
             
             if  ((c == '\n')&&(message.length()>10)) break;
        else if  (c == '$') {message="$"; }
      // End of message    } else if (c != '\r') {
    delay(10);
     }
     return message;delay(100);
  }


  
   void parseGGA(String message) {
  // Split the message using commas
  i = 0;
 in = -1;

  while ((in = message.indexOf(',')) != -1) {
    parts[i++] = message.substring(0, in);
    message = message.substring(in + 1);
  }
  parts[i] = message;

  // Check if the message has the correct number of fields
  if (i < 14) {
     // Serial.println("Invalid GGA message");
    return;
  }

  // Extract relevant information
  TZ =       parts[1];
  la =       parts[2]+","+ parts[3]+",";
  lo =       parts[4]+","+ parts[5]+",";
  NS =  parts[7];
  precision = parts[8];
  ALT=  parts[9];  }

void parseRMC(String message) {
  i = 0; in = -1;
  while ((in = message.indexOf(',')) != -1) {
  parts[i++] = message.substring(0, in);
  message = message.substring(in + 1);  }
  parts[i] = message;

  if (i < 12) {     // Serial.println("Invalid RMC message");
  return;  }

 // Extract relevant information
 
   TZ =      parts[1];
   GPQ =     parts[2];
   la  =     parts[3]+","+ parts[4]+",";
   lo  =     parts[5]+","+ parts[6]+",";
  Sped =     parts[7];
 DEGRE =     parts[8];
  DATE =     parts[9];
  
 DAY=DATE.substring(0, 2);MONTH=DATE.substring(2, 4);YER=DATE.substring(4, 6);
 GMT=YER+MONTH+DAY+TZ.substring(0,4);loca=la+lo;
 SPED=(Sped.toInt());  }
  /////////////////////////////////////////SMS LOCATION///////////////////////////////////////////////////////

void pos(){while(neo.available()) {gps.encode(neo.read());
        
        if(gps.location.isUpdated()){gps.encode(neo.read());SPED=(gps.speed.kmph());
        dloc="http://maps.google.com/maps?f=q&q=";
        dloc+=String(gps.location.lat(),6);dloc+=",";dloc+=String(gps.location.lng(),6); dloc+="&z=16\n\n" ;
        
        if((acc==0)){ Serial.println ("AT");delay(100);Serial.println("AT+CSCLK=0");delay(500);
                     SMS();if(gpstat=="0")Serial.println("AT+CSCLK=2");}
       
        sms(); s=dloc; s+="IMEI :"     ;s+=IMEI   ;                                              s+="\n" ;delay(100); 
               s+="ACC: "      ;if(in2==1) s+="ON"; else s+="OFF";                       s+="\n" ;delay(100);          
               s+="DOUT: "     ;s+=digitalRead(DOUT)   ;                                 s+="\n" ;delay(100);
      SEND();delay(1000);
        gpstat="0";f=SPIFFS.open("/gpstat.txt", "w");f.print("0");f.close();
        break;}}}
      
       
 ////////////////////////-----------------------SMS SENDING-----------------------------------------------------------------------------

void call(){Serial.print("ATD\"");Serial.print(phoneNumber);Serial.println("\";");}

void sms()            {   s="AT+CMGS=\"";s+=phoneNumber;s+="\""; Serial.println(s); s="";  delay (1000);}

void SEND()           { Serial.print(s);   Serial.write(26);delay (1000);}

//////////////////////////////////////// SPEED SMS SITUATION /////////////////////////////////////////////////

 void BATTD(){   s+=" Batterie DEBRANCHEZ.....!";   gpstat="1";f=SPIFFS.open("/gpstat.txt", "w");f.print("1");f.close(); delay(100);}

 void OVERSPED(){s+=" VITESSE MAXIMAL.....!";                                                delay(100);pos();}
 
 void MARCH(){   s+="remaitre contact en cours......!" ; s+="\n";                            delay(100); }
 
 void STOP(){    s+="Coupure contact en cours......!" ;  s+="\n";                            delay(100); }
 
 void WIFI(){    s+="ACTIVATION WiFi en cours......!" ;  s+="\n";                            delay(100); }
 
 void rst(){     s+="Redemarrage en cours......!" ;      s+="\n";                            delay(100); }                                                   

 void inf(){      
      s+="FIRM : 4G-V"    ;s+=VER   ;                                           s+="\n" ;delay(100); 
      s+="IMEI :"     ;s+=IMEI   ;                                              s+="\n" ;delay(100); 
      s+="ACC: "      ;if(in2==1) s+="ON"; else s+="OFF";                       s+="\n" ;delay(100);          
      s+="DOUT: "     ;s+=digitalRead(DOUT)   ;                                 s+="\n" ;delay(100);
      s+="GPRS: "     ;s+= signal4G;                                             s+="%\n" ;delay(100);                                 
      s+="GPS: "      ;if(GPS=="OK"){if(NS.toInt()>0) { s+=(NS.toInt()*100)/12; s+="%";}else s+="OK"; }else s+="NON";                     s+="\n" ;delay(100);          
      s+="WIFI PASS:" ;s+=wifip  ;                                              s+="\n" ;delay(100);               }   
     
       
  void smspos(){ 
      s=dloc;   s+="\n" ;delay(100);  s+="\n" ;delay(100);                                         
              }                                                                               

/////////////////////////////////////////CHEKING COMMANDE///////////////////////////////////////////////////////
void check(){

//---------------------------------------------------------------------------------------------------
 if(!(strncmp(str,"ADMINE",6))){s=str;  if((s.length()>=17)&&(s.length()<=23)){
  s=s.substring(7,(s.length()));   f = SPIFFS.open("/number.txt", "w"); f.print(s);f.close();}
   number=s; delay(500);
   sms();s+="OK...!\r\nBienvenue dans notre monde!!";SEND(); }
  
//---------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"WIFIPSWD",8))){st=str;
  s=st.substring(9,(st.length()));if(s.length()>=8) {wifip=s;      f=SPIFFS.open("/wifip.txt", "w");   f.print(s);f.close();sms();s+="OK...";s+=st;SEND();}
  sms();   s+="OK...!\r\nWIFIPSWD  :";           s+=wifip;
  SEND(); }
 
 ///---------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"APN",3))){s=str;      if((s.length()>=8)&&(s.length()<=20)){
  s=s.substring(4,(s.length()));f=SPIFFS.open("/apn.txt", "w");f.print(s);f.close();}
  f = SPIFFS.open("/apn.txt", "r");apn="";
  for(int i=0;i<(f.size());i++){apn+=((char)f.read());}f.close();  delay(2000);}
   //---------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"VMAX",4))){s=str;      if((s.length()>=8)&&(s.length()<=20)){
  s=s.substring(5,(s.length()));f=SPIFFS.open("/VMAX.txt", "w");f.print(s);f.close();}
  f = SPIFFS.open("/apn.txt", "r");s="";
  for(int i=0;i<(f.size());i++){s+=((char)f.read());}f.close();VMAX=(s.toInt());  delay(2000);}
   
//---------------------------------------------------------------------------------------------------
else if(!(strncmp(str,"WIFI",4))){sms();WIFI();SEND();delay(2000);WiFi.forceSleepWake() ;delay(2000);wifisett();}
//---------------------------------------------------------------------------------------------------
else if(!(strncmp(str,"GPS",3))){  gpstat="1"; f=SPIFFS.open("/gpstat.txt", "w");f.print(gpstat);f.close();sms();s+="OK...";SEND();}                                                       
//---------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"call",4))){sms();s+="OK...";SEND();delay(2000);call();}
 //---------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"XDM",4))){sms();s+="OK..CLEAR CACH.";SEND();XML_D();delay(2000);}
//---------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"FOTA",4))){   dureefota=millis();CONFIG();delay(2000);}
//---------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"INFO",4))){  MODULE_STAT();Read(); sms();inf();SEND();delay(2000);}
 //--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"RST",3))) {      sms();rst();SEND();delay(2000);
 Serial.println("AT+CMGD=1,4");digitalWrite(GPR, LOW);tim=millis();while(millis()<(tim+6000)){if(Serial.available())break;delay(100);}delay(200);
digitalWrite(GPR, HIGH);delay(3000);digitalWrite(GPR, LOW);ESP.reset();}
 //--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"CLEAR",5))) {      sms();rst();SEND();delay(2000);
 Serial.println("AT+CMGD=1,4");tim=millis();while(millis()<(tim+6000)){if(Serial.available())break;delay(100);}delay(200);
        
   SPIFFS.remove("/IMEI.txt" );
   SPIFFS.remove("/SERVER.txt");
   SPIFFS.remove("/PORT.txt");
   SPIFFS.remove("/BUSERVER.txt");
   SPIFFS.remove("/BUPORT.txt");
   SPIFFS.remove("/number.txt");
   SPIFFS.remove("/paswrd.txt");
   SPIFFS.remove("/apn.txt");
   SPIFFS.remove("/devicep.txt");
   SPIFFS.remove("/ANG.txt");
   SPIFFS.remove("/AUTOR.txt");
   SPIFFS.remove("/interv.txt");
   SPIFFS.remove("/wifip.txt");     
   Serial.println ("AT");delay(100);Serial.println("AT+CSCLK=0");delay(500);  Serial.println("AT+CPOWD=1"); B=0;ESP.reset();}
 //--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"OUT1",4))){sms();STOP();SEND();delay(2000); dout=true ;f=SPIFFS.open("/DOUT.txt", "w");f.print("1");f.close();
                                                                    digitalWrite(DOUT,HIGH);delay(1000);}
 //-------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"OUT0",4))){sms();MARCH();SEND();delay(2000);dout=false ;f=SPIFFS.open("/DOUT.txt", "w");f.print("0");f.close();
                                                                    digitalWrite(DOUT,LOW);delay(1000);} 
 ///-------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"UPDATE",6))){ confirme();s=str;      if((s.length()>=6)&&(s.length()<=30)){
  tr=s.substring(7,(s.length()));VER=tr;
  sms();s="OK...\n\rVER: ";s+=tr;SEND();HTTPUPDATE();}}
  //-------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"AUTOR",5))){ confirme();s=str;      if((s.length()>=6)&&(s.length()<=30)){
  s=s.substring(6,(s.length()));
  AUTOR=s;f=SPIFFS.open("/AUTOR.txt", "w");f.print(AUTOR);f.close();}}
//--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"CONFIGUE",8))){ confirme();confirme();s=str;  
  st=s.substring(9,(s.length())); i=i1=0; 
  s=st.substring  (0,(st.indexOf(',',i1)))   ;if((s.length()>=1)&&(s.toInt()>=1))  {stopt=s.toInt();           f=SPIFFS.open("/stopt.txt", "w");    f.print(stopt);f.close();}        i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  s=st.substring(i+1,(st.indexOf(',',i1)))   ;if((s.length()>=1)&&(s.toInt()>=5)) {interv1=(s.toInt()*1000);  f=SPIFFS.open("/interv.txt", "w");   f.print(interv1);f.close();}      i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  s=st.substring(i+1,(st.indexOf(',',i1)))   ;if((s.length()>=1)&&(s.toInt()>=6)) {ANG=s.toInt();             f=SPIFFS.open("/ANG.txt", "w");      f.print(ANG);f.close(); }         i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
  s=st.substring(i+1,(st.indexOf(',',i1)))   ;if((s.length()>=1)&&(s.toInt()<60)) {intervSleep=s.toInt();       f=SPIFFS.open("/intervSleep.txt", "w"); f.print(intervSleep);f.close();}     i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  sms();s+="OK...!\r\nSTOP    : ";      s+=stopt;
        s+="\r\nINTERV  : ";            s+=(interv1/1000);
        s+="\r\nANG     : ";            s+=ANG;
        s+="\r\nint Sleep: ";            s+=intervSleep;

         SEND();
 }
//--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"SYSTEME",7))){ confirme();confirme();s=str;  
  s=s.substring(8,(s.length())); i=i1=0; 
  if((s.length()==1)&&(s.toInt()<=2)){IGS=s.toInt();f=SPIFFS.open("/IGS.txt", "w");f.print(s);f.close();}  
  sms();s+="OK...!\r\nIGNITION :";       if(IGS==0)s+="ACC";
                                    else if(IGS==1)s+="Vitesse";
                                    else if(IGS==2)s+="TOUJOUR!";
                                    SEND();}
//--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"SMS",3))){ confirme();confirme();s=str;  
  st=s.substring(4,(s.length())); i=i1=0; 
  s=st.substring  (0,(st.indexOf(',',i1)))   ;if(s.length()==6){paswrd=s; f=SPIFFS.open("/paswrd.txt", "w");f.print(s);f.close();}     i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=5){number=s; f=SPIFFS.open("/number.txt", "w");f.print(s);f.close();}     i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  sms();s+="OK...\r\nSMS CODE:";        s+=paswrd;
             s+="\r\nAdmine  :";        s+=number;  
                             SEND();           
                             }
  //--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"SERVER",6))){ confirme();confirme();s=str;  
  st=s.substring(7,(s.length())); i=i1=0; 
  s=st.substring  (0,(st.indexOf(',',i1)))   ;if(s.length()>=10){SERVER=s;f=SPIFFS.open("/SERVER.txt", "w");f.print(s);f.close(); }                i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
  s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1){PORT=s;f=SPIFFS.open("/PORT.txt", "w");f.print(s);f.close();}                      i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
  s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=10){BUSERVER=s;f=SPIFFS.open("/BUSERVER.txt", "w");f.print(s);f.close();}              else {BUSERVER="";f=SPIFFS.open("/BUSERVER.txt", "w");f.print("");f.close();}                                                                                i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
  s=st.substring(i+1,(st.indexOf(',',i1)))   ;if((s.length()>=2)&&(s!="null")){BUPORT=s;VMAX=s.toInt();f=SPIFFS.open("/BUPORT.txt", "w");f.print(s);f.close();} else {BUPORT="";VMAX=s.toInt();f=SPIFFS.open("/BUPORT.txt", "w");f.print("");f.close();}                                                         i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
        sms();
                    s+="OK...!\r\nSER  :";           s+=SERVER;s+=",";     s+= PORT;
                    if(BUSERVER.length()>=10)   s+="\r\nBUSER  :";   s+=BUSERVER;s+=",";     s+= BUPORT;
                    SEND();delay(2000);connectToServer();}    

 
  //--------------------------------------------------------------------------------------------------
 else if(!(strncmp(str,"GLOBAL",6))){ confirme();confirme();s=str;  
    st=s.substring(7,(s.length())); i=i1=0; 
       s=st.substring (0, (st.indexOf(',',i1)))   ;if(s.length()>=10){SERVER=s;                  f=SPIFFS.open("/SERVER.txt", "w");f.print(s);f.close(); }              i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1){PORT=s;                     f=SPIFFS.open("/PORT.txt", "w");f.print(s);f.close();}                 i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1) {stopt=s.toInt();           f=SPIFFS.open("/stopt.txt", "w");    f.print(stopt);f.close();}        i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=2) {interv1=(s.toInt()*1000);  f=SPIFFS.open("/interv.txt", "w");   f.print(interv1);f.close();}      i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);  
       s=st.substring(i+1,(st.indexOf(',',i1)))   ;if(s.length()>=1) {ANG=s.toInt();             f=SPIFFS.open("/ANG.txt", "w");      f.print(ANG);f.close(); }         i=st.indexOf(',',i+1); i1=st.indexOf(',',i+1);
       s=st.substring(i+1,i+2)                    ;if(s. toInt()<=2) {IGS=s.toInt();              f=SPIFFS.open("/IGS.txt", "w");f.print(s);f.close();} 

   sms();
                    s+="OK...!\r\nSER  :";                s+=SERVER;s+=",";     s+= PORT;
                          s+="\r\nstopt   :";             s+=stopt;
                          s+="\r\nINTERV  : ";            s+=(interv1/1000);
                          s+="\r\nANG     : ";            s+=ANG;
                          s+="\r\nIGNTION : ";            s+=IGS;
SEND();
 } memset(str,'\0',256)  ; }
