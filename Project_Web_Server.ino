#include <DHT.h>
#define DHTPIN D4                 // Digital pin connected to DHT11 GPIO2
#define DHTTYPE DHT11             // DHT 11

DHT dht(DHTPIN, DHTTYPE);         // Initialize DHT sensor

#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define SSID        "XXXXXXXXXX"      // ให้ใส่ ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD    "XXXXXXXXXX"  // ใส่ รหัส Wifi

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

////////////////////////////////////////////////////////////////////////////////
//Web Server
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2> Node Status </h2>
  <p>
    <i class="fas fa-thermometer-half"  style="color:#059e8a;"></i>
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units"> %% </sup>
  </p>
  
  <p>
    <i class="fas fa-solid fa-smoking" style="color:#A09B9A;"></i>
    <span class="dht-labels">Smoke</span>
    <span id="smoke">%SMOKE%</span>
  </p>

  <p>
    <i class="fas fa-running fa-lg" style="color:#FFAF50;"></i>
    <span class="dht-labels">Motion : </span>
    <span id="motions">%MOTION%</span>
  </p>

  
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("smoke").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/smoke", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("motions").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/motions", true);
  xhttp.send();
}, 10000 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "SMOKE"){
    return String(s);
  }
  else if(var == "MOTION"){
    if (state == HIGH) {
    motions = "Motion detected!";
    }
    if (state == LOW) {
      motions = "No motion detected.";
    }
    return String(motions);
  }

  return String();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
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
 
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });
  server.on("smoke", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(s).c_str());
  });

  // Start server
  server.begin();

  
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

}
