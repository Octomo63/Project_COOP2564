#include <DHT.h>
#define DHTPIN D4                 // Digital pin connected to DHT11 GPIO2
#define DHTTYPE DHT11             // DHT 11

DHT dht(DHTPIN, DHTTYPE);         // Initialize DHT sensor

#include <TridentTD_LineNotify.h>
#include <Hash.h>

#define SSID        "XXXXXXXXXXX"      // ให้ใส่ ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD    "XXXXXXXXXXX"  // ใส่ รหัส Wifi
#define LINE_TOKEN  "XXXXXXXXXXX"   // ใส่ รหัส TOKEN

//--------Smoke----------
int analogPin = A0; //ประกาศตัวแปร ให้ analogPin
float s = 0.0;

//-----Motion----
int motion = 13;  // Digital pin D7
int state = digitalRead(motion);
String motions = " ";

float h = 0.0;                     // กำหนดค่าเริ่มต้น 0
float t = 0.0;                     // กำหนดค่าเริ่มต้น 0
float f = 0.0;                     // กำหนดค่าเริ่มต้น 0

void setup() 
{
  //-----motion-----
  pinMode(motion, INPUT);
  
  Serial.begin(115200);           // Sets the data rate in bits per second (baud) for serial data transmission.
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  
  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);
  
}
void loop()
{
  h = dht.readHumidity();                        // Read humidity
  t = dht.readTemperature();                     // Read temperature as Celsius (the default)
  f = dht.readTemperature(true);                 // Read temperature as Fahrenheit (isFahrenheit = true)

  if (isnan(h) || isnan(t) || isnan(f)) {              // Check if any reads failed and exit early (to try again).
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hif = dht.computeHeatIndex(f, h);              // Compute heat index in Fahrenheit (the default)
  float hic = dht.computeHeatIndex(t, h, false);       // Compute heat index in Celsius (isFahreheit = false)

  //--------Smoke----------
  s = analogRead(analogPin);  //อ่านค่าสัญญาณ analog 

  if (state == HIGH) {
    motions = "Motion detected!";
  }
  if (state == LOW) {
    motions = "No motion detected.";
  }
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
  
  Serial.print("val: "); 
  Serial.println(s); //แสดงค่าที่วัดได้

  Serial.print("Motion : ");
  Serial.print(motions);

  //ตรวจสอบความผิดปกติของอุณหภูมิ แล้วแจ้งเตือนทางไลน์
  if(t < 18 || t > 27)
  {
    // ส่งข้อความแจ้งเตือน
    LINE.notify("อุณหภูมิผิดปกติ");
    LINE.notify("Humidity: " + String(h) + "%");
    LINE.notify("Temperature(Celsius): " + String(t) + "*C");
    LINE.notify("Temperature(Fahrenheit): " + String(f) + "*F");
    LINE.notify("Heat index(Celsius): " + String(hic) + "*C");
    LINE.notify("Heat index(Fahrenheit): " + String(hif) + "*F");
    LINE.notify("Smoke : " + String(s) );
    LINE.notify(motions);
    LINE.notifySticker(11538,51626518);          //ส่ง Line Sticker ด้วย PackageID 11538 , StickerID 51626518
    delay(10000);
  }

  //ตรวจสอบความผิดปกติของความชื้นสัมพัทธ์ แล้วแจ้งเตือนทางไลน์
  if(h < 40 || h > 60)
  {
    // ส่งข้อความแจ้งเตือน
    LINE.notify("ความชื้นสัมพัทธ์ผิดปกติ");
    LINE.notify("Humidity: " + String(h) + "%");
    LINE.notify("Temperature(Celsius): " + String(t) + "*C");
    LINE.notify("Temperature(Fahrenheit): " + String(f) + "*F");
    LINE.notify("Heat index(Celsius): " + String(hic) + "*C");
    LINE.notify("Heat index(Fahrenheit): " + String(hif) + "*F");
    LINE.notify("Smoke : " + String(s) );
    LINE.notify(motions);
    LINE.notifySticker(11538,51626522);          //ส่ง Line Sticker ด้วย PackageID 11538 , StickerID 51626522
    delay(10000);
  }
  //ตรวจสอบความผิดปกติ Smoke แล้วแจ้งเตือนทางไลน์
  if(s > 500)
  {
    // ส่งข้อความแจ้งเตือน
    LINE.notify("Smoke ผิดปกติ");
    LINE.notify("Humidity: " + String(h) + "%");
    LINE.notify("Temperature(Celsius): " + String(t) + "*C");
    LINE.notify("Temperature(Fahrenheit): " + String(f) + "*F");
    LINE.notify("Heat index(Celsius): " + String(hic) + "*C");
    LINE.notify("Heat index(Fahrenheit): " + String(hif) + "*F");
    LINE.notify("Smoke : " + String(s) );
    LINE.notify(motions);
    LINE.notifySticker(11538,51626523);          //ส่ง Line Sticker ด้วย PackageID 11538 , StickerID 51626523
    delay(10000);
  }
  //ตรวจสอบการเคลื่อนไหว แล้วแจ้งเตือนทางไลน์
  if(state == HIGH)
  {
    // ส่งข้อความแจ้งเตือน
    LINE.notify("Motion detected!");
    LINE.notify("Humidity: " + String(h) + "%");
    LINE.notify("Temperature(Celsius): " + String(t) + "*C");
    LINE.notify("Temperature(Fahrenheit): " + String(f) + "*F");
    LINE.notify("Heat index(Celsius): " + String(hic) + "*C");
    LINE.notify("Heat index(Fahrenheit): " + String(hif) + "*F");
    LINE.notify("Smoke : " + String(s) );
    LINE.notify(motions);
    LINE.notifySticker(1070,17875);          //ส่ง Line Sticker ด้วย PackageID 1070 , StickerID 17875
    delay(10000);
  }
  else
  {
    delay(10000);
  }
  
}
