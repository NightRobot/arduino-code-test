#include <ESP8266WiFi.h>
#include "MFRC522.h"
#define RST_PIN 15 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15 
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
const char* host    = "api.thingspeak.com";
const char* api_key = "UO0U63SZI95U5351";
String uid;
void setup() {
    Serial.begin(115200);
    delay(10);
    connectWifi("nightRobot","123456789");
    SPI.begin();           // Init SPI bus
    mfrc522.PCD_Init();    // Init MFRC522 
}
void loop() {
  // Look for new cards
  if ( mfrc522.PICC_IsNewCardPresent()) {
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      delay(50);
      return;
    }
    // Show some details of the PICC (that is: the tag/card)
//      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
//      Serial.print(uid);
//      Serial.println();
      send_data(uid);
      delay(2000);
    return;
    }
  }
  
void send_data(String data){
  Serial.print("send Card UID : ");
  Serial.println(data);
  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // api reqest GET https://api.thingspeak.com/update?api_key=UO0U63SZI95U5351&field1=0
  // We now create a URI for the request
  String url = "/update?api_key=";
  url += api_key;
  url += "&field1=";
  url += data;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection");
  }
void connectWifi(const char* ssid,const char* password){
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
   
    while (WiFi.status() != WL_CONNECTED)   //รอจนกว่าจะเชื่อมต่อสำเร็จ
    {
            delay(500);
            Serial.print(".");
    }
    Serial.println(""); 
    Serial.println("WiFi connected");   //แสดงข้อความเชื่อมต่อสำเร็จ  
    Serial.println("IP address: ");   
    Serial.println(WiFi.localIP());   //แสดงหมายเลข IP ของ ESP8266(DHCP)
  }
void read_rfid(){
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  }
// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  String code = "";
  for (byte i = 0; i < bufferSize; i++) {
    code += String(buffer[i]);
  }
  uid = code;
}
