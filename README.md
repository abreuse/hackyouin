# hackyouin

## Hardware requirements

- 1 - ESP8266
- 1 - RFID-RC522
- 1 - Battery Holder 4xAA

## Installation

Follow thoses steps :

- download arduino here: https://www.arduino.cc/en/main/software
- If you use windows/OSx you will probably need drivers: https://www.wemos.cc/downloads
- Start arduino and open the Preferences window
  - In the Additional Board Manager URLs field, enter: http://arduino.esp8266.com/versions/2.3.0/package_esp8266com_index.json
  - Open Tools -> Board -> Boards Manager
  - Type esp and the esp8266 platform will appear, install it
- Install WifiManager :
  - go to Sketch/Include Library/Manage Libraries 
  - Search for 'WifiManager'
  - Install it
- Same way, install 'MFRC522' library.


## Using

- Load program to ESP module.
- Connect to 'Module_RFID' wifi access point.
- Choose your network from the list and set password.
- Connect to your wifi network and acces to the module IP.
- Fill the form and send it.
- You can now scan some RFIDs just by putting it in front of the RFID module.

> Note : Every ten minutes, the module will try to connect to the specified ssid and password.
Then it will send scanned RFIDS to the specified mail.


## Schema

![SCHEMA](https://image.noelshack.com/fichiers/2017/11/1489757082-schema.png)

## Logs

- 15/03/2017 - Add sendMail
- 15/03/2017 - Add readWrite
- 15/03/2017 - Add displayRFID
- 16/03/2017 - Add sendRfidUidViaMail
- 16/03/2017 - Add scanAndWriteRFIDsToFile
- 16/03/2017 - Add scanAndSendMailEveryTenMin
- 16/03/2017 - Add WebPage
- 16/03/2017 - Add useButtonToSendMail (in beta)
