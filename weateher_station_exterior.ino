#define ARDUINOJSON_USE_LONG_LONG 1
#define LARGE_JSON_BUFFERS 1
#include <Arduino.h>
#include <Thing.h>
#include <WebThingAdapter.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
/*Put your SSID & Password*/
char ssid[] = "Xaragall";
char password[] = "a78k7bkl9nj0al11u";
float temp, hum, pres, altitude;
// Hostname used by mDNS
const String mDNSHostname = "weathersensor_exterior";
WebThingAdapter *adapter;
const char* bme280Types[] = {"TemperatureSensor", nullptr};
ThingDevice weather("Exterior", "Xafarder", bme280Types);
ThingProperty weatherTemp("temperature", "", NUMBER, "TemperatureProperty");
// Set humidity as level-property
ThingProperty weatherHum("humidity", "", NUMBER, "LevelProperty");
ThingProperty weatherPres("pressure", "", NUMBER, "nullptr");


// Function reading values from BME280 and updates Thingproperties
void readBME280Data() {
  temp = bme.readTemperature();
  hum = bme.readHumidity();
  pres = (bme.readPressure() / 100.0F) + 4;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  ThingPropertyValue value;
  value.number = temp;
  weatherTemp.setValue(value);
  value.number = pres;
  weatherPres.setValue(value);
  value.number = hum;
  weatherHum.setValue(value);
  Serial.println("Tempertaure: ");
  Serial.println(temp);
  Serial.println("pressure: ");
  Serial.println(pres);
  Serial.println("humidity: ");
  Serial.println(hum);
}

void setup() {
  bme.begin(0x76);  
  Serial.begin(115200);
  delay(2000);
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(ssid);
  Serial.println("\"");
#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
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
  
    // Set title to "Humidity"
    weatherHum.title = "Humitat";
    // Set unit for humidity to %
    weatherHum.unit = "percent";
    // Set humidity as read only
    weatherHum.readOnly = "true";
    // Set min and max for LevelProperty
    weatherHum.minimum = 0;
    weatherHum.maximum = 100;
    weatherPres.minimum = 950;
    weatherPres.maximum = 1050;
  
    weather.addProperty(&weatherTemp);
    weather.addProperty(&weatherPres);
    weather.addProperty(&weatherHum);
    adapter->addDevice(&weather);
    adapter->begin();
  }
}

void loop() {
  readBME280Data();
  adapter->update();
  delay(300000);
}
