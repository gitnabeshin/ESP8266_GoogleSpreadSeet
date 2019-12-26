/***
 * https://script.google.com/macros/s/XXXXXXXXXXXXXXXXXXXXXXXXXXXX/exec
 * 
 *  // google.com に接続が拒否される場合、setup()にこれを挿入する。
 *  // https://github.com/esp8266/Arduino/issues/4826
 *  client.setInsecure();
 *  
 *  Arduino IDE Setting
 *  Board:  Generic ESP8266 Module 
 *  CPU:    80MHz
 *  FLASH:  40Mhz
 *  FlashMode:  DOUT(compatible)
 *  FlashSize:  4M(3MSPIFFS)  
 *  ResetMode:  nodemcu
 *  
 */
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define ANALOG_IN_PIN (A0)  // ESP8266 Analog Pin ADC0 = A0

WiFiClientSecure client;

const char* ssid      = "*****";    
const char* password  = "*****";  
const char* server    = "script.google.com"; 
// google script key
const char* key = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

double sensorVal  = 0;  //ADコンバータの値 0 - 1024
double voltage    = 0;  //電圧計算値: 3.3v --> 14.5V 換算

void setup() {

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // google.com に接続が拒否される場合、setup()にこれを挿入する。
  // https://github.com/esp8266/Arduino/issues/4826
  client.setInsecure();

  for(int i=0; i<6; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(600);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
}

void wifi_conect() {

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_BUILTIN, HIGH);  //LED ON
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void g_acsess(){
   
  String URL="https://script.google.com/macros/s/";
  URL += key;
  URL += "/exec?";
  URL += "&1_cell="; 
  URL += sensorVal ;
  URL += "&2_cell="; 
  URL += voltage ;
//  URL += "&3_cell="; 
//  URL += sensor_data3 ;
   
  Serial.println(URL);
  // サイトにアクセス
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    client.println("GET " + URL);
     client.stop();
     Serial.println("HTTP GET finished.\n");
  } 
}
  
void loop() {
  // read the analog in value 3V  <---- 12V 
  sensorVal = analogRead( ANALOG_IN_PIN );
  // 14.5Vに換算する
  voltage = ( sensorVal/1024 ) * 14.5;
  // 実測値ベースで補正: センサ取得値12.5vに対し実際は12.05、12.05/12.5=0.96
  voltage = voltage * 0.96;

  Serial.print("Sensor:");
  Serial.print( sensorVal );
  Serial.print(", Voltage:");
  Serial.println( voltage );

  wifi_conect();
  g_acsess();   
  WiFi.disconnect();
  Serial.println("Wifi disconnected.\n");
  digitalWrite(LED_BUILTIN, LOW); //LED OFF

  Serial.println("###############################");
  Serial.println("starting deep sleep...");
  Serial.println("connect D0 and RST for wakeup!");
  Serial.println("###############################\n\n\n");

  // 書き込み後、D0とRSTを接続する事！！
  // DeepSleep （復帰タイマー時間[μ秒]: n(sec）* 1000,000 
  ESP.deepSleep( 30 * 60 * 1000000 );

  // DeepSleep移行待機
  delay( 1000 );
}
