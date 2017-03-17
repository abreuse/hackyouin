#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>          
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include "Gsender.h"
#include "FS.h"
#include <vector>

#define RST_PIN         2
#define SS_PIN          15

#pragma region Globals
ESP8266WebServer server(80);
const char *AP_ssid = "Module_RFID";
const char *AP_pwd = "Module_RFID";
char serviceSetIdentifier[100];
char name[100];
char surname[100];
char email[100];
char password[100];

unsigned long timer;
unsigned long prevTimer;
const char* ssid_to_connect = "HotspotRFID";
const char* password_to_connect = "hotspotRFID";
uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;
//String mailAdr = "breusemanouanaesgi@gmail.com";
String pathFile = "/rfids.txt";
struct rfid
{
  String UID;
  String type;
};
std::vector<rfid> rfids;
#pragma endregion Globals

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

//WEB PAGE
String rootHTML = "\
<!doctype html> <html> <head> <title> Configuation Form </title> </head> <body>\
<h1> Hack You In </h1>\
<form method='get' action='/register'>\
  <br>Email : <br> <input type='text' name='email' />\
  <br> Name : <br> <input type='text' name='name' />\
  <br> Surname : <br> <input type='text' name='surname' />\
  <br> Ssid : <br> <input type='text' name='ssid' />\
  <br> Password : <br> <input type='password' name='password' />\
  <br><br> <input type='submit' name='register' />\
</form>\
</body> </html>\
";

void handleRoot() {
    server.send(200, "text/html", rootHTML);
}

void handleForm(){
  server.send(200, "<h1> Register </h1>");
  server.arg(0).toCharArray(email, 100);
  server.arg(1).toCharArray(name, 100);
  server.arg(2).toCharArray(surname, 100);
  server.arg(3).toCharArray(serviceSetIdentifier, 100);
  server.arg(4).toCharArray(password, 100);
}

void setupWifi() {
    WiFiManager wifiManager;

    wifiManager.resetSettings();
    wifiManager.autoConnect(AP_ssid, AP_pwd);

    Serial.println("local ip");
    Serial.println(WiFi.localIP());
}

void setupServer() {
    server.on("/", handleRoot);
    server.on("/register", handleForm);
    server.begin();
    Serial.println("HTTP server started");
}

void setupMDNS() {
    if (MDNS.begin(AP_ssid)) {
        Serial.print("MDNS responder started as http://");
        Serial.print(AP_ssid);
        Serial.println(".local");
    }
    MDNS.addService("http", "tcp", 8080);
}

void setup() {
  Serial.begin(115200);
  email[0] = 0;
  Serial.println("Starting WiFi.");     
  setupWifi();
  setupServer();
  setupMDNS();
   
  SPIFFS.begin();
  //SPIFFS.format();
  while (!Serial);
  SPI.begin();                
  mfrc522.PCD_Init();
  prevTimer = millis();

  File file = SPIFFS.open(pathFile, "w");
  if (!file) {
     Serial.println("file open failed");
  }
}

String convertUidToString(byte *buffer, byte bufferSize) {
  String UID = "";
  for (byte i = 0; i < bufferSize; i++) {
    UID = UID + (buffer[i] < 0x10 ? " 0" : " ");
    UID = UID + (String(buffer[i], HEX));
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  return UID;
}

void appendRFIDToFile(rfid scannedRfid, String pathFile)
{
    File file = SPIFFS.open(pathFile, "a");
    if (!file) {
       Serial.println("file open failed");
    }
    Serial.println("====== Writing" + scannedRfid.UID + scannedRfid.type + "=========");
    file.println(scannedRfid.UID);
    file.println(scannedRfid.type);
    file.close();
}

String readUidsFromFile(String pathFile)
{
  String uids = " ";
  
  File file = SPIFFS.open(pathFile, "r");
    if (!file) {
       Serial.println("file open failed");
    }
    
    while (file.available()) {
      uids = uids + file.readStringUntil('\n');
  }
  Serial.println("read = " + uids);
  file.close();
  return uids;
}

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
    WiFi.begin(nSSID, nPassword);
    Serial.println(nSSID);
  } else {
    //WiFi.begin(ssid_to_connect, password_to_connect);
    WiFi.begin(serviceSetIdentifier, password);
    Serial.println(serviceSetIdentifier);
  }
  
    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
    delay(200);
    Serial.print(".");
  }
    ++attempt;
    Serial.println("");
    if(i == 51) {
    //timeout
    Serial.print("Connection: TIMEOUT on attempt: ");
    Serial.println(attempt);
    if(attempt % 2 == 0)
    Serial.println("Check if access point available or SSID and Password\r\n");
      return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}


void loop() {
  server.handleClient();
  if(email[0] == 0)
  {
    Serial.println("email == 0");
    return;
  }
    

  Serial.println("going on for scan");
  timer = millis();
  if(timer > (prevTimer + 30000))
  {
    prevTimer = millis();
    sendMail();
  }
  scanRFID();
}


void scanRFID() {
  //Serial.println("Waiting for card to scan...");

  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  rfid scannedRfid;
  scannedRfid.UID = "";
  scannedRfid.type = "";
  scannedRfid.UID = scannedRfid.UID + F("Card UID:");
  scannedRfid.UID = scannedRfid.UID + convertUidToString(mfrc522.uid.uidByte, mfrc522.uid.size);
  scannedRfid.type = scannedRfid.type + F("PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  scannedRfid.type = scannedRfid.type + mfrc522.PICC_GetTypeName(piccType);

  if(RfidAlreadyScanned(scannedRfid))
  {
    return;
  }
  else
  {
    appendRFIDToFile(scannedRfid, pathFile);
    //sendMail();
  }
  
}


bool RfidAlreadyScanned(rfid scannedRfid)
{
  for(int i = 0; i < rfids.size(); i++)
  {
    Serial.println("UID in array : " + rfids[i].UID);
    if(scannedRfid.UID == rfids[i].UID)
    {
      Serial.println("RFIDS already scanned");
      return true;
    }
  }
  rfids.push_back(scannedRfid);
  return false;
}


void sendMail()
{
  connection_state = WiFiConnect();
  if (!connection_state)
    Awaits();
    
  String RFIDS = readUidsFromFile(pathFile);
  
  Gsender *gsender = Gsender::Instance();
  String subject = "Your RFIDS";
  
  if (gsender->Subject(subject)->Send(email, RFIDS)) 
  {
    Serial.println("Mail sent.");
  }
  else 
  {
    Serial.print("Error sending message: ");
    Serial.println(gsender->getError());
  }
}


void Awaits()
{
  uint32_t ts = millis();
  while(!connection_state)
  {
    delay(50);
    if(millis() > (ts + reconnect_interval) && !connection_state)
    {
      connection_state = WiFiConnect();
      ts = millis();
    }
  }
}