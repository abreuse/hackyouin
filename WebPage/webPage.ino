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
char publicKey[200];
char privateKey[200];
char deleteKey[200];



/*****WebPage*****/
// Warning: only use simple quotes in the html (no double)
String registerPage = "\
<!doctype html> <html> <head> <title> Configuation Form </title> </head> <body>\
<h1> Hack You In </h1>\
<form method='post' action='/register'>\
  <br>Email : <br> <input type='text' name='email' />\
  <br> Name : <br> <input type='text' name='name' />\
  <br> Surname : <br> <input type='text' name='surname' />\
  <br> Ssid : <br> <input type='text' name='ssid' />\
  <br> Password : <br> <input type='password' name='password' />\
  <br><br> <button type='submit' name='register' > Regsiter </button>\
</form>\
</body> </html>\
";
String sparkFunPage = "\
<!doctype html> <html> <head> <title> sparkFun Form </title> </head> <body>\
<h1> Hack You In </h1>\
<div> Congrats! You have now setup your device. To see the data collected, you have to do one more thing. <br> Go to \
<a> href = https:/\/data.sparkfun.com/streams/make </a> <br> Don't freak out I'm here to help. You will need to give a title and a description to your data. \
The next field should be set to 'visible'. <br>\
After that you'll have to add 'date, uid, type' on the Fields field. you can give an Alias to your stream. <br>\
You can now hit  'Save' and complete the form below</div>\
<form method='post' action'/handleKeys'>\
  <br> Public Key : <input type 'text' name='plubliKey' />\
  <br> Private Key : <input type='text' name='privateKey' />\
  <br> Delete Key : <input type='text' name='deleteKey' />\
  <br><br> <button type='submit' name='send' > Send </button>\
</form>\
</body> </html>\
";


 
void handleRoot() {
    server.send(200, "text/html", registerPage);
}

void handleForm(){
  server.send(200, "text/html", sparkFunPage);

  server.argName(0).toCharArray(email, 100);
  server.argName(1).toCharArray(name, 100);
  server.argName(2).toCharArray(surname, 100);
  server.argName(3).toCharArray(serviceSetIdentifier, 100);
  server.argName(4).toCharArray(password, 100);
  
}

void handleKeys(){
  server.send(200, "<h1> Sending </h1>");

  server.argName(0).toCharArray(publicKey, 100);
  server.argName(1).toCharArray(privateKey, 100);
  server.argName(2).toCharArray(deleteKey, 100);  
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
    server.on("/handleKeys", handleKeys);
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

