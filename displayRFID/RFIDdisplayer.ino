#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         2 
#define SS_PIN          15

MFRC522 mfrc522(SS_PIN, RST_PIN);  

void setup() {
	Serial.begin(9600);		
	while (!Serial);	
	SPI.begin();			
	mfrc522.PCD_Init();		
	mfrc522.PCD_DumpVersionToSerial();	
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}