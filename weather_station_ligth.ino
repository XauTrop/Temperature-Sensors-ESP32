#define ARDUINOJSON_USE_LONG_LONG 1
#define LARGE_JSON_BUFFERS 1
#include <Arduino.h>
#include <Thing.h>
#include <WebThingAdapter.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <HTTPClient.h>
#define SEALEVELPRESSURE_HPA (1013.25)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0,21,22,U8X8_PIN_NONE);
Adafruit_BME280 bme;
/*Put your SSID & Password*/
const char ssid[] = "Xaragall";
const char password[] = "a78k7bkl9nj0al11u";
int status = WL_IDLE_STATUS;
const char* serverExtTemp = "http://192.168.1.46/things/Exterior/properties/temperature";
const char* serverExtHum = "http://192.168.1.46/things/Exterior/properties/humidity";
float temp, hum, pres, altitude, valLeni, valLeno;
int temp_oled, hum_oled, pres_oled, sfti, sfto, ext_temp, ext_hum;
//Variables
const int sensorPin = 33;     //analog input pin constant
int Lig_sensorValue;
const int light=54;
int human_touch;
String ext_hum_string, ext_temp_string;
// Hostname used by mDNS
const String mDNSHostname = "weathersensor_exterior";
WebThingAdapter *adapter;
const char* bme280Types[] = {"TemperatureSensor", nullptr};
ThingDevice weather("Menjador", "Xafec", bme280Types);
ThingProperty weatherTemp("temperature", "", NUMBER, "TemperatureProperty");
// Set humidity as level-property
ThingProperty weatherHum("humidity", "", NUMBER, "LevelProperty");
ThingProperty weatherPres("pressure", "", NUMBER, "nullptr");
ThingProperty weatheratmbrightness("atm_brigthness", "", NUMBER, "levelProperty");

#define ironman_width 44
#define ironman_height 64
static const unsigned char ironman_bits[] PROGMEM = {
 0x00, 0x00, 0xfc, 0x03, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x3f, 0x00, 0x00,
 0x00, 0xf8, 0xff, 0xff, 0x01, 0x00, 0x00, 0x86, 0xff, 0x1f, 0x06, 0x00,
 0x80, 0x81, 0xff, 0x1f, 0x18, 0x00, 0xc0, 0x80, 0xff, 0x1f, 0x30, 0x00,
 0x20, 0x80, 0xff, 0x1f, 0xc0, 0x00, 0x10, 0x00, 0xff, 0x0f, 0x80, 0x01,
 0x08, 0x00, 0xff, 0x0f, 0x00, 0x01, 0x08, 0x00, 0xff, 0x0f, 0x00, 0x02,
 0x08, 0x00, 0xff, 0x0f, 0x00, 0x02, 0x08, 0x00, 0xfe, 0x07, 0x00, 0x02,
 0x08, 0x00, 0xfe, 0x07, 0x00, 0x02, 0x08, 0x00, 0xfe, 0x07, 0x00, 0x02,
 0x08, 0x00, 0x00, 0x00, 0x00, 0x06, 0x08, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x08, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x0c, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x0c, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x0c, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x06, 0x00, 0x00, 0x00, 0x00, 0x0e, 0xc6, 0x07, 0x00, 0x00, 0x7c, 0x0e,
 0xe6, 0xff, 0xff, 0xff, 0xff, 0x0e, 0xe6, 0xff, 0x03, 0xf8, 0xff, 0x0c,
 0x02, 0x3f, 0x00, 0x80, 0x1f, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0c,
 0x03, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x0c,
 0x03, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x0c,
 0x03, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x07, 0x00, 0x00, 0x00, 0x00, 0x06, 0x16, 0x00, 0x00, 0x00, 0x00, 0x06,
 0x2e, 0x00, 0x00, 0x00, 0x80, 0x07, 0x6e, 0x00, 0x00, 0x00, 0x40, 0x07,
 0x6e, 0x00, 0x00, 0x00, 0x60, 0x07, 0xde, 0x00, 0x00, 0x00, 0xb0, 0x07,
 0xde, 0x00, 0x00, 0x00, 0xb0, 0x07, 0x9c, 0x01, 0x00, 0x00, 0xd8, 0x03,
 0xbc, 0x01, 0x00, 0x00, 0xf8, 0x03, 0x3c, 0x01, 0x00, 0x00, 0xe8, 0x03,
 0x7c, 0x03, 0x00, 0x00, 0xf4, 0x03, 0xfc, 0x02, 0x00, 0x00, 0xf4, 0x03,
 0xfc, 0xc2, 0xff, 0x1f, 0xfa, 0x03, 0xf8, 0x25, 0xff, 0x27, 0xfa, 0x01,
 0xf8, 0x38, 0x00, 0x60, 0xf9, 0x01, 0xf8, 0x10, 0x00, 0xc0, 0xf8, 0x01,
 0xf0, 0x00, 0x00, 0x00, 0xf8, 0x01, 0xf0, 0x00, 0x00, 0x00, 0xf8, 0x01,
 0xf0, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0xe0, 0xc1, 0xff, 0x0f, 0xf8, 0x00,
 0xc0, 0xe3, 0xff, 0x1f, 0x7c, 0x00, 0x80, 0xff, 0xff, 0x7f, 0x3e, 0x00,
 0x00, 0xff, 0xff, 0x7f, 0x0f, 0x00, 0x00, 0xf6, 0x00, 0xfe, 0x07, 0x00,
 0x00, 0x3c, 0x00, 0xb0, 0x01, 0x00, 0x00, 0x08, 0x00, 0x40, 0x00, 0x00 };

// Function reading values from BME280 and updates Thingproperties
void draw(void) {
 // graphic commands to redraw the complete screen should be placed here  
 u8g2.drawXBMP( 42, 0, ironman_width, ironman_height, ironman_bits);
}

void print_oled(){
  temp_oled = (int) temp;
  hum_oled = (int) hum;
  pres_oled = (int) pres;
  u8g2.firstPage();
  u8g2.setContrast(0);
  do {
    //En este do while podemos dibujar o escribir lo que queramos mostrar en pantalla.
    //Como ejemplo un Hello World.
    u8g2.setFont(u8g2_font_fub35_tf);
    u8g2.setCursor(1, 40);
    u8g2.print((int) temp_oled);
    u8g2.setFont(u8g2_font_fub11_tf);
    u8g2.setCursor(56-sfti, 14);
    u8g2.print("o");
    u8g2.setFont(u8g2_font_fub17_tr);
    u8g2.setCursor(1+sfto, 63);
    u8g2.print(ext_temp);
    u8g2.setFont(u8g2_font_fub11_tr);
    u8g2.setCursor(28, 54);
    u8g2.print("o");
    u8g2.setFont(u8g2_font_fub17_tr);
    u8g2.setCursor(76, 24);
    u8g2.print((int) hum_oled);
    u8g2.setCursor(104, 24);
    u8g2.print("%");
    u8g2.setFont(u8g2_font_fub11_tr);
    u8g2.setCursor(90, 42);
    u8g2.print((int) ext_hum);
    u8g2.setCursor(111, 42);
    u8g2.print("%");
    u8g2.setFont(u8g2_font_fub11_tr);
    u8g2.setCursor(60, 63);
    u8g2.print((int) pres_oled);
    u8g2.setCursor(100, 63);
    u8g2.print("hPa");
  } while ( u8g2.nextPage() );
}


void readData() {
  temp = bme.readTemperature();
  hum = bme.readHumidity();
  pres = (bme.readPressure() / 100.0F) + 4;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Lig_sensorValue = analogRead(sensorPin);
//  int light_percent = Lig_sensorValue*0.0518 
  int light_percent = (18.733*log(Lig_sensorValue))-0.9054;
  ThingPropertyValue value;
  value.number = temp;
  weatherTemp.setValue(value);
  value.number = pres;
  weatherPres.setValue(value);
  value.number = hum;
  weatherHum.setValue(value);
  value.number = light_percent;
  weatheratmbrightness.setValue(value); 
  Serial.println("Tempertaure: ");
  Serial.println(temp);
  Serial.println("pressure: ");
  Serial.println(pres);
  Serial.println("humidity: ");
  Serial.println(hum);
  Serial.println("light: ");
  Serial.println(Lig_sensorValue);
}

void setup() {
  bme.begin(0x76);
  u8g2_SetI2CAddress(u8g2.getU8g2(), 0x3C * 2);
  u8g2.begin();  
  Serial.begin(115200);
  delay(2000);
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(ssid);
  Serial.println("\"");

// Print image
   u8g2.firstPage();
   do {
     draw();
    } while( u8g2.nextPage() );
    
#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    delay(1000);
    adapter = new WebThingAdapter(mDNSHostname, WiFi.localIP());
   // Set unit for temperature
    weatherTemp.unit = "degree celsius";
    // Set title to "Temperature"
    weatherTemp.title = "Temperatura";
    // Set title to "Pressure"
    weatherPres.title = "Pressió";
    // Set unit for pressure to hPa
    weatherPres.unit = "hPa";
    // Set pressure to read only
    weatherPres.readOnly = "true";
    
    weatheratmbrightness.unit = "%";
    // Set title to "Temperature"
    weatheratmbrightness.title = "Il·luminació";
    
    // Set title to "Humidity"
    weatherHum.title = "Humitat";
    // Set unit for humidity to %
    weatherHum.unit = "percent";
    // Set humidity as read only
    weatherHum.readOnly = "true";
    // Set min and max for LevelProperty
    weatherHum.minimum = 0;
    weatherHum.maximum = 100;
    weatheratmbrightness.minimum = 0;
    weatheratmbrightness.maximum = 100;
  
    weather.addProperty(&weatherTemp);
    weather.addProperty(&weatherPres);
    weather.addProperty(&weatherHum);
    weather.addProperty(&weatheratmbrightness);
    adapter->addDevice(&weather);
    adapter->begin();
  }
}

void loop() {
  readData();
  ext_temp_string = httpGETRequest(serverExtTemp);
  ext_hum_string = httpGETRequest(serverExtHum);
  ext_temp = ext_temp_string.substring(15,17).toInt();
  ext_hum = ext_hum_string.substring(12,14).toInt();
//  Serial.println("Temperature exterior: " + ext_temp + " *C");
  if(ext_temp > 9){
    valLeno = 0;
    }
  else if(ext_temp < 9 and ext_temp > 0){
    valLeno = 1;
  }
  else {
    valLeno = 0.5;
  }
  sfto = valLeno * 12;
  
  if(temp > 9){
    valLeni = 0;
    }
  else if(temp < 9 and temp > 0){
    valLeni = 1;
  }
  else {
    valLeni = 0.5;
  }
  sfti = valLeni * 26;
  print_oled();
  adapter->update();
  human_touch = touchRead(T0);
  delay(300000);
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
