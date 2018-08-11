#include <DHT.h>
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"

#define WIFI_AP "ttonlywifi"      //vanke535
#define WIFI_PASSWORD "ttsocute"  //12345678

#define DHTPIN 6           // 定義DHT11訊號輸出腳
#define DHTTYPE DHT11      // 定義DHT型號
DHT dht(DHTPIN, DHTTYPE); // 初始化感測器

char MqttServer[] = "172.20.10.14";  //192.168.123.13

// 初始化以太网客户端对象 -- WiFiEspClient.h
WiFiEspClient espClient;


// 初始化MQTT库PubSubClient.h的对象
PubSubClient client(espClient);

SoftwareSerial soft(7, 13); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;


void setup() {

  Serial.begin(9600);

  dht.begin();

  InitWiFi();                                // 连接WiFi
  client.setServer( MqttServer, 1883 );      // 连接WiFi之后，连接MQTT服务器

  lastSend = 0;
}

void loop() {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("[loop()]Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // 连接WiFi热点
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("[loop()]Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // 用于定时1秒钟发送一次数据
    getAndSendTemperatureAndHumidityData(); // 获取温湿度数据发送到MQTT服务器上去
    lastSend = millis();
  }

  client.loop();
}

/*
 * 
 * 读取温湿度数据，然后发送到MQTT服务器上去
 * 
 */
void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  //宣告並讀取攝氏溫度與溼度，大概250ms读取一次
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float l = 909.30;
  float n = 82.03;
  float cs = 0.49;
  float cb = 0.49;

  // 查看是否读取温湿度失败的
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

   Serial.print("TEMP. : ");
   Serial.print(t);
   Serial.println("*C");
   Serial.print("Humidity : ");
   Serial.print(h);
   Serial.println("%");
   Serial.println("---------------------------");

   delay(3000);

  String temperature = String(t);
  String humidity = String(h);
  String light = String(l);
  String noise = String(n);
  String co2_simulation = String(cs);
  String co2_binarization = String(cb);

  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature );
  Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );

  // 构建一个 JSON 格式的payload的字符串
  String payload = "{";
  //payload += "\"time\":"; 
  //payload += "\"2018-07-22/13:33:33\"";
  //payload += ",";
  //payload += "\"localshortaddr\":"; 
  //payload += "\"F5A1\""; 
  //payload += ",";
  //payload += "\"gateway_id\":"; 
  //payload += "0"; 
  //payload += ",";
  //payload += "\"slaveId\":"; 
  //payload += "1"; 
  //payload += ",";
  payload += "\"humidity\":";
  payload += humidity;
  payload += ",";
  payload += "\"temperature\":"; 
  payload += temperature;
  payload += ",";
  payload += "\"light\":"; 
  payload += light; 
  payload += ",";
  payload += "\"noise\":"; 
  payload += noise;
  payload += ",";
  payload += "\"co2_simulation\":"; 
  payload += co2_simulation;
  payload += ",";
  payload += "\"co2_binarization\":"; 
  payload += co2_binarization;
  payload += "}";

  // Send payload
  char attributes[150];
  payload.toCharArray( attributes, 150 );

  // boolean publish(const char* topic, const char* payload);

  client.publish( "0/node/data/sensors", attributes );
  Serial.print("[publish]-->>");
  Serial.println( attributes );
}

void InitWiFi()
{
  // 初始化软串口，软串口连接ESP模块
  soft.begin(9600);
  // 初始化ESP模块
  WiFi.init(&soft);
  // 检测WiFi模块在不在，宏定义：WL_NO_SHIELD = 255,WL_IDLE_STATUS = 0,
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  Serial.println("[InitWiFi]Connecting to AP ...");
  // 尝试连接WiFi网络
  while ( status != WL_CONNECTED) {
    Serial.print("[InitWiFi]Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("[InitWiFi]Connected to AP");
}

/**
 * 
 * MQTT客户端断线重连函数
 * 
 */

void reconnect() {
  // 一直循环直到连接上MQTT服务器
  while (!client.connected()) {
    Serial.print("[reconnect]Connecting to MQTT Server ...");
    // 尝试连接connect是个重载函数 (clientId, username, password)
    if ( client.connect("yen", NULL, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ mqtt connect error code = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );// Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
