#include <SPI.h>
#include <Wire.h>
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <Adafruit_Sensor.h>
#include <DHT.h>
//#include "HTTPClient.h"
#include "ESPAsyncWebServer.h"
#include <ESP8266HTTPClient.h>
#include "./constantes.h"
#include "./pages.h"

// nodeMCU 1.0
#define DHT1PIN D6     // pate vers le data du DHT
#define DHT2PIN D7     // pate vers le data du DHT
#define ALIMDHTPIN D6     // alimentation du DHT2
#define DHTTYPE DHT22 // modèle DHT11 ou DHT22
#define ADR_I2C_SSD1306 0x3C // adresse I2C de l'écran OLED
#define position "Chambre" // titre de la page web

//const char* nom_capteur [2] = {"portail", "garage"}; //{"portail", "garage"}; //{"chambre", "radiateur"}; //
const char* lieu = "Chambre";
const char* capteur1 = "capteur1";
const char* capteur2 = "capteur2";
const char* rev = "r9";

DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);
AsyncWebServer server(80);
SSD1306Wire display(0x3c, D3, D5);

HTTPClient http;

// card : http://arduino.esp8266.com/stable/package_esp8266com_index.json
// install manuelle des libs :
// - https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip
// - https://github.com/me-no-dev/AsyncTCP/archive/master.zip
// - https://github.com/me-no-dev/ESPAsyncTCP/archive/master.zip
// dans C:\Users\loicp\Documents\Arduino\hardware\espressif\esp32\libraries
float temp1;
float temp2;
float hum1;
float hum2;
String ip;

void init_wifi() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  ip= WiFi.localIP().toString();
  Serial.println(ip);
}

void init_serial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
}

void init_dht() {
  dht1.begin();
  dht2.begin();
}

void init_display() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  hello();
}

String processor(const String& var){
  //Serial.println(var);
  if(var == "TITRE"){
    return String(lieu);
  }
  else if(var == "TEMP1"){
    return String(temp1);
  }
  else if(var == "TEMP2"){
    return String(temp2);
  }
  else if(var == "HUM1"){
    return String(hum1);
  }
  else if(var == "HUM2"){
    return String(hum2);
  }
  else if(var == "CAPTEUR1"){
    return String(capteur1);
  }
  else if(var == "CAPTEUR2"){
    return String(capteur2);
  }
  else if(var == "P"){
    return String("%");
  }
  return String();
}

void init_serveur() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Alu");
  });
  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", page_temp, processor);
  });
  server.on("/temp1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(temp1).c_str());
  });
  server.on("/hum1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(hum1).c_str());
  });
  server.on("/temp2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(temp2).c_str());
  }); 
  server.on("/hum2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(hum2).c_str());
  });
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });
  server.begin();
}

void setup() {
  init_serial();
  init_display();
  init_dht();
  init_wifi();
  init_serveur();
}

String insert_message(float temp1, float temp2, float hum1, float hum2) {
  String mess;
  mess = "{\"TableName\": \"vrac\", \"Item\": {\"id\": {\"S\":\""+String(millis()/1000)+"\"}, \"lieu\": {\"S\":\""+String(lieu)+"\"}, \"temp1\": {\"S\":\""+String(temp1)+"\"}}}";
  //mess = "{\"TableName\": \"vrac\", \"Item\": {\"id\": {\"S\":\"X"+String(lieu)+"\"}, \"temp1\": {\"S\":\""+String(temp1)+"\"}, \"hum1\": {\"S\":\""+String(hum1)+"\"}}}";
  Serial.println(mess); //debug
  return mess;
}

void log_service() {
  http.begin(logtofileUrl, fingerprint_aws);
  // fingerprint_aws obtenu avec https://www.grc.com/fingerprints.htm
  //http.begin("https://httpbin.org/ip", fingerprint_bin);
  http.addHeader("x-api-key", cleapi);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(insert_message(temp1, temp2, hum1, hum2));
  if(httpResponseCode>0){
      String response = http.getString();  //Get the response to the request
      Serial.println(httpResponseCode);   //Print return code
      Serial.println(response);           //Print request answer
  }else{
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
  }
}

void hello() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Initialisation");
  display.drawString(0, 50, rev);
  display.display();
}

void maj_temp() {
  temp1 = dht1.readTemperature();
  temp2 = dht2.readTemperature();
  hum1= dht1.readHumidity();
  hum2= dht2.readHumidity();
  if (isnan(temp1)) {
    temp1= 0.0;
  }
  if (isnan(temp2)) {
    temp2= 0.0;
  }
  if (isnan(hum1)) {
    hum1= 0.0;
  }
  if (isnan(hum2)) {
    hum2= 0.0;
  }  
}

void aff_temp() {
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "cap1");
  display.drawString(90, 0, String(temp1));
  display.drawString(90, 11, String(temp2));
  Serial.println(String(temp1) + " " + String(temp2));
}

void aff_sys() {
  display.drawString(0, 54, String(int(millis()/1000)));
  display.drawString(45, 54, ip);
  display.drawString(115, 54, rev);
}

void maj_display() {
  display.clear();  
  aff_temp();
  aff_sys();
  display.display();  
}

void loop() {
  // put your main code here, to run repeatedly:
  maj_temp();
  maj_display();
  log_service();
  delay(600000);
}
