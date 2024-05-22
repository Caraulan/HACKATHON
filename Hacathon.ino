#include <DHT.h>
#include <WiFi.h>
#include <ArduinoMqttClient.h>

#define DHTPIN 22 
#define LedPIN 19
#define PIN_MQ2 33
#define BuzzerPin 25

int gasValue;
float h,t;
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "MikroLAB";        // your network SSID (name)
char pass[] = "@2wsxdr%5";    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char broker[] = "91.121.93.94";
int        port     = 1883;
char topic[]  = "Temperatura";
char topic2[]  = "Umiditate";
char topic3[]  = "Gas";

//set interval for sending messages (milliseconds)
const long interval = 10000;
unsigned long previousMillis = 0;

int count = 0;

// Одна из следующих строк закоментирована. Снимите комментарий, если подключаете датчик DHT11 к arduino
DHT dht(DHTPIN, DHT11); //Инициация датчика



int readSensor() {
  unsigned int sensorValue = analogRead(PIN_MQ2);  // Read the analog value from sensor
  unsigned int outputValue = map(sensorValue, 0, 1023, 0, 255); // map the 10-bit data to 8-bit data
  return outputValue;             // Return analog moisture value
}
//DHT dht(DHTPIN, DHT11);
void setup() {
  pinMode(PIN_MQ2, INPUT);
  pinMode(LedPIN, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  dht.begin();
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
 
}

void Send_mqqt_data(int data, char topic_broker[]){
    mqttClient.poll();
    Serial.print("Sending message to topic: ");
    Serial.println(topic_broker);
    Serial.println(data);

    mqttClient.beginMessage(topic_broker);
    mqttClient.print(data);
    mqttClient.endMessage();

    Serial.println();
    delay(200);
  
}

void loop() {
   h = dht.readHumidity(); //Измеряем влажность
   t = dht.readTemperature(); //Измеряем температуру

    if (isnan(h) || isnan(t)) {  // Проверка. Если не удается считать показания, выводится «Ошибка считывания», и программа завершает работу
    Serial.println("Ошибка считывания");
    return;
  }
  
 if(t >= 25){
  digitalWrite(LedPIN, HIGH);

 }else{
  digitalWrite(LedPIN, LOW);
 }

  gasValue = readSensor();

  Serial.print("GasValue:");
  Serial.println(gasValue);
    if(gasValue >= 450){
      tone(BuzzerPin, 1000); // Send 1KHz sound signal...
      delay(500);        // ...for 1 sec
      noTone(BuzzerPin);     // Stop sound...
     delay(500);        // ...for 1sec
  }else{
   noTone(BuzzerPin);
  }


  Send_mqqt_data(t, topic);
  delay(100);

  Send_mqqt_data(h, topic2);
  delay(100);

  Send_mqqt_data(gasValue, topic3);
  delay(100);


}
