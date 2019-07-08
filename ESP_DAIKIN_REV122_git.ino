

//  Version Review:
#define REVISIONPROG "1.22 2019-05-01" 

// Rev. 1.22
// Actualização livraria https://github.com/markszabo/IRremoteESP8266 funcionamento total Send\Receive v2.6.0 revision Dainkin216Tweaks
// Não envia funçoes de de tempos, cuurenttime, ontime, offtime 

// Rev. 1.19
// PIR - Adicional delay na transmissão de mensagens

// Rev. 1.18
// MQTT start para alem de tempo por voltas loop  startcount++;
// int cor_min (int min_ ) // Caso recebe MQTT min superior a (23:59) 1439 converte para inferior 
// quando solicitado publica startup ON - client.publish((topic_client + "startup").c_str() , "ON");
// Update JSON v6
// IRremoteESP8266 Pass: 2.54

// Rev. 1.17
// Actualização libraria e programa DHT Sensor

// Rev. 1.16 
// Correcção Ajax input html Expand document.getElementById('Cip_01').defaultValue

// Rev. 1.15
// Restruturacao file connfig, introducao JSON estrutura https://github.com/bblanchon/ArduinoJson

// Rev. 1.14 
// Restruturação de todo o WIFI - Wifimanager Autoconnect setup_wifi()  https://github.com/tzapu/WiFiManager#custom-ip-configuration

// Rev. 1.13
// Restruturação Wifi Connect e Reconnect
// Optimizacao Programa remocao Strings

// Rev. 1.12
// Protocolo IR Receiver Compativel com ACR433 (Protcolo old 2016 https://github.com/danny-source/Arduino_DY_IRDaikin)
// Adicionado contador
// Alteracao \ optimizacao da programacao Pubsubclient
// Corecção MDNS http://Ipadress.local ou http://hostname.local

// Rev. 1.11
// Procolo MQTT restruturado (Se Recebe -> Transmite a confirmar)
// Remocao do MQQT Word Send

// Rev. 1.10
// Actualização XML para não haver erros
// Actualização protocolo MQTT
// Criacao de pagina para alterar offline

// Rev. 1.0.9
// Incluir OTA Password:arduino

// Mode 0= Auto kDaikinAuto \ 1=  \ 2=Dry kDaikinDry\ 3= Cool kDaikinCool\ 4=Heat kDaikinHeat\ 5= \ 6=Fan kDaikinFan
// Fan  1=1 MIN \2=2 \3=3 \4=4 \ 5=5 MAX \ 10 = Auto \ 11 = Quiet   

//  You MUST change <PubSubClient.h> to have the following (or larger) value:
//  #define MQTT_MAX_PACKET_SIZE 512

 /*
  void begin();
  void on(void);
  void off(void);
  void setPower(const bool on);
  bool getPower(void);
  void setTemp(const uint8_t temp);
  uint8_t getTemp();
  void setMode(const uint8_t mode);
  uint8_t getMode(void);
  static uint8_t convertMode(const stdAc::opmode_t mode);
  void setFan(const uint8_t fan);
  uint8_t getFan(void);
  static uint8_t convertFan(const stdAc::fanspeed_t speed);
  void setSwingVertical(const bool on);
  bool getSwingVertical(void);
  void setSwingHorizontal(const bool on);
  bool getSwingHorizontal(void);
  void setQuiet(const bool on);
  bool getQuiet(void);
  
  */


#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h> //  IRREMOTEESP8266
//#include <DYIRDaikin.h> // DYIRDaikin
#include <IRrecv.h>
#include <IRutils.h>
#include "DHT.h"
#include <ir_Daikin.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h> 
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

//Include the HTML, STYLE and Script "Pages"
#include "html.h"

// Declarar Sensor temperatura e Humidade
float humidade = NULL;
float temperatura = NULL;
float humidade_test = NULL;
float temperatura_test = NULL;
float humidade_ok = NULL;
float temperatura_ok = NULL;
int DHT_retries=0; 
float Temp_factor = 1.000;
float Hum_factor = 1.000;
float Lux_factor = 1.000;
float lasttemperatura = 0;

unsigned long lastMillis1 = 0;
unsigned long lastMillis11 = 0;

unsigned long lastMillis2 = 0;
unsigned long lastMillis3 = 0; 
unsigned long lastMillisstart = 0;
unsigned long lastMillis4 = 0;
unsigned long lastMillisdelayIR = 0;
unsigned long lastMilliscomando = 0;
unsigned long lastMillisPIR = 0;

uint32_t lastDisconnectedTimeMQTT = 0;
uint32_t DisconnectedMQTTcount = 0;

// Declarar MQTT e Wifi
#define SECRET_SSID "STDHOME" 
#define SECRET_PASS "123456"
#define SERVER_IP "192.168.1.110"
#define MQTT_CLIENT "HVAC/TESTE/"
#define MQTT_CLIENT1 "hvac_teste"

#define DHTPIN 5 // Pin which is connected to the DHT sensor.
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);


#define PIRPIN 12

// Uma placa tem este PIN
//#define PIRPIN 13

// Wifi Settings  arduino_secrets.h //
char ssid[30] =  SECRET_SSID;      // your network SSID (name)
char pass[30] =  SECRET_PASS;   // your network password
char serverip[30] =  SERVER_IP; // your raspberry IP
String topic_client = MQTT_CLIENT; 
char Hostname[30] = MQTT_CLIENT1;

// MQTT 
WiFiClient espClient;
PubSubClient client(espClient);
boolean transmiteMQTT = false;
int startcount = 0;

//MDNSResponder mdns;
int status = WL_IDLE_STATUS;
WiFiClient net;
ESP8266WebServer server(80);
IPAddress ip;
String stringip = "NULL";
//int iapoint = 0;
//int apoint_status = 0;
//String apoint_mode = "Client Mode";
boolean ir_daikin_check = false;


// Variáveis html
boolean html_pass_write = false;
boolean html_mem_grava = false;

// Configuration that we'll store on disk
struct Config {
  char M_Cip_01[5];
  char M_Cip_02[5];
  char M_Cip_03[5];
  char M_Cip_04[5];
  char M_MQTT_client[30];
  char M_hostname[30];
  char M_Temp_factor[10];
  char M_Hum_factor[10];
  char M_Lux_factor[10];
};

Config config; // <- global configuration object


char Pass_verifica[10];


// pagina web
String html_mqtt_client = "";
String html_ir_receive = "";
String html_file_status ="FILE INIT.......";
String html_rssi_color = "";

// Variáveis HVAC
// OLD Power: On, Mode: 0 (AUTO), Temp: 25C, Fan: 10 (AUTO), Powerful: Off, Quiet: Off, Sensor: Off, Eye: On, Mold: Off, Swing (Horizontal): Off, Swing (Vertical): Off, Current Time: 14:52, On Time: Off, Off Time: Off
// >rev 1.22 Mesg Desc.: Power: On, Mode: 0 (AUTO), Temp: 22C, Fan: 10 (AUTO), Swing (Horizontal): Off, Swing (Vertical): Off, Quiet: Off

 
boolean power = false;
uint8_t Mode = 0;
uint8_t Temp = 20;
uint8_t Fan = 10;
boolean Powerful = false;
boolean Quiet = false;
boolean Sensor = false;
boolean Eye  = false;
boolean Mold  = false;
boolean Econo = false;
boolean SwingVertical = false;
boolean SwingHorizontal = false;
uint16_t CurrentTime;
uint16_t OnTimerT; // on timer Time
boolean OnTimerS  = false; // on timer Status
uint16_t OffTimerT; // off timer Time
boolean OffTimerS  = false; // off timer Status

String modestring="";
String fanstring="";

// estruutura MQTT receive AC
// Se não funcionar alterar: #define MQTT_MAX_PACKET_SIZE 512
char payloadAC[256];
bool MQTT_R_AC_OK = false;
boolean MQTT_R_SENDIR = false;

// iluminação
int iluminacao =0;
int lastiluminacao=0;

// PIR
boolean PIR_Status = false; 
boolean PIR_Last = false;

//FUNCAO ENVIO *************************************************
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
//IRDaikinESP daikinir_send(kIrLed);  // Set the GPIO to be used to sending the message
IRDaikin216 daikinir_send(kIrLed);  // Set the GPIO to be used to sending the message

//FUNCAO ENVIO *************************************************


//FUNCAO RECEIVER LEITURA IRREMOTEESP8266*************************************************
// start of TUNEABLE PARAMETERS ====================
// An IR detector/demodulator is connected to GPIO pin 14
// e.g. D5 on a NodeMCU board.
const uint16_t kRecvPin = 14;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;

uint32_t irRecvCounter = 0;
uint32_t irSendCounter = 0;

#if DECODE_AC
const uint8_t kTimeout = 50; //50
#else  // DECODE_AC
// Suits most messages, while not swallowing many repeats.
const uint8_t kTimeout = 15;
#endif  // DECODE_AC
const uint16_t kMinUnknownSize = 2* 10;
// end of TUNEABLE PARAMETERS ====================

// Use turn on the save buffer feature for more complete capture coverage.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);

decode_results results;  // Somewhere to store the results
//IRDaikinESP daikinir_rec(0);
IRDaikin216 daikinir_rec(0);
// Display the human readable state of an A/C message if we can.
void dumpACInfo(decode_results *results) {
  String description = "";
#if DECODE_DAIKIN216
  if (results->decode_type == DAIKIN216) {
    daikinir_rec.setRaw(results->state);
    description = daikinir_rec.toString();
    ir_daikin_check = true;
    irRecvCounter = irRecvCounter + 1;
  }
  else ir_daikin_check = false;
#endif  // DECODE_DAIKIN216

  // If we got a human-readable description of the message, display it.
  if (description != "")  
  {
    Serial.println("Mesg Desc.: " + description);
    html_ir_receive= String(description); 
  }
  
}
//FUNCAO RECEIVER LEITURA  IRREMOTEESP8266 *************************************************


//FUNCAO RECEIVER LEITURA DYIRDaikin *************************************************

//#define DYIRDAIKIN_SOFT_IR
//DYIRDaikin irdaikin;

//FUNCAO RECEIVER LEITURA DYIRDaikin *************************************************

//xxxxxxx CONNECT WIFI MQTT xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//Nova Versão Rev. 1.12

boolean reconnect() {

// Wifi Funcao

int wifistart =0;   
  Serial.print("checking wifi...");
  if(WiFi.status() != WL_CONNECTED)
  {
    while (WiFi.status() != WL_CONNECTED) {
     Serial.print(".");
     //status = WiFi.begin(ssid, pass);
     delay(2000);
     wifistart=1;
    }
    Serial.println("*");
  }
  else
  {
   Serial.println("OK");
  }
   if(wifistart == 1)
  {
   server.begin();
   stringip = WiFi.localIP().toString();
  }


// MQTT Funcao
  

    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = Hostname;
    clientId += "-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...

      ip = WiFi.localIP();
      client.publish((topic_client + "online").c_str() , "ON");
     

      
      
      // ... and resubscribe
        client.subscribe(topic_client.c_str());
        client.subscribe((topic_client + "online/set").c_str());
        client.subscribe((topic_client + "localip/set").c_str());
        client.subscribe((topic_client + "Power/set").c_str());
        client.loop();
        client.subscribe((topic_client + "SENSORTEMP/set").c_str());
        client.subscribe((topic_client + "SENSORHUM/set").c_str());
        client.subscribe((topic_client + "Mode/set").c_str());
        client.subscribe((topic_client + "Setpoint/set").c_str());
        client.loop();
        client.subscribe((topic_client + "Fan/set").c_str());
        client.subscribe((topic_client + "Powerful/set").c_str());
        client.subscribe((topic_client + "Quiet/set").c_str());
        client.subscribe((topic_client + "CurrentTime/set").c_str());
        client.loop();
        client.subscribe((topic_client + "OnTimerT/set").c_str());
        client.subscribe((topic_client + "OnTimerS/set").c_str());
        client.subscribe((topic_client + "OffTimerT/set").c_str());
        client.subscribe((topic_client + "OffTimerS/set").c_str());
        client.loop();
        client.subscribe((topic_client + "LUX/set").c_str());
        client.subscribe((topic_client + "Send/set").c_str());
       // client.subscribe((topic_client + "Comando/set").c_str());
        client.subscribe((topic_client + "PIR/set").c_str());
 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again");
    }

 
  
  return client.connected();
}


/* 
 *  -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
 *  -3 : MQTT_CONNECTION_LOST - the network connection was broken
 *  -2 : MQTT_CONNECT_FAILED - the network connection failed
 *  -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
 *   0 : MQTT_CONNECTED - the client is connected
 *   1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
 *   2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
 *   3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
 *   4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
 *   5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
 */
 
//xxxxxxx MQTT Arrive xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadstring = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] "); 
  
  for (int i = 0; i < length; i++) {
    payloadAC[i] = payload[i];
    Serial.print((char)payload[i]);
    payloadstring = payloadstring + (char)payload[i];
   }
    Serial.println();
    
   if(String(topic).startsWith(topic_client))
   {
    boolean enviarIR = true; 
    String topic1 = String(topic);
     
     //  mqttarrive(topic,payload,length)
     String ACmode ="";
     String ACvalue ="";
     ACvalue = payloadstring;
   

     // envia configurações para MQTT
     if (topic1.equals(topic_client + "online/set"))
     {  
        client.publish((topic_client + "startup").c_str() , "ON");
        client.publish((topic_client + "localip").c_str(), stringip.c_str() ); 
        client.publish((topic_client + "online").c_str() , "ON");
        delay(50);
        client.publish((topic_client + "SENSORTEMP").c_str(), (String(temperatura)).c_str());
        client.publish((topic_client + "SENSORHUM").c_str() , (String(humidade)).c_str()); 
        client.publish((topic_client + "LUX").c_str() , (String(iluminacao)).c_str());  
        client.publish((topic_client + "PIR").c_str() , (maponoff_(PIR_Status)).c_str());
        delay(50);
        client.publish((topic_client + "Comando").c_str(), String("ON").c_str());
        client.publish((topic_client + "Power").c_str(), (maponoff_(power)).c_str());
        client.publish((topic_client + "Mode").c_str(), (String(modestring)).c_str());  
        client.publish((topic_client + "Setpoint").c_str(), (String(Temp)).c_str()); 
        delay(50);
        client.publish((topic_client + "Fan").c_str(), (String(fanstring)).c_str());
        client.publish((topic_client + "Powerful").c_str(), (maponoff_(Powerful)).c_str());
        client.publish((topic_client + "Quiet").c_str(), (maponoff_(Quiet)).c_str());
        //client.publish((topic_client + "CurrentTime").c_str(), (String(CurrentTime)).c_str());
        client.publish((topic_client + "OnTimerT").c_str(), (String(OnTimerT)).c_str());
        delay(50);
        client.publish((topic_client + "OnTimerS").c_str(), (maponoff_(OnTimerS)).c_str());
        client.publish((topic_client + "OffTimerT").c_str(), (String(OffTimerT)).c_str());
        client.publish((topic_client + "OffTimerS").c_str(), (maponoff_(OffTimerS)).c_str());    
             
         enviarIR = false;
     }

     else if (topic1.equals(topic_client + "SENSORTEMP/set"))
       {client.publish((topic_client + "SENSORTEMP").c_str(), (String(temperatura)).c_str());
       enviarIR = false;}
     else if (topic1.equals(topic_client + "SENSORHUM/set"))
       {client.publish((topic_client + "SENSORHUM").c_str() , (String(humidade)).c_str());  
       enviarIR = false;}
     else if (topic1.equals(topic_client + "LUX/set"))
       {client.publish((topic_client + "LUX").c_str() , (String(iluminacao)).c_str());  
       enviarIR = false;}   
     else if (topic1.equals(topic_client + "PIR/set"))
       {client.publish((topic_client + "PIR").c_str() , (maponoff_(PIR_Status)).c_str());    
       enviarIR = false;}
    // else if (String(topic) == String(topic_client + "Comando/set"))
    //  {
     //   enviarIR = false;
     //     client.publish((topic_client + "Comando").c_str(), String(payloadstring).c_str()); 
    //  }
     else if (topic1.equals(topic_client + "localip/set"))
       {client.publish((topic_client + "localip").c_str(), stringip.c_str() ); 
       enviarIR = false;} 
     ////////  
       else if (topic1.equals(topic_client + "CurrentTime/set"))
         {
          CurrentTime = cor_min(ACvalue.toInt());
          client.publish((topic_client + "CurrentTime").c_str(), (String(CurrentTime)).c_str());
          enviarIR = false;
         }
        else if (topic1.equals(topic_client + "OnTimerT/set"))
         {
          OnTimerT = cor_min(ACvalue.toInt());
          client.publish((topic_client + "OnTimerT").c_str(), (String(OnTimerT)).c_str());
          enviarIR = false;
         }
        else if (topic1.equals(topic_client + "OnTimerS/set"))
         {
          if (ACvalue == "ON")
           OnTimerS = true;
          else if (ACvalue == "OFF")
           OnTimerS = false;
          client.publish((topic_client + "OnTimerS").c_str(), (maponoff_(OnTimerS)).c_str());
          enviarIR = false;
         }
        else if (topic1.equals(topic_client + "OffTimerT/set"))
        {
         OffTimerT = cor_min(ACvalue.toInt());
         client.publish((topic_client + "OffTimerT").c_str(), (String(OffTimerT)).c_str());
         enviarIR = false;
         }
        else if (topic1.equals(topic_client + "OffTimerS/set"))
        {
         if (ACvalue == "ON")
          OffTimerS = true;
         else if (ACvalue == "OFF")
          OffTimerS = false;
         client.publish((topic_client + "OffTimerS").c_str(), (maponoff_(OffTimerS)).c_str());  
         enviarIR = false;  
         }  
      

    
    Serial.print(enviarIR);
    Serial.print(" :: ");
    Serial.print(topic);
    Serial.print(" : ");
    Serial.println(payloadstring);
    if (enviarIR == true)
     {
         MQTT_R_AC_OK = true;
         publish_comando();
         delay(100);
         //Serial.println(payloadAC);
         //Serial.println("Payload");
 
        if (topic1.equals(topic_client + "Power/set"))
         {
          if (ACvalue == "ON")
          power = true;
          else if (ACvalue == "OFF")
          power = false;
          client.publish((topic_client + "Power").c_str(), (maponoff_(power)).c_str());
         } 
        else if (topic1.equals(topic_client + "Mode/set"))
         { 
          // Mode = 0;
          if(ACvalue == "AUTO")
           Mode = 0;
          else if (ACvalue == "DRY")
           Mode = 2;
          else if (ACvalue == "COOL")
           Mode = 3;
          else if (ACvalue == "HEAT")
           Mode = 4;
          else if (ACvalue == "FAN")
           Mode = 6;   
          modestring=stringmode_(Mode);    
          client.publish((topic_client + "Mode").c_str(), (String(modestring)).c_str());  
         }
        else if (topic1.equals(topic_client + "Setpoint/set"))
         {
          int Acvalueint = ACvalue.toInt();
          if( Acvalueint > 16 && Acvalueint < 31)
          Temp = ACvalue.toInt(); // payloadstring; //.toInt()
          client.publish((topic_client + "Setpoint").c_str(), (String(Temp)).c_str()); 
         }
        else if (topic1.equals(topic_client + "Fan/set"))
         {
          Fan = 10;
          if(ACvalue == "MIN" || ACvalue == "LOW")
           Fan = 1;
          else if(ACvalue == "MIDDLE")
           Fan = 3;
          else if(ACvalue == "MAX" || ACvalue == "HIGH")
           Fan = 5;
          else if(ACvalue == "AUTO")
           Fan = 10;
          else if(ACvalue == "SILENT")
           Fan = 11;      
          fanstring = stringfan_(Fan);     
          if(Fan == 11)
           Quiet = true;
          else
           Quiet = false;          
          client.publish((topic_client + "Fan").c_str(), (String(fanstring)).c_str());
         }
        else if (topic1.equals(topic_client + "Powerful/set"))
         {
          if (ACvalue == "ON")
           Powerful = true;
          else if (ACvalue == "OFF")
           Powerful = false;
          client.publish((topic_client + "Powerful").c_str(), (maponoff_(Powerful)).c_str());
         }
        else if (topic1.equals(topic_client + "Quiet/set"))
         {
          if (ACvalue == "ON")
           Quiet = true;
          else if (ACvalue == "OFF")
           Quiet = false;        
          client.publish((topic_client + "Quiet").c_str(), (maponoff_(Quiet)).c_str());
         }

    } 
     }  
      
 
   
}

//xxxxxxx SETUP WIFI xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void setup_wifi() {

  
  delay(10);
  // We start by connecting to a WiFi network

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
   WiFiManager wifiManager;

  //set static ip
  wifiManager.setAPStaticIPConfig(IPAddress(192,168,0,1), IPAddress(192,168,0,1), IPAddress(255,255,255,0));

  wifiManager.setTimeout(300);  // Time out after 5 mins.
  //reset settings - for testing
  //wifiManager.resetSettings();

  if (!wifiManager.autoConnect()) {
    Serial.println("WIFI Falha ligacao");
    delay(3000);
    // Reboot. A.k.a. "Have you tried turning it Off and On again?"
    wifiManager.resetSettings();
    delay(10);
    ESP.restart();
    delay(1000);
   // ESP.reset();
  //  delay(5000);
  }
  WiFi.hostname(Hostname);

}

//xxxxxxx FILE SYSTEM READ xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void file_read () {

    // Inicia File função leitura
      if(SPIFFS.begin()){
      Serial.println("SPIFFS System OK 1.2");
      File f = SPIFFS.open("/conf.txt", "r");
      if (!f) 
          {
            Serial.println("ERROR: File doesn't exist.");
            html_file_status ="ERROR: File doesn't exist."; 
            File f = SPIFFS.open("/conf.txt", "w");
            f.close();
          } else {
                  //size_t size = f.size();
                  // Allocate a buffer to store contents of the file.
                  //std::unique_ptr<char[]> buf(new char[size]);

                  //f.readBytes(buf.get(), size);
                  //DynamicJsonBuffer jsonBuffer;
                  
                  //JsonObject& json = jsonBuffer.parseObject(buf.get());
                  //json.printTo(Serial);

                  const size_t capacity = JSON_OBJECT_SIZE(12) + 200;                  
                  DynamicJsonDocument json(capacity);
                  DeserializationError jsonerror = deserializeJson(json, f);
                  serializeJson(json, Serial);
                  
                  if (jsonerror) {
                    Serial.println("failed to load json config");
                     html_file_status ="File Failed load json config (Default)";
                  } else {
                     Serial.println("\nparsed json");
                     html_file_status ="File Read OK 2.2";
                  }

                  // Ler Dados
                  strlcpy(config.M_Cip_01,           // <- destination
                  json["cip01"] | "192",  // <- source
                  sizeof(config.M_Cip_01));          // <- destination's capacity
                  strlcpy(config.M_Cip_02,           // <- destination
                  json["cip02"] | "168",  // <- source
                  sizeof(config.M_Cip_02));          // <- destination's capacity
                  strlcpy(config.M_Cip_03,           // <- destination
                  json["cip03"] | "0",  // <- source
                  sizeof(config.M_Cip_03));          // <- destination's capacity
                  strlcpy(config.M_Cip_04,           // <- destination
                  json["cip04"] | "11",  // <- source
                  sizeof(config.M_Cip_04));          // <- destination's capacity
                  strlcpy(config.M_MQTT_client,           // <- destination
                  json["mqttclient"] | MQTT_CLIENT,  // <- source
                  sizeof(config.M_MQTT_client));          // <- destination's capacity
                  strlcpy(config.M_hostname,           // <- destination
                  json["hostname"] | MQTT_CLIENT1,  // <- source
                  sizeof(config.M_hostname));          // <- destination's capacity
                  strlcpy(config.M_Temp_factor,           // <- destination
                  json["tempfactor"] | "1.000",  // <- source
                  sizeof(config.M_Temp_factor));          // <- destination's capacity
                  strlcpy(config.M_Hum_factor,           // <- destination
                  json["humfactor"] | "1.000",  // <- source
                  sizeof(config.M_Hum_factor));          // <- destination's capacity
                  strlcpy(config.M_Lux_factor,           // <- destination
                  json["luxfactor"] | "1.000",  // <- source
                  sizeof(config.M_Lux_factor));          // <- destination's capacity
                                                                                                             
                      //strcpy(mqtt_server, json["mqtt_server"]);   
                  //json.printTo(Serial);
                  serializeJson(json, Serial);
            }
             Serial.println();
             f.close();    
      }
       else {
        html_file_status ="File Failed SPIFFS MEM";
       }

}



// SETUP ***********************************************************************SETUP *********************
// The section of code run only once at start-up.
void setup() {

  Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
  pinMode(PIRPIN, INPUT);

    Serial.println(F("DHTxx test!"));
    dht.begin();
  

  lastMillisstart = millis();
  lastDisconnectedTimeMQTT = millis();
  
  //while (!Serial)  // Wait for the serial connection to be establised.
  //  delay(50);
  //Serial.println();

  // Inicia IR Envio Daikin
  daikinir_send.begin(); // IRREMOTEESP8266

  file_read();
 

  // Copia File M para variaveis globais

       //strncpy(ssid, M_CSSID, 30);    
       //strncpy(pass, M_CSSIDP, 30);
 
       String stringips = String (config.M_Cip_01);
       stringips = stringips + ".";
       stringips = stringips + String (config.M_Cip_02);
       stringips = stringips + ".";
       stringips = stringips + String (config.M_Cip_03);
       stringips = stringips + ".";
       stringips = stringips + String (config.M_Cip_04);
       stringips.toCharArray(serverip, 30);
       //strncpy(pass, M_CSSIDP, 30);
       topic_client = String(config.M_MQTT_client); 
       strncpy(Hostname, config.M_hostname, 30);  
       Temp_factor = atof(config.M_Temp_factor);
       Hum_factor = atof(config.M_Hum_factor);
       Lux_factor = atof (config.M_Lux_factor);
       if ( Temp_factor == 0 || Temp_factor == NULL)
        Temp_factor = 1.000;
       if ( Hum_factor == 0 || Hum_factor == NULL)
        Hum_factor = 1.000;
       if ( Lux_factor == 0 || Lux_factor == NULL)
        Lux_factor = 1.000;
       
      //Serial.println(Temp_factor);
      

//FUNCAO RECEIVER LEITURA IRREMOTEESP8266*************************************************
  Serial.print("IRrecvDumpV2 is now running and waiting for IR input on Pin ");
  Serial.println(kRecvPin);
#if DECODE_HASH
  // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(kMinUnknownSize);
#endif  // DECODE_HASH
  irrecv.enableIRIn();  // Start the receiver*/
  
//FUNCAO RECEIVER LEITURA IRREMOTEESP8266 *************************************************

//FUNCAO RECEIVER LEITURA DYIRDAIKIN*************************************************
//  irdaikin.begin();
//  irdaikin.decodePin(kRecvPin);

//FUNCAO RECEIVER LEITURA DYIRDAIKIN*************************************************

 // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname(Hostname);

  // No authentication by default
   ArduinoOTA.setPassword("arduino");
  
  // attempt to connect to Wifi network:
   setup_wifi();
 

  // you're connected now, so print out the status:
  printWifiStatus();
  
   stringip = WiFi.localIP().toString();
   ip = WiFi.localIP();

   ArduinoOTA.begin();
  
  //http server

  if (!MDNS.begin(Hostname, ip)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  else
  {
  Serial.println("mDNS responder started");

  }

  MDNS.addService("http", "tcp", 80);
  server.on("/", handleRoot);
  server.on("/GET", handlehtmlget);
  server.on("/setBOT", handlehtmlbotoes);
 
  server.on("/xml", handleXML);
  server.on("/expand", expandhtml);
  server.on("/inline", []() {
    server.send(200, "text/plain", "ESP8266 START");
  });
  server.onNotFound(handleNotFound);
  server.begin(); // http server
  Serial.println("HTTP server started");
  
  //mqtt server
  client.setServer(serverip, 1883);
  client.setCallback(callback);
  randomSeed(micros());
  // old para arduino -> client.begin(serverip, net); // Broker connect
 // old para rduino -> client.onMessage(messageReceived);
 reconnect();
 
 Serial.println("ESP8266 START");
 delay(4000);
}


//****** LOOP ************************************************************************LOOP *************
// The repeating section of the code
//
void loop() {

//Html Server
server.handleClient();  

// mdns.update();

ArduinoOTA.handle();

// Inicio MQTT

  if (!client.connected()) {
    if (millis() - lastDisconnectedTimeMQTT > 10000) {
      Serial.println("Falha MQTT Ligacao");
      Serial.print("failed, rc=");
      Serial.println(client.state());      
      lastDisconnectedTimeMQTT = millis();
      DisconnectedMQTTcount= DisconnectedMQTTcount + 1;
      // Attempt to reconnectreconnect
      if (reconnect()) {
        printWifiStatus();
      }
    }
  } else {
    // Client connected
    client.loop();
  } 

// Função Sensor Temperatura e Humidaed e LDR

    if (millis() - lastMillis11 > 3000)
    {
     lastMillis11 = millis();
     humidade_test = dht.readHumidity();
     temperatura_test = dht.readTemperature();
     DHT_retries = DHT_retries + 1;
      if(!isnan(humidade_test) && !isnan(temperatura_test))
      {
        humidade_ok = humidade_test * Hum_factor;
        temperatura_ok = temperatura_test * Temp_factor;
        DHT_retries = 0;
       }
      if ( DHT_retries > 6)
       {
        Serial.println("DHT Error");
        humidade_ok = NULL;
        temperatura_ok = NULL;
        }
    }

  if (millis() - lastMillis1 > 10000 && MQTT_R_AC_OK == false) {
    lastMillis1 = millis();
      humidade = humidade_ok;
      temperatura = temperatura_ok;
     
     // iluminação
     //Lux=(500*(3,3-Vout))/(10*Vout)
     //Vout=Analogreading*0.0048828125
     int ilum = analogRead(A0);
     double Vout=ilum*0.0032227;
     double ilumx = ((1650/(10*Vout))-50) * Lux_factor;
     if( ilum <= 0 || ilum > 2000)
       iluminacao=0;
     iluminacao = (int) ilumx;    
   }

   PIR_Status = digitalRead(PIRPIN);
  

//FUNCAO RECEIVER LEITURA IR IRREMOTEESP8266*************************************************
  // Check if the IR code has been received.
  if (irrecv.decode(&results)) {  
    if (results.overflow)
      Serial.printf("WARNING: IR code is too big for buffer (>= %d). "
                    "This result shouldn't be trusted until this is resolved. "
                    "Edit & increase kCaptureBufferSize.\n",
                    kCaptureBufferSize);
    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));
    html_ir_receive = String(resultToHumanReadableBasic(&results)); // webpage
     
    dumpACInfo(&results);  // Display any extra A/C info if we have it.
    yield();  // Feed the WDT as the text output can take a while to print.

    
  if (ir_daikin_check == true && MQTT_R_AC_OK == false) {
    ir_daikin_check = false;
    
    client.publish((topic_client + "Comando").c_str(), String("1").c_str());

      power = daikinir_rec.getPower(); // boolean
      client.publish((topic_client + "Power").c_str(), (String(power)).c_str());
   
      Mode = daikinir_rec.getMode(); //uint8_t
      modestring=stringmode_(Mode);
      client.publish((topic_client + "Mode").c_str(), (String(modestring)).c_str());
    
    Temp = daikinir_rec.getTemp(); //uint8_t
    client.publish((topic_client + "Setpoint").c_str(), (String(Temp)).c_str());
    // Fan  1=1 MIN \2=2 \3=3 \4=4 \ 5=5 MAX \ 10 = Auto \ 11 = Quiet   
    Fan = daikinir_rec.getFan(); //uint8_t
    fanstring = stringfan_(Fan);
  
    
    client.publish((topic_client + "Fan").c_str(), (String(fanstring)).c_str());

    Quiet = daikinir_rec.getQuiet(); // boolean
    client.publish((topic_client + "Quiet").c_str(), (String(Quiet)).c_str());    
    Powerful = daikinir_rec.getPowerful(); // boolean
    client.publish((topic_client + "Powerful").c_str(), (String(Powerful)).c_str());

    
  /*  
 // 

//    Sensor = daikinir_rec.getSensor(); // boolean
    client.publish((topic_client + "Sensor").c_str(), (String(Sensor)).c_str());
//    Eye  = daikinir_rec.getEye(); // boolean
    client.publish((topic_client + "Eye").c_str(), (String(Eye)).c_str());
//    Mold  = daikinir_rec.getMold(); // boolean
    client.publish((topic_client + "Mold").c_str(), (String(Mold)).c_str());
    SwingVertical = daikinir_rec.getSwingVertical(); // boolean
    client.publish((topic_client + "SwingVertical").c_str(), (String(SwingVertical)).c_str());
    SwingHorizontal = daikinir_rec.getSwingHorizontal(); // boolean
    client.publish((topic_client + "SwingHorizontal").c_str(), (String(SwingHorizontal)).c_str());
//    CurrentTime = daikinir_rec.getCurrentTime(); // uint16_t
    client.publish((topic_client + "CurrentTime").c_str(), (String(CurrentTime)).c_str());
//    OnTimerS  = daikinir_rec.getOnTimerEnabled();  // boolean
    client.publish((topic_client + "OnTimerS").c_str(), (String(OnTimerS)).c_str());
    if (OnTimerS == true){
    OnTimerT = daikinir_rec.getOnTime(); // uint16_t
    client.publish((topic_client + "OnTimerT").c_str(), (String(OnTimerT)).c_str());
    }
    OffTimerS  = daikinir_rec.getOffTimerEnabled();  // boolean
    client.publish((topic_client + "OffTimerS").c_str(), (String(OffTimerS)).c_str());
    if (OffTimerS == true){
    OffTimerT = daikinir_rec.getOffTime(); // uint16_t
    client.publish((topic_client + "OffTimerT").c_str(), (String(OffTimerT)).c_str());
    }*/
  }
    
  
  }
//FUNCAO RECEIVER LEITURA IR IRREMOTEESP8266*************************************************

//FUNCAO RECEIVER LEITURA DYIRDAIKIN*************************************************
 /* if (irdaikin.decode() == 1) {
    ir_daikin_check = true;
    irRecvCounter = irRecvCounter + 1;
    
    irdaikin.description();

  }


  if (ir_daikin_check == true && MQTT_R_AC_OK == false) {
    ir_daikin_check = false;
    publish_comando();
    

     if( irdaikin.getPower() == 1) //uint8_t
      power = true;
      else if ( irdaikin.getPower() == 0 ) //uint8_t
      power = false;
    client.publish((topic_client + "Power").c_str(), (maponoff_(power)).c_str());

      //DYIRDAIKIN       0 FAN 1 COOL 2 DRY 3 HEAT 4 Auto
      // IRREMOTEESP8266 6 FAN 3 COOL 2 DRY 4 HEAT 0 Auto
    switch (irdaikin.getMode()) { //uint8_t 
    case 0:
      Mode = 6;
      break;
    case 1:
      Mode = 3;
      break;
    case 2:
      Mode = 2;
      break;
    case 3:
      Mode = 4;
      break;
    case 4:
      Mode = 0;
      break;   
    default:
      Mode = 0;
      break;
     }
      modestring=stringmode_(Mode);
      client.publish((topic_client + "Mode").c_str(), (String(modestring)).c_str());

    // DYIRDAIKIN       0~4 speed,5 auto,6 moon
    // IRREMOTEESP8266  1~5 speed, 10 auto, 11 Silent
    // IRREMOTEESP8266   Fan  1=1 MIN \2=2 \3=3 \4=4 \ 5=5 MAX \ 10 = Auto \ 11 = Quiet   

    switch (irdaikin.getFan()) {
    case 0:
      Fan = 1;
      break;
    case 1:
      Fan = 2;
      break;
    case 2:
      Fan = 3;
      break;
    case 3:
      Fan = 4;
      break;
    case 4:
      Fan = 5;
      break;
    case 5:
      Fan = 10;
      break;
    case 6:
      Fan = 11;
      break;      
    default:
      Fan = 10;
      break;
     }   
    fanstring = stringfan_(Fan);
    client.publish((topic_client + "Fan").c_str(), (String(fanstring)).c_str());

    
    Temp = irdaikin.getTemp(); //uint8_t
    client.publish((topic_client + "Setpoint").c_str(), (String(Temp)).c_str());

      if( irdaikin.getSwing() == 1) //uint8_t
      SwingVertical = true;
      else if ( irdaikin.getPower() == 0 ) //uint8_t
      SwingVertical = false;
    client.publish((topic_client + "SwingVertical").c_str(), (maponoff_(SwingVertical)).c_str());
      if( irdaikin.getSwingLR() == 1) //uint8_t
      SwingVertical = true;
      else if ( irdaikin.getSwingLR() == 0 ) //uint8_t
       SwingHorizontal = false;   
    client.publish((topic_client + "SwingHorizontal").c_str(), (maponoff_(SwingHorizontal)).c_str());
  
    Serial.println("-----------------------------------------------");
    Serial.print("Power:");
    Serial.print(power);
    Serial.print(" , ");
    Serial.print("Mode:");
    Serial.print(modestring);
    Serial.print(" , ");
    Serial.print("Fan:");
    Serial.print(fanstring);
    Serial.print(" , ");
    Serial.print("Temp:");
    Serial.print(Temp);
    Serial.print(" , ");
    Serial.print("Swing:");
    Serial.print(SwingVertical);
    Serial.print(" , ");
    Serial.print("SwingLR:");
    Serial.print(SwingHorizontal);
    Serial.println();    
    Serial.println("-----------------------------------------------");
    html_ir_receive = "Power:" + maponoff_(power) + " , Mode:" + modestring + " , Fan:" + fanstring + " , Temp:" + String(Temp) + " , Swing V:" + maponoff_(SwingVertical) + " , Swing H:" + maponoff_(SwingHorizontal);
    //html_ir_receive = "Power:" + String(power);
    //html_ir_receive = html_ir_receive + " , Mode:" + modestring + " , Fan:" + fanstring;
    //html_ir_receive = html_ir_receive + " , Temp:" + String(Temp);
    //html_ir_receive = html_ir_receive + " , Swing V:" + String(SwingVertical);
    //html_ir_receive = html_ir_receive + " , Swing H:" + String(SwingHorizontal);
    //Serial.print(html_ir_receive);
    //client.loop(); // mqtt client loop  
  } */

//FUNCAO RECEIVER LEITURA DYIRDAIKIN*************************************************


  if(startcount < 6)
    startcount++;
  if (millis() - lastMillisstart > 50000 && transmiteMQTT == false && startcount > 5) {
    transmiteMQTT = true;
   }

  // publish a message roughly every second. 1000
   if (millis() - lastMillis2 > 60000 && transmiteMQTT == true) {
    lastMillis2 = millis();
    lasttemperatura = temperatura;
    client.publish((topic_client + "online").c_str() , "ON");
    client.publish((topic_client + "SENSORTEMP").c_str(), (String(temperatura)).c_str());
    client.publish((topic_client + "SENSORHUM").c_str() , (String(humidade)).c_str());
   }

   if (millis() - lastMillis3 > 10000 && transmiteMQTT == true && iluminacao >=0 && iluminacao != lastiluminacao && (iluminacao > lastiluminacao + 10 || iluminacao < lastiluminacao - 10 )) {
      lastMillis3 = millis();
      lastiluminacao = iluminacao;
      client.publish((topic_client + "LUX").c_str() , (String(iluminacao)).c_str());     
   }
 
   if ( transmiteMQTT == true && PIR_Status != PIR_Last && millis() - lastMillisPIR > 20000)
   {
     PIR_Last = PIR_Status;
     lastMillisPIR = millis();
    // Serial.print("PIR: ");
    // Serial.println(PIR_Status);
     client.publish((topic_client + "PIR").c_str() , (maponoff_(PIR_Status)).c_str());
   }
 
   if(PIR_Status == PIR_Last && PIR_Status)
      lastMillisPIR = millis();
  
// FUNCAO RECEIVER MQTT e SEND IR ****************************************************
// Passoua para void callback(char* topic, byte* payload, unsigned int length)


 if( MQTT_R_AC_OK == true && MQTT_R_SENDIR == false)
 {
  lastMillisdelayIR = millis();
  MQTT_R_SENDIR = true;
  //Serial.println("Enviar Start");
 }
 else if ( MQTT_R_SENDIR == true && millis() - lastMillisdelayIR > 700)
 {
  //Serial.println("Enviar");
  MQTT_R_SENDIR = false;
  sendirfunction();
 }
 
 
// FUNCAO RECEIVER MQTT e SEND IR ****************************************************  

  

}
// FIM LOOP *******************************************************************



void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

    ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// http server
void handleRoot() {

// ************* PAGINA WEB


String htmlpage = "<!DOCTYPE html>"
"<head>"
"<meta http-equiv='Cache-control' content='no-cache'>"
"</head>"
"<body>"
"<div style='margin-left: 15px'>"
"<p style='text-align: left;font-size: 20px;'> ESP8266 - Samuel David OFFLINE </p>" 
"<table>"
"<tr>"
"<td style='border: 1px solid black; padding-right: 20px; padding-left: 5px;'>" 
   "<table class='table table-condensed' style='width: 100%;'>"
       "<tr>"
        "<td>SSID:</td>"
        "<td style='font-weight: bold;'>" + String(WiFi.SSID()) + "</td>"
       "</tr>"
       "<tr>"
        "<td>Hostname:</td>"
        "<td style='font-weight: bold;'>" + String(WiFi.hostname()) + "</td>"
       "</tr>"
       "<tr>"
        "<td>MQTT Client:</td>"
        "<td style='font-weight: bold;'>" + String(topic_client) + "</td>"
       "</tr>"
       "<tr>"
        "<td>Mac Adress:</td>"
        "<td style='font-weight: bold;'>" + String(WiFi.macAddress()) + "</td>"
       "</tr>"
       "<tr>"  
        "<td>IP Address:</td>"
        "<td style='font-weight: bold;'>" + String(stringip) + "</td>"
       "</tr>"
       "<tr>"
        "<td>MQTT Status:</td>"
        "<td style='font-weight: bold;'>" + String(html_mqtt_client) + "</td>"
       "</tr>"      
   "</table>"
//"<br>"
"</td>"
"<td style='padding-right: 20px;'></td>"

"<td style='border: 1px solid black; padding-right: 50px; padding-left: 5px;'>"
    "<form method='POST' action='/GET' enctype='multipart/form-data' onsubmit='return SendText()' >"
    "<b>Client</b><br>"
    "Hostname:<input type='text'  name='hostname' id='hostname' value='" + String(config.M_hostname) + "'>"
    "MQTT Client:<input type='text'  name='MQTTclient' id='MQTTclient' value='" + String(config.M_MQTT_client) + "'>"
    "<br><b>Server</b><br>"
    "SERVER_IP:"
    "<input type='text' name='Cip_01' id='Cip_01' maxlength='3' size='3' value='" + String(config.M_Cip_01) + "'>."
    "<input type='text' name='Cip_02' id='Cip_02' maxlength='3' size='3' value='" + String(config.M_Cip_02) + "'>."
    "<input type='text' name='Cip_03' id='Cip_03' maxlength='3' size='3' value='" + String(config.M_Cip_03) + "'>."
    "<input type='text' name='Cip_04' id='Cip_04' maxlength='3' size='3' value='" + String(config.M_Cip_04) + "'>"
    "<br><b>Temp:Hum Calibration (0.000)</b><br>"
    "Temp&nbsp;Cal:&nbsp;<input type='text' name='Temp_factor' id='Temp_factor' maxlength='5' size='5' value='" + String(config.M_Temp_factor) + "'>&nbsp;&nbsp;&nbsp;"
    "Humi&nbsp;Cal:&nbsp;<input type='text' name='Humi_factor' id='Humi_factor' maxlength='5' size='5' value='" + String(config.M_Hum_factor) + "'>"
    "<br><b>Lux Calibration (0.000)</b><br>"
    "Lux&nbsp;Cal:&nbsp;<input type='text' name='Lux_factor' id='Lux_factor' maxlength='5' size='5' value='" + String(config.M_Lux_factor) + "'>"
    "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button type='button' class='btn btn-default' onclick='sendData(\"reset\")'>REBOOT</button><br>"
    "Password: arduino""<br>"
    "Password:&nbsp;<input type='password' name='Pverifica' id='Pverifica' maxlength='7' size='7'>"
      "<input type='submit' value='Validar'>&nbsp;Status:&nbsp;"
    "<b id='Pmessage' ></b>"
    "<b>&nbsp; - &nbsp; </b>"
    "<b id='filestatus'><b>&nbsp;&nbsp;&nbsp;"
    "<b id='Pmessage1' ></b>" 
    "</form>" 
  
"</td>"
"</tr>"
"<tr>"
"<td style='font-weight: bold;'> Utilizar Endereco: http://"+ String(stringip) +"/expand  ou&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
"<button type='button' class='btn btn-default' onclick='window.location.href=\"/expand\"'>MODO EXPANDIDO</button>"
"</td>"
"</tr>"
"</table>"


"</div>"
"</body>"

"<script>"

"function SendText() {"   
    "var Confpassinput = document.getElementById('Pverifica').value;"
    "var Confpass = 'arduino';"
    "var n = Confpassinput.localeCompare(Confpass);"
    "if ( n == 0) {"    
                    "document.getElementById('Pmessage').innerHTML='OK';"
                     // Countdown timer
                     "var counter = 10;"
                     "var interval = setInterval(function()"
                     "{"
                        "document.getElementById('Pmessage1').innerHTML = -- counter;"
                        "if (counter == 0)"
                        "{"
                            "clearInterval(interval);"
                            "location.reload(true);"  
                        "}"
                      "}, 1000);"
                  "}"
    "else {"
           " document.getElementById('Pmessage').innerHTML='ERRO';"
           " return false;"
           "}"
  "}"

"</script>"
"</html>";

server.send(200, "text/html",htmlpage);


}
  
// http server
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

String timeconvert ( int timeinit) {

String hourstring="";
String minstring="";
if(timeinit<0 )
 timeinit=0;
if(timeinit>1440)
 timeinit=0;
int hour = (timeinit / 60);
int minutes = timeinit - hour*60;
if (hour < 10)
hourstring = "0" + String(hour);
else
hourstring = String(hour);
if (minutes < 10)
{
 if(minutes<0)
  minstring = "00";
 else    
  minstring = "0" + String(minutes);
}
else
minstring = String(minutes);

String horario = hourstring + ":" + minstring;

return horario;
}  


String onoff01 (int inputnumber){

String outputonoff01 ="";

if ( inputnumber == 0)
  outputonoff01 = "OFF";
else if ( inputnumber == 1)
  outputonoff01 = "ON";

return outputonoff01;
}


String stringmode_ (int mode_){

    switch (mode_) {
    case 0:
      modestring = "AUTO";
      break;
    case 2:
      modestring = "DRY";
      break;
    case 3:
      modestring = "COOL";
      break;
    case 4:
      modestring = "HEAT";
      break;
    case 6:
      modestring = "FAN";
      break;   
    default:
      modestring = "ERR";
      break;
  }

return modestring;

}

String stringfan_ (int fan_){

   switch (fan_) {
    case 1:
      fanstring = "LOW";
      break;
    case 2:
      fanstring = "MIDDLE";
      break;
    case 3:
      fanstring = "MIDDLE";
      break;
    case 4:
      fanstring = "MIDDLE";
      break;
    case 5:
      fanstring = "HIGH";
      break;
    case 10:
      fanstring = "AUTO";
      break;
    case 11:
      fanstring = "SILENT";
      break;      
    default:
      fanstring = "ERR";
      break;
  }   
return fanstring;
}

int cor_min (int min_ ) // Caso recebe min superior a (23:59) 1439 converte para inferior 
{
  if (min_ < 0)
    return 0;
  else if(min_ < 1440 )
    return min_;
  else if(min_ <= 2880 )
    return min_ = min_ - 1440;
  else if(min_ <= 4320 )
    return min_ = min_ - 2880;
  else if(min_ <= 5760 )
    return min_ = min_ - 4320;
}

void sendirfunction(){

     // IRRemoteESP8266 
     irrecv.disableIRIn();
     //DY Daikin
   // Set up what we want to send. See ir_Daikin.cpp for all the options.
     daikinir_send.setPower(power);
     daikinir_send.setMode(Mode);
     modestring=stringmode_(Mode);
     daikinir_send.setTemp(Temp);
     daikinir_send.setFan(Fan);
     fanstring = stringfan_(Fan);
     daikinir_send.setQuiet(Quiet);
     daikinir_send.setPowerful(Powerful);  
/*     
     daikinir_send.setSensor(Sensor);
     daikinir_send.setEye(Eye);
     daikinir_send.setMold(Mold);
     daikinir_send.setSwingVertical(SwingVertical);
     daikinir_send.setSwingHorizontal(SwingHorizontal);
     daikinir_send.setEcono(Econo);
     if ( OnTimerS == true)
     daikinir_send.enableOnTimer(OnTimerT);
     if ( OffTimerS == true) 
     daikinir_send.enableOffTimer(OffTimerT);
     daikinir_send.setCurrentTime(CurrentTime);
     if ( OnTimerS == false) 
     daikinir_send.disableOnTimer();
     if ( OffTimerS == false) 
     daikinir_send.disableOffTimer();*/
     
     // Display what we are going to send.
      Serial.print("Daikin Send IR:");
      Serial.println(daikinir_send.toString());

     // Now send the IR signal.
    //#if SEND_DAIKIN
    //  daikinir_send.send();
   // #endif  // SEND_DAIKIN 

    // Now send the IR signal.
    #if SEND_DAIKIN216
      daikinir_send.send();
    #endif  // SEND_DAIKIN 
    
    delay(100);
    MQTT_R_AC_OK = false;   
    irSendCounter = irSendCounter + 1;
    //delay(500);
    // IRRemoteESP8266 
    irrecv.enableIRIn();   
     //irdaikin.begin();  //DY Daikin
    // irdaikin.decodePin(kRecvPin); //DY Daikin
}

// Parse the URL args to find the IR code.
void handlehtmlget() {

  for (uint16_t i = 0; i < server.args(); i++) {

    if (server.argName(i) == "MQTTclient")
      server.arg(i).toCharArray(config.M_MQTT_client, 30);

    else if (server.argName(i) == "hostname")
      server.arg(i).toCharArray(config.M_hostname, 30);

    else if (server.argName(i) == "Cip_01")
      server.arg(i).toCharArray(config.M_Cip_01, 5);
      
    else if (server.argName(i) == "Cip_02")
      server.arg(i).toCharArray(config.M_Cip_02, 5);      
    
    else if (server.argName(i) == "Cip_03")
      server.arg(i).toCharArray(config.M_Cip_03, 5);      
    
    else if (server.argName(i) == "Cip_04")
      server.arg(i).toCharArray(config.M_Cip_04, 5);    

    else if (server.argName(i) == "Temp_factor")
      server.arg(i).toCharArray(config.M_Temp_factor, 10);    

    else if (server.argName(i) == "Humi_factor")
      server.arg(i).toCharArray(config.M_Hum_factor, 10);   
       
    else if (server.argName(i) == "Lux_factor")
      server.arg(i).toCharArray(config.M_Lux_factor, 10);   
     
  }

  //Serial.println(String(M_CSSID));
   file_store();

}


void handlehtmlbotoes(){
 
 String t_state = server.arg("BOTstate"); // setBOT?BOTstate='+bot Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 boolean transmite = false;
 Serial.println(t_state);
  if(t_state.equals("reset"))
  {
   handleRoot(); 
   WiFi.forceSleepBegin(); 
   wdt_reset(); 
   ESP.restart(); 
   while(1)wdt_reset();
  }
  else if(t_state.equals("resetwifi"))
  {
   handleRoot(); 
   WiFiManager wifiManager;
   wifiManager.resetSettings();
   delay(10);   
   WiFi.forceSleepBegin(); 
   wdt_reset(); 
   ESP.restart(); 
   while(1)wdt_reset();
  }
 else if(t_state.equals("bpoweron")){
  transmite = true;
  power= true; 
  publish_comando();
  client.publish((topic_client + "Power").c_str(), (maponoff_(power)).c_str());
 }
 else if(t_state.equals("bpoweroff")){
  transmite = true;
  power= false; 
  publish_comando();
  client.publish((topic_client + "Power").c_str(), (maponoff_(power)).c_str());
 }
 else if(t_state.equals("bpowerfulon")){
  transmite = true;
  Powerful= true; 
  publish_comando();
  client.publish((topic_client + "Powerful").c_str(), (maponoff_(Powerful)).c_str());
 }
 else if(t_state.equals("bpowerfuloff")){
  transmite = true;
  Powerful= false; 
  publish_comando();
  client.publish((topic_client + "Powerful").c_str(), (maponoff_(Powerful)).c_str());
 }
 else if(t_state.equals("bquieton")){
  transmite = true;
  Quiet= true; 
  publish_comando();
  client.publish((topic_client + "Quiet").c_str(), (maponoff_(Quiet)).c_str());
 }
 else if(t_state == "bquietoff"){
  transmite = true;
  Quiet= false; 
  publish_comando();
  client.publish((topic_client + "Quiet").c_str(), (maponoff_(Quiet)).c_str());
 }
 else if(t_state.equals("bsensoron")){
  transmite = true;
  Sensor= true; 
  publish_comando();
  client.publish((topic_client + "Sensor").c_str(), (maponoff_(Sensor)).c_str());
 }
 else if(t_state.equals("bsensoroff")){
  transmite = true;
  Sensor= false; 
  publish_comando();
  client.publish((topic_client + "Sensor").c_str(), (maponoff_(Sensor)).c_str());
 }
 else if(t_state.equals("beyeon")){
  transmite = true;
  Eye= true; 
  publish_comando();
  client.publish((topic_client + "Eye").c_str(), (maponoff_(Eye)).c_str());
 }
 else if(t_state.equals("beyeoff")){
  transmite = true;
  Eye= false; 
  publish_comando();
  client.publish((topic_client + "Eye").c_str(), (maponoff_(Eye)).c_str());
 }
 else if(t_state.equals("bmoldon")){
  transmite = true;
  Mold= true; 
  publish_comando();
  client.publish((topic_client + "Mold").c_str(), (maponoff_(Mold)).c_str());
 }
 else if(t_state.equals("bmoldoff")){
  transmite = true;
  Mold= false; 
  publish_comando();
  client.publish((topic_client + "Mold").c_str(), (maponoff_(Mold)).c_str());
 }
   else if(t_state.equals("bswingverticalon")){
  transmite = true;
  SwingVertical= true; 
  publish_comando();
  client.publish((topic_client + "SwingVertical").c_str(), (maponoff_(SwingVertical)).c_str());
 }
 else if(t_state.equals("bswingverticaloff")){
  transmite = true;
  SwingVertical= false; 
  publish_comando();
  client.publish((topic_client + "SwingVertical").c_str(), (maponoff_(SwingVertical)).c_str());
 } 
   else if(t_state.equals("bswinghorizontalon")){
  transmite = true;
  SwingHorizontal= true; 
  publish_comando();
  client.publish((topic_client + "SwingHorizontal").c_str(), (maponoff_(SwingHorizontal)).c_str());
 }
 else if(t_state.equals("bswinghorizontaloff")){
  transmite = true;
  SwingHorizontal= false; 
  publish_comando();
  client.publish((topic_client + "SwingHorizontal").c_str(), (maponoff_(SwingHorizontal)).c_str());
 } 


 
   
 else if(t_state.equals("bontimerson")){
  transmite = true;
  OnTimerS= true; 
  publish_comando();
  client.publish((topic_client + "OnTimerS").c_str(), (maponoff_(OnTimerS)).c_str());
 }   
 else if(t_state.equals("bontimersoff")){
  transmite = true;
  OnTimerS= false; 
  publish_comando();
  client.publish((topic_client + "OnTimerS").c_str(), (maponoff_(OnTimerS)).c_str());
 }   
      
 else if(t_state.equals("bofftimerson")){
  transmite = true;
  OffTimerS= true; 
  publish_comando();
  client.publish((topic_client + "OffTimerS").c_str(), (maponoff_(OffTimerS)).c_str());
 }   
 else if(t_state.equals("bofftimersoff")){
  transmite = true;
  OffTimerS= false; 
  publish_comando();
  client.publish((topic_client + "OffTimerS").c_str(), (maponoff_(OffTimerS)).c_str());
 }

  else if(t_state.equals("beconoon")){
  transmite = true;
  Econo= true; 
  
 }   
 else if(t_state.equals("beconooff")){
  transmite = true;
  Econo= false; 
  
 }
  
 else if(t_state.equals("bmode")){
  transmite = true;
  if (Mode == 0)
   Mode = 2;
  else if( Mode == 2)
   Mode = 3;
  else if (Mode == 3)
   Mode = 4;
  else if (Mode == 4)
   Mode = 6;
  else if (Mode == 6)
   Mode = 0;
  modestring=stringmode_(Mode);
 publish_comando(); 
 client.publish((topic_client + "Mode").c_str(), (String(modestring)).c_str()); 
 }   

 else if(t_state.equals("bfan")){
  transmite = true;
  if (Fan == 1)
   Fan = 2;
  else if( Fan == 2 || Fan == 3 || Fan == 4)
   Fan = 5;
  else if (Fan == 5)
   Fan = 10;
  else if (Fan == 10)
   Fan = 11;
  else if (Fan == 11)  
   Fan = 1;
  fanstring = stringfan_(Fan);
  if(Fan == 11)
    Quiet = true;
  else
    Quiet = false;
  publish_comando();
  client.publish((topic_client + "Fan").c_str(), (String(fanstring)).c_str());
 }   
 
 else if(t_state.equals("bsetempp")){
  transmite = true;
  if (Temp < 30)
   Temp = Temp +1;
  publish_comando();
  client.publish((topic_client + "Setpoint").c_str(), (String(Temp)).c_str());
 }
 else if(t_state.equals("bsetempm")){
  transmite = true;
  if (Temp > 18)
   Temp = Temp -1;
  publish_comando();
  client.publish((topic_client + "Setpoint").c_str(), (String(Temp)).c_str());
 }    
    

 if(transmite)
 {
  sendirfunction();
  transmite = false;
 }
 server.send(200, "text/plane", ""); //Send web page 
}


void handleXML() {

  modestring = stringmode_(Mode);
  fanstring = stringfan_(Fan);
  String html_wifi_signal="null";
  String html_rssi_sign="null";
  String html_wifi_status="null";
  String html_irsend ="null";
  String html_MQTT_uptime = timeSince(lastDisconnectedTimeMQTT);
  html_MQTT_uptime = html_MQTT_uptime + "(" + String(DisconnectedMQTTcount);
  html_MQTT_uptime = html_MQTT_uptime + ")";
  
  long rssi = WiFi.RSSI();

  if(client.connected())
 html_mqtt_client = "ONLINE";
 else
 html_mqtt_client = "ERRO"; 


    switch (WiFi.status()) {
    case 0:
      html_wifi_status = "IDLE_STATUS";
      break;
    case 1:
      html_wifi_status = "NO_SSID_AVAIL";
      break;
    case 2:
      html_wifi_status = "SCAN_COMPLETED";
      break;
    case 3:
      html_wifi_status = "CONNECTED";
      break;
    case 4:
      html_wifi_status = "CONNECT_FAILED";
      break;
    case 5:
      html_wifi_status = "CONNECTION_LOST";
      break;
    case 6:
      html_wifi_status = "DISCONNECTED";
      break;         
    default:
      html_wifi_status = "ERROR";
      break;
  }

html_irsend = daikinir_send.toString();
if(html_irsend.length() <1)
html_irsend ="null";

if(html_ir_receive.length() <1)
html_ir_receive ="null";


// Serial.println(xmlvar);
 

String xmlpage =   "<?xml version='1.0'?>"
  "<adress>"

  //Cria as label XML atraves das TAG's criadas, podem possuir qualquer nome
  "<progrevision>" + String(REVISIONPROG) + "</progrevision>"
  "<wifissid>" + String(WiFi.SSID()) + "</wifissid>"
  "<wifihostname>"+ String(WiFi.hostname()) + "</wifihostname>"
  "<topicclient>" + String(topic_client) + "</topicclient>"
  "<wifimac>" + String(WiFi.macAddress()) + "</wifimac>"
  "<stringip>" + String(stringip) + "</stringip>"
  "<clientcon>" + String(html_mqtt_client) + "</clientcon>"
  "<hostname>" + String(config.M_hostname) + "</hostname>"
  "<MQTTclient>" + String(config.M_MQTT_client) + "</MQTTclient>"
  "<Cip_01>" + String(config.M_Cip_01) + "</Cip_01>"
  "<Cip_02>" + String(config.M_Cip_02) + "</Cip_02>"
  "<Cip_03>" + String(config.M_Cip_03) + "</Cip_03>"
  "<Cip_04>" + String(config.M_Cip_04) + "</Cip_04>"
  "<Temp_factor>" + String(config.M_Temp_factor) + "</Temp_factor>"
  "<Humi_factor>" + String(config.M_Hum_factor) + "</Humi_factor>"
  "<Lux_factor>" + String(config.M_Lux_factor) + "</Lux_factor>"
  
  "<wifirssi>" + String(WiFi.RSSI()) + "</wifirssi>"
  "<wifisatus>" + String(html_wifi_status) + "</wifisatus>"
   "<uptime>" + String(timeSince(lastMillisstart)) + "</uptime>"
  "<uptimeMQTT>" + String(html_MQTT_uptime) + "</uptimeMQTT>"
 

  "<temperatura>" + String(temperatura, 2) + "</temperatura>"
  "<humidade>" + String(humidade, 2) + "</humidade>"

  "<temp>" + String(Temp) + "</temp>"
  "<power>" + String(onoff01(power)) + "</power>"
  "<mode>" + String(Mode) + ":" + String(modestring) + "</mode>"
  "<fan>" + String(Fan) + ":" + String(fanstring) + "</fan>"
  "<powerful>" + String(onoff01(Powerful)) + "</powerful>"
  "<quiet>" + String(onoff01(Quiet)) + "</quiet>"
  "<sensor>" + String(onoff01(Sensor)) + "</sensor>"
  "<eye>" + String(onoff01(Eye)) + "</eye>"
  "<mold>" + String(onoff01(Mold)) + "</mold>"
  "<econo>" + String(onoff01(Econo)) + "</econo>"
  "<swingvertical>" + String(onoff01(SwingVertical)) + "</swingvertical>"
  "<swinghorizontal>" + String(onoff01(SwingHorizontal)) + "</swinghorizontal>"
  "<ontimers>" + String(onoff01(OnTimerS)) + "</ontimers>"
  "<ontimert>" + String(timeconvert(OnTimerT)) + "</ontimert>"
  "<offtimers>" + String(onoff01(OffTimerS)) + "</offtimers>"
  "<offtimert>" + String(timeconvert(OffTimerT)) + "</offtimert>"  
  "<currentime>" + String(timeconvert(CurrentTime)) + "</currentime>" 
  "<filestatus>" + String(html_file_status) + "</filestatus>"
  "<ilumina>" + String(iluminacao) + "</ilumina>"
  "<ilumina1>" + String(iluminacao) + "</ilumina1>"
  "<htmlirreceive>" + String(html_ir_receive) + "</htmlirreceive>"
  "<htmlirsend>" + String(html_irsend) + "</htmlirsend>"
  "<pirstatus>" + String(maponoff_(PIR_Status)) + "</pirstatus>"
  "<irrecvcounter>" + String(irRecvCounter) + "</irrecvcounter>"
  "<irsendcounter>" + String(irSendCounter) + "</irsendcounter>"
  "</adress>";
   server.send(200,"text/xml",xmlpage );
}



//xxxxxxx FILE SYSTEM SAVE xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void file_store() {

            if(SPIFFS.remove("/conf.txt"))
            {
              delay(100);
              File f = SPIFFS.open("/conf.txt", "w");
              if (!f) {
                        Serial.println("ERROR: File creation failed");
                        html_file_status ="ERROR: File creation failed";
                      }
              else{
                    // now write two lines in key/value style with  end-of-line characters
                    Serial.println("File creation");

                    //DynamicJsonBuffer jsonBuffer;
                    //JsonObject& json = jsonBuffer.createObject();
                    const size_t capacity = JSON_OBJECT_SIZE(12) + 200;                  
                    DynamicJsonDocument json(capacity);

                    // Set the values 
                    json["cip01"] = config.M_Cip_01;
                    json["cip02"] = config.M_Cip_02;
                    json["cip03"] = config.M_Cip_03;
                    json["cip04"] = config.M_Cip_04;
                    json["mqttclient"] = config.M_MQTT_client;
                    json["hostname"] = config.M_hostname;
                    json["tempfactor"] = config.M_Temp_factor;
                    json["humfactor"] = config.M_Hum_factor;
                    json["luxfactor"] = config.M_Lux_factor;
    
                    // Serialize JSON to file
                    if (serializeJson(json, f) == 0) {
                      html_file_status ="ERROR: Failed to write to file";
                      Serial.println("ERROR: Failed to write to file");
                    }
                    else {
                      serializeJson(json, Serial);
                      //json.printTo(Serial);
                      //json.printTo(f);
                      
                   Serial.println();
                   Serial.println("File created and saved");
                   html_file_status ="File created and saved";
                  }
            }
          
            f.close();

         }
         else
         {
          html_file_status ="ERROR: File erase";
         Serial.println("ERROR: File erase");
         File f = SPIFFS.open("/conf.txt", "w");
         f.close();
         }

   delay(100);
   handleRoot();
   WiFi.forceSleepBegin(); wdt_reset(); ESP.restart(); while(1)wdt_reset();

}

void expandhtml()
{

server.send(200, "text/html",PAGE_NetworkConfiguration);


}

//funcao convert 0 OFF \ 1 ON

String maponoff_ (int input)
{
if (input == 1)
return ("ON");
else
return ("OFF");
}


String timeSince(uint32_t const start) {
  if (start == 0)
    return "Never";
  uint32_t diff = 0;
  uint32_t now = millis();
  if (start < now)
    diff = now - start;
  else
    diff = UINT32_MAX - start + now;
  diff /= 1000;  // Convert to seconds.
  if (diff == 0)  return "Now";

  // Note: millis() can only count up to 45 days, so uint8_t is safe.
  uint8_t days = diff / (60 * 60 * 24);
  uint8_t hours = (diff / (60 * 60)) % 24;
  uint8_t minutes = (diff / 60) % 60;
  uint8_t seconds = diff % 60;

  String result = "";
  if (days)
    result += String(days) + " day";
  if (days > 1)  result += "s";
  if (hours)
    result += " " + String(hours) + " h:";
  if (minutes)
    result += " " + String(minutes) + " m:";
  if (seconds)
    result += " " + String(seconds) + " s";
  result.trim();
  return result + " ago ";
}

void publish_comando (){

if (millis() - lastMilliscomando > 1000) {
  lastMilliscomando = millis();
  client.publish((topic_client + "Comando").c_str(), "ON");
}
  
}


 
