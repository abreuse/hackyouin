/****Includes****/
#include <ESP8266WiFi.h>          
#include <WiFiClient.h>
#include <DNSServer.h>            
#include <ESP8266WebServer.h>     
#include <WiFiManager.h>          
#include <ESP8266mDNS.h>          

/*****Initialization*****/
ESP8266WebServer server(80);
const char *ssid = "Configuration";
char  serviceSetIdentifier[100];
char name[100];
char surname[100];
char email[100];
char password[100];

/*****WebPage*****/
// Warning: only use simple quotes in the html (no double)
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
/****Setups****/

void setupWifi() {
    WiFiManager wifiManager;

    wifiManager.resetSettings();
    wifiManager.autoConnect(ssid);

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
    if (MDNS.begin(ssid)) {
        Serial.print("MDNS responder started as http://");
        Serial.print(ssid);
        Serial.println(".local");
    }
    MDNS.addService("http", "tcp", 8080);
}

void setup() {

    Serial.begin(115200);

     Serial.println("Starting WiFi.");
    setupWifi();
    setupServer();
    setupMDNS();

    Serial.println("Setup OK.");
}

/****Loop****/
void loop() {
    server.handleClient();
}

