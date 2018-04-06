#include "ESP8266WiFi.h"
#include "MFRC522.h"
#define RST_PIN 15 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15 
#define SS_PIN  2  // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
MFRC522 rfid(SS_PIN, RST_PIN);   // Create MFRC522 instance
MFRC522::MIFARE_Key key;

//const char* host    = "data.sparkfun.com";
const char* host    = "rfid-iot.appspot.com";
const char* private_key = "wYRRDRxZk1FERAqY8Y4G";
const char* public_key = "w5zzXz1wR9CNlvpA7A8q";
const char* del_key = "MVMMKMkE5NHyKjgEaEMe";
byte nuidPICC[4];
String uid = "";
void setup() {
  pinMode(D0, OUTPUT); //set beep pin out
  Serial.begin(115200);
  delay(10);

  //connect wifi function
  connectWifi("xp_atz", "11111111");

  //setup rfid
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  //reset key
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  //check duplicate card
  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));
    Serial.print("current uid = ");

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
      //      Serial.print(nuidPICC[i]);
      //      Serial.print(String(nuidPICC[i], HEX));
      uid += String(nuidPICC[i], HEX);
      //      Serial.print(" ");

    }
    Serial.println(uid);
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    beep();
    send_data(uid);
  } else {
    //is dupplicate card
    Serial.println(F("Card read previously."));
    beep();
    beep();
    beep();
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
  Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void beep() {
  digitalWrite(D0, HIGH);
  delay(100);
  digitalWrite(D0, LOW);
  delay(100);
}
/**
  Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
const char* host    = "rfid-iot.appspot.com";
void send_data(const char* hostconst char* host,String data) {
  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // api reqest GET https://<domain>/input/<data id rfid>
  // We now create a URI for the request
  String url = "/input/";
    url += data;
  //  url += "?private_key=";
  //  url += private_key;
  //  url += "&uid=";
  //  url += data;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  uid = "";
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (line.startsWith("HTTP/1.1 200 OK")) {
      Serial.println("send successfull!");
      beep();
      break;
    } else {
      //Serial.print(line);
      Serial.println("insert again");
      beep();
      beep();
      for (byte i = 0; i < 4; i++) {
        nuidPICC[i] = 0xFF;
      }
      break;
    }
  }
//  while(client.available()){
//    String line = client.readStringUntil('\r');
//    Serial.print(line);
//  }
  Serial.println();
  Serial.println("closing connection");
}
void connectWifi(const char* ssid, const char* password) {
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
