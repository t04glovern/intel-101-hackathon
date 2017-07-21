/**
   Include Libraries
*/
#include <CurieBLE.h>
#include <CurieTime.h>


/**
   BLE Peripheral Initialization
   - this is the board we are using -
*/
BLEPeripheral blePeripheral;


/**
   Define BLE Service
   - this is the service that hosts our arduino characteristics -
*/
BLEService arduinoSensorService("2571bb9c-3516-4399-94d4-7b531e97100c");


/**
   Define BLE Characteristics
   - Referenced by our central device (RPi) and polled for data

   Info: https://www.arduino.cc/en/Reference/BLECharacteristicConstructor
   Usage:
    > UUID
    > Properties (BLERead, BLENotify, BLEWrite)
    > Max Byte Length (Max allowed 20 bytes)
*/
BLECharacteristic timestamp("b43521f5-16ec-4936-849c-6c218e7bdab5", BLERead | BLENotify | BLEWrite, 20);
BLECharacteristic data_point_1("beb203a6-bdc3-4111-9569-b2a3aa121f00", BLERead, 5);
BLECharacteristic data_point_2("0d41699d-cae3-41d4-8504-86fd0072b31a", BLERead , 5 );
BLECharacteristic data_point_3("8cd25b99-1690-47bc-b3ac-de594fb313de", BLERead, 5);
BLECharacteristic data_point_4("0b86e58a-1456-410c-a69b-601aaeaa2a46", BLERead, 5);


/**
   Define Pins for our data points
   - Placeholders for actual sensor attachments
*/
int data_pin_1 = 1;
int data_pin_2 = 2;
int data_pin_3 = 3;
int data_pin_4 = 4;

/**
   Define out systems name
   - We will use this field as our identifier characteristic and local name
*/
String dev_id = "arduino-node-01";


void setup() {

  // Initialze serial port for debugging communications
  Serial.begin(115200);
  Serial.println("Starting " + dev_id + " communications");

  // Set data pin modes
  pinMode(data_pin_1, INPUT);
  pinMode(data_pin_2, INPUT);
  pinMode(data_pin_3, INPUT);
  pinMode(data_pin_4, INPUT);

  // BLE Setup
  blePeripheral.setLocalName("arduino-node-01");
  blePeripheral.setAdvertisedServiceUuid(arduinoSensorService.uuid());

  // BLE Service
  blePeripheral.addAttribute(arduinoSensorService);

  // BLE Characteristics for this service
  blePeripheral.addAttribute(timestamp);
  blePeripheral.addAttribute(data_point_1);
  blePeripheral.addAttribute(data_point_2);
  blePeripheral.addAttribute(data_point_3);
  blePeripheral.addAttribute(data_point_4);

  // Initial Sensor Get
  readSensors();

  // BLE Peripherals go live
  blePeripheral.begin();

}

void loop() {

  // Checks if any central devices try to connect
  BLECentral central = blePeripheral.central();
  
  if (central) {
    
    // When connected, prints central's MAC address
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      
      // While connected to central, we read our sensors
      readSensors();
      
    }

    // When central disconnects, print status
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void readSensors() {

  // Set/Get DateTime
  getTimestamp();

  // Get Data 1
  getDataPoint1();

  // Get Data 2
  getDataPoint2();

  // Get Data 3
  getDataPoint3();

  // Get Data 4
  getDataPoint4();

  return;
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void getTimestamp() {
  int datetime = now();
  char charArray[20];
  dtostrf(datetime, 20, 0, charArray);

  timestamp.setValue(charArray);

  Serial.print("Ok, Time = ");
  print2digits(hour());
  Serial.write(':');
  print2digits(minute());
  Serial.write(':');
  print2digits(second());
  Serial.print(", Date (D/M/Y) = ");
  Serial.print(day());
  Serial.write('/');
  Serial.print(month());
  Serial.write('/');
  Serial.print(year());
  Serial.println();
}

void getDataPoint1() {
  int sensorValue = digitalRead(data_pin_1);
  
  Serial.print("DataPoint1 is: ");
  Serial.println(sensorValue);

  char charArray[5];
  dtostrf(sensorValue, 5, 0, charArray);
  
  data_point_1.setValue(charArray);
}

void getDataPoint2() {
  int sensorValue = digitalRead(data_pin_2);
  
  Serial.print("DataPoint2 is: ");
  Serial.println(sensorValue);

  char charArray[5];
  dtostrf(sensorValue, 5, 0, charArray);
  
  data_point_2.setValue(charArray);
}

void getDataPoint3() {
  int sensorValue = digitalRead(data_pin_3);
  
  Serial.print("DataPoint3 is: ");
  Serial.println(sensorValue);

  char charArray[5];
  dtostrf(sensorValue, 5, 0, charArray);
  
  data_point_3.setValue(charArray);
}

void getDataPoint4() {
  int sensorValue = digitalRead(data_pin_4);
  
  Serial.print("DataPoint4 is: ");
  Serial.println(sensorValue);

  char charArray[5];
  dtostrf(sensorValue, 5, 0, charArray);
  
  data_point_4.setValue(charArray);
}


