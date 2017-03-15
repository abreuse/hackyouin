#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include "Gsender.h"
#include "FS.h"

#define RST_PIN         2           // Configurable, see typical pin layout above
#define SS_PIN          15          // Configurable, see typical pin layout above

#pragma region Globals
const char* ssid = "LAnnexeSamasoulé";                           // WIFI network name
const char* password = "bsapthek";                       // WIFI network password
uint8_t connection_state = 0;                    // Connected to WIFI or not
uint16_t reconnect_interval = 10000;             // If not connected wait time to try again
#pragma endregion Globals

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
char choice;
void setup() {
  Serial.begin(115200);         // Initialize serial communications with the PC

  while (!Serial);            // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();         // Init MFRC522 card
  Serial.println(F("Try the most used default keys to print block 0 to 63 of a MIFARE PICC."));
  Serial.println("1.Read card \n2.Write to card \n3.Copy the data.");
}
String dump_byte_array(byte *buffer, byte bufferSize) {
  String s = "";
  for (byte i = 0; i < bufferSize; i++) {
    s = s + (buffer[i] < 0x10 ? " 0" : " ");
    s = s + (String(buffer[i], HEX));
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  return s;
}

  uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
  {
      static uint16_t attempt = 0;
      Serial.print("Connecting to ");
      if(nSSID) {
      WiFi.begin(nSSID, nPassword);
      Serial.println(nSSID);
    } else {
      WiFi.begin(ssid, password);
      Serial.println(ssid);
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
  start();

}

void start() {
  choice = Serial.read();

  if (choice == '1')
  {
    Serial.println("Read the card");
    keuze1();

  }
}

void keuze1() { //Read card
  Serial.println("Insert card...");
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  // Show some details of the PICC (that is: the tag/card)
  String s = "";
  s = s + F("Card UID:");
  s = s + dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  s = s + "\n" + F("PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  s = s + mfrc522.PICC_GetTypeName(piccType);

  connection_state = WiFiConnect();
  if (!connection_state) // if not connected to WIFI
    Awaits();          // constantly trying to connect

  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
  String subject = "Subject is optional!";
  if (gsender->Subject(subject)->Send("breusemanouanaesgi@gmail.com", s)) {
    Serial.println("Message sent.");
  } else {
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