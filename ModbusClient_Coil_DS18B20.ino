/*
  Originally by Yaser Ali Husen
  This program to control PLC Siemens S7-1200 from ESP8266 using modbus TCP communication
  PLC S7 as modbus TCP server
  ESP8266 as modbus Client
  - For connection, dont forget to create bridge between Wifi and ethernet
  - Set PLC IP address same with network
  GPIO Reference: https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
  Library: https://github.com/emelianov/modbus-esp8266
*/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>

//-----------For modbus------------
IPAddress remote(192, 168, 1, 201);  // Address of Modbus Slave device
ModbusIP mb;                         //ModbusIP object
const int REG = 100;                 // Modbus Coils Offset
//-----------For modbus------------

#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
//const int oneWireBus = 4;
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

int deviceCount = 0;

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
//for sensor--------------------------------------

//Used Pins
int button1 = 5;  //D1
int button2 = 4;  //D2
int button3 = 0;  //D4
bool status_button1 = false;
bool status_button2 = false;
bool status_button3 = false;

//millis================================
//Set every 5 sec read sensor
unsigned long previousMillis = 0;  // variable to store the last time the task was run
const long interval = 1000;        // time interval in milliseconds (eg 1000ms = 1 second)
//======================================

void setup() {
  // DS18b20 sensor
  sensors.begin();
  Serial.begin(9600);

  WiFi.begin("ssid", "pwpwpwpwpwpw");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
    // Locate the devices on the bus:
  Serial.println("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount);
  Serial.println(" devices");
  // determine how many DS18B20 sensors are present

  uint8_t address[8];
 
  if(sensors.getAddress(address, 0)){
    Serial.print("Address fetched:");
  }else{
    Serial.println("Error fetching the address");
    return;
  }
 
  for(int i = 0; i<8; i++){
    Serial.printf("%02X ",address[i]);
  }
  
  mb.client();

  //Setup Pin
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);

}

void loop() {
  delay(50);
  unsigned long currentMillis = millis();  // mendapatkan waktu sekarang
  // Checks whether it is time to run the task
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the task was run
    previousMillis = currentMillis;
    //read DS18B20---------------------------------------
    sensors.requestTemperatures(); 
    float t = sensors.getTempCByIndex(0);
    //Serial.print("Humidity = ");
    //Serial.print(h);
    //Serial.print("% ");
    Serial.print("Temperature = ");
    Serial.print(t);
    Serial.print(" \xC2\xB0"); // shows degree symbol
    Serial.println(" C ");
    //read DS18B20---------------------------------------
    //split decimal and integer value
    //temperature
    int t_int = int(t);
    int t_dec = int((t - t_int) * 100);
    //humidity
    //int h_int = int(h);
    //int h_dec = int((h - h_int) * 100);

    //write value temp and humidity to holding registers
    //register 0 and 1 for temperature
    mb.writeHreg(remote,0,t_int);
    delay(50);
    mb.writeHreg(remote,1,t_dec);
    //delay(50);
    //register 2 and 3 for humidity
    //mb.writeHreg(remote,2,h_int);
    //delay(50);
    //mb.writeHreg(remote,3,h_dec);   
  }

  if (!mb.isConnected(remote)) {  // Check if connection to Modbus Slave is established
    mb.connect(remote);           // Try to connect if no connection
    Serial.print(".");
  }
  mb.task();  // Common local Modbus task
  check_button();
}

void check_button() {
  //check button1=================================
  int buttonValue1 = digitalRead(button1);
  if (buttonValue1 == LOW) {
    if (status_button1 == false) {
      //turn on
      status_button1 = true;
      Serial.println("Turn ON Coil1");
      mb.writeCoil(remote, 0, true);
    }
  } else if (buttonValue1 == HIGH) {
    if (status_button1 == true) {
      //turn off
      status_button1 = false;
      Serial.println("Turn OFF Coil1");
      mb.writeCoil(remote, 0, false);
    }
  }
  //============================================

  //check button2=================================
  int buttonValue2 = digitalRead(button2);
  if (buttonValue2 == LOW) {
    if (status_button2 == false) {
      //turn on
      status_button2 = true;
      Serial.println("Turn ON Coil2");
      mb.writeCoil(remote, 1, true);
    }
  } else if (buttonValue2 == HIGH) {
    if (status_button2 == true) {
      //turn off
      status_button2 = false;
      Serial.println("Turn OFF Coil2");
      mb.writeCoil(remote, 1, false);
    }
  }
  //============================================

  //check button3=================================
  int buttonValue3 = digitalRead(button3);
  if (buttonValue3 == LOW) {
    if (status_button3 == false) {
      //turn on
      status_button3 = true;
      Serial.println("Turn ON Coil3");
      mb.writeCoil(remote, 2, true);
    }
  } else if (buttonValue3 == HIGH) {
    if (status_button3 == true) {
      //turn off
      status_button3 = false;
      Serial.println("Turn OFF Coil3");
      mb.writeCoil(remote, 2, false);
    }
  }
  //============================================
}
