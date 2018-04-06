#include <ESP8266WiFi.h>
void setup() 
{
    Serial.begin(115200);   //ตั้งค่าใช้งาน serial ที่ baudrate 115200
    delay(10);
    Serial.println();
    Serial.println();
    connectWifi("nightRobot","123456789");
}
void loop() {}
//const char* ssid     = "nightRobot";
//const char* password = "123456789";
void connectWifi(const char* ssid,const char* password){
    Serial.print("Connecting to "); //แสดงข้อความ  “Connecting to”
    Serial.println(ssid);       //แสดงข้อความ ชื่อ SSID 
    WiFi.begin(ssid, password); // เชื่อมต่อไปยัง AP
   
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
