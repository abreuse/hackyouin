#include "FS.h"

void writeToArduinoFile(String txtFile)
{
    File file = SPIFFS.open(txtFile, "w");
    if (!file) {
       Serial.println("file open failed");
    }
    Serial.println("====== Writing to SPIFFS file =========");
    file.println("123");
    file.println("456");
    file.println("789");
    file.close();
}

void readDataFromArduinoFile(String txtFile)
{
  String s = " ";
  char c = ' ';
  
  File file = SPIFFS.open(txtFile, "r");
    if (!file) {
       Serial.println("file open failed");
    }
    
    while (file.available()) {
      c = file.read();
      s = s + String(c);
  }
  Serial.println("read = " + s);
  file.close();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println("Please wait 30 secs for SPIFFS to be formatted");
  SPIFFS.format();
  Serial.println("Spiffs formatted");

  writeToArduinoFile("/rfids.txt");
  readDataFromArduinoFile("/rfids.txt");
}

void loop() {
  // put your main code here, to run repeatedly:

}