/* 
  Dibuat Oleh 
  Christian Trisno Sen Long Chen - 219116782
 */
 /* 
  Init Library
 */
//LCD
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
// WIFI
#include <WiFi.h>
//MQTT
#include <PubSubClient.h>
/* 
  Init Variables
*/
String words[100];
String messageCallback;
unsigned long int prevMillis, currMillis;
// State :0 -> No Message & No Audio
// State :1 -> With Message
// State :2 -> Audio
// State :3 -> Motion Detected?
int state = 0;
int loopMessage = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
/* 
  Utility Methods
*/
int split(String string, char ch){
  String data = "";
  int ctr = 0;
  for (int i = 0; i < string.length(); i++)
  {
    char c = string[i];
    
    if (c != ch)
    {
      data += c;
    }
    else{
      words[ctr++] = data;
      data = "";
    }
  }

  return ctr;
}
int combine_words(int count){
  int ctr = 0;
  for (int i = 0; i < count; i++){
    String word= words[i];
    if(i+1> count){
      words[ctr] = word;
      Serial.println(word);
      ctr++;
    }
    else{
      String next = words[i+1];
      if (word.length() + next.length() <16){
        words[ctr] = word + " " + next;
        ctr++;
        i++;
      }
    }
  }
  return ctr;
}
void changeState(int i){
  lcd.clear();
  state = i;
}
int calculateOffset(String str){
  int len = str.length();
  return 8 - (len/2);
}

/* 
  LCD Section
 */
#define SDA 21
#define SCL 22

void setupLCD(){
  Wire.begin(SDA, SCL);
  lcd.init();
  lcd.backlight();
  lcd.begin(16,2);
}
void printText(int x, int y, String s){
  lcd.setCursor(x,y);
  lcd.print(s);
}
void scanLCDAddress(){
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
    delay(5000);   
}
int pos = 0;
void scrollTextHorizontal(int row, String message, int delayTime, int lcdColumns) {
  currMillis = millis();
  if(currMillis - prevMillis >=delayTime){
    for (int i=0; i < lcdColumns; i++) {
        message = " " + message; 
      }
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    pos++;
    pos = pos % message.length();
    prevMillis = currMillis;
  }
}
void scrollingTextVertical(String sentence){
  int loop = 5;
  Serial.println(sentence);
  int count = split(sentence, ' ');
  count = combine_words(count);
  for (int i = 0; i < count; ++i)
  {
    if (words[i].length() > 0){
      Serial.println(words[i]);
      printText(calculateOffset(words[i]),0,words[i]);
    }
    if (words[i+1].length() > 0 && i+1 < count){
      Serial.println(words[i+1]);
      printText(calculateOffset(words[i+1]),1,words[i+1]);
    }
    
    delay(1000);
    lcd.clear();
  }
}
/* 
  WIFI Setup
 */
const char* ssid = "Mama Mix";
const char* password = "mamamix88";
String serverName = "192.168.1.11";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
//String serverName = "example.com";   // OR REPLACE WITH YOUR DOMAIN NAME

WiFiClient client;
void setupWIFI(){
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());
}
/* 
  MQTT Setup
 */
PubSubClient mqttClient(client);
const char *mqtt_broker = "51.79.161.0";
const char *topic = "esp32";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 messageCallback = "";
 for (int i = 0; i < length; i++) {
     messageCallback +=String((char) payload[i]);
 }
 messageCallback += " ";
 Serial.println(messageCallback);
 changeState(1);
 Serial.println("-----------------------");
}
void setupMQTT(){
 mqttClient.setServer(mqtt_broker, mqtt_port);
 Serial.println(topic);
 mqttClient.setCallback(callback);
 while (!mqttClient.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (mqttClient.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("MQTT Server connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(mqttClient.state());
         delay(2000);
     }
 }
 mqttClient.subscribe(topic);
}

void setup() {
  Serial.begin(9600);
  setupWIFI();
  setupLCD();
  setupMQTT();
  // scrollingTextVertical("Hello World My Name is Christian");
}

void loop() {
  mqttClient.loop();
  if(state == 0){
    printText(1,0, "HELLO STRANGER!");
    scrollTextHorizontal(1, "Welcome to Chen's Home! ", 500, 16);
  }
  else if(state == 1){
    loopMessage++;
    if(loopMessage<=5){
      scrollingTextVertical(messageCallback);
    }
    else {
      changeState(0);
      loopMessage = 0;
    }
  }
  else if(state == 2){
    
  }
}
