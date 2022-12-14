#include <Wire.h>
#include "MQ2.h"
#include "MQ131.h"
#include "MICS6814.h"
#include "DSM501A.h"
#include "DHT.h"
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <WiFiManager.h>


#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

#define DHTTYPE     DHT22
#define DHTPin      22
#define MQ2Pin      34
#define MQ131Pin    35
#define MicsNH3Pin  0
#define MicsNO2Pin  32
#define MicsCOPin   33
#define PM25Pin     21
#define PM10Pin     19

MQ2       mq2(MQ2Pin);
MQ131     mq131(MQ131Pin);
MICS6814  mics6814(MicsCOPin, MicsNO2Pin, MicsNH3Pin);
DSM501A   particle(PM25Pin, PM10Pin);
DHT       dht(DHTPin, DHTTYPE);

float PM25 = 0;
float PM10 = 0;

unsigned long updatetime = 0;

typedef struct{
  float temperature; float humidity; float CHGas; float O3Gas;
  float NO2Gas; float COGas; float PM25; float PM10; float NH3Gas;
} Data_t;


// #if defined(ESP32)
//   #include <WiFiMulti.h>
//   WiFiMulti wifiMulti;
//   #define DEVICE "ESP32"
// #elif defined(ESP8266)
//   #include <ESP8266WiFiMulti.h>
//   ESP8266WiFiMulti wifiMulti;
//   #define DEVICE "ESP8266"
// #endif


int output;
// // WiFi AP SSID
// #define WIFI_SSID "msi"
// // WiFi password
// #define WIFI_PASSWORD "aswanzakky"
// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://ap-southeast-2-1.aws.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> Generate API Token)
#define INFLUXDB_TOKEN "Hu9VjuisgUHAnY0TrkUeDiV09EgZ1xq_WWgehYsitiGIGr64ECt49oWMza862EfCBCr7WqwlyPCkvlyWMPM3KQ=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "myOrg"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "bismillah"


// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
#define TZ_INFO "PKT-5"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data points
Point sensor("measurements");

void setup() {
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  bool res;  
  res = wm.autoConnect("AutoConnectAP","password");
  if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
    

  // put your setup code here, to run once:
  dht.begin();
  mq2.begin();
  mq131.begin();
  mics6814.begin();
  particle.begin();
  updatetime = millis();
  
  pinMode(DHTPin, INPUT);

  // WiFi.mode(WIFI_STA);
  // wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  // Serial.print("Connecting to wifi");
  // while (wifiMulti.run() != WL_CONNECTED) {
  //   Serial.print(".");
  //   delay(500);
  // }
  // Serial.println();
 
  // Add tags
  // sensor.addTag("device", DEVICE);
  // sensor.addTag("SSID", WiFi.SSID());
  
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  // Store measured values into points
  Data_t data;
  sensor.clearFields();

  data.PM25 = particle.readpm25();
  data.PM10 = particle.readpm10();
  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature();
  data.CHGas = mq2.readCH();
  data.O3Gas = mq131.readO3();
  data.NO2Gas = mics6814.measure(NO2); delay(100);
  data.COGas = mics6814.measure(CO); delay(100);
  data.NH3Gas = mics6814.measure(NH3); delay(100);


  Serial.print("Humidity: ");
  Serial.println(data.humidity);
  Serial.print("Temperature: ");
  Serial.println(data.temperature);
  Serial.print("O3Gas: ");
  Serial.println(data.O3Gas);
  Serial.print("CHGas: ");
  Serial.println(data.CHGas);
  Serial.print("NO2Gas: ");
  Serial.println(data.NO2Gas);
  Serial.print("COGas: ");
  Serial.println(data.COGas);

  
  sensor.addField("Temperature",data.temperature);
  sensor.addField("Humidity",data.humidity);
  sensor.addField("O3Gas",data.O3Gas);
  sensor.addField("CHGas",data.CHGas);
  sensor.addField("NO2Gas",data.NO2Gas);
  sensor.addField("COGas",data.COGas);

  if (millis() - (particle.starttime3) > particle.sampletime_ms){
    PM25 = data.PM25;
    PM10 = data.PM10;
    particle.starttime3 = millis();
  }

  Serial.print("PM25: ");
  Serial.println(PM25);
  Serial.print("PM10: ");
  Serial.println(PM10);
  sensor.addField("PM25",PM25);
  sensor.addField("PM10",PM10);
   

  
  Serial.print("Writing: ");
  Serial.println(client.pointToLineProtocol(sensor));

  // If no Wifi signal, try to reconnect it
  // if (wifiMulti.run() != WL_CONNECTED) {
  //   Serial.println("Wifi connection lost");
  // }
  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  Serial.println("");
}
