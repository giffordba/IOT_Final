#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

int soilSensor = A0;
int reading = -1;
int data = -1;
int old_data = -1;
int val = -1;   

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  
  BLE.setLocalName("Soil Sensor"); 
  BLE.advertise();

  Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
  Serial.println(" ");
}

void loop() {
  connectToPeripheral();
}

void connectToPeripheral(){
  BLEDevice peripheral;
  
  Serial.println("- Discovering peripheral device...");

  do
  {
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
  } while (!peripheral);
  
  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic sensorCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);
    
  if (!sensorCharacteristic) {
    Serial.println("* Peripheral device does not have sensor_type characteristic!");
    peripheral.disconnect();
    return;
  } else if (!sensorCharacteristic.canWrite()) {
    Serial.println("* Peripheral does not have a writable gesture_type characteristic!");
    peripheral.disconnect();
    return;
  }
  
  while (peripheral.connected()) {
    data = sensorReading();

    if (old_data != data) {  
      old_data = data;
      Serial.print("* Writing value to gesture_type characteristic: ");
      Serial.println(data);
      sensorCharacteristic.writeValue((byte)data);
      Serial.println("* Writing value to sensor_type characteristic done!");
      Serial.println(" ");
    }
  
  }
  Serial.println("- Peripheral device disconnected!");
}
  
int sensorReading() {
  reading = analogRead(soilSensor);
  if(reading >= 650){
    //Here is where the watering cycle begins due to dry soil
    Serial.write("Soil is too dry. Starting water cycle... ");
    Serial.write('\n');
    val = 2;
    delay(1000);
    return val;
  }
  else if ((reading <= 649) && (reading >= 450)){
    Serial.write("Plant soil is getting dry. Water cycle will begin soon");
    Serial.write('\n');
    val = 1;
    delay(1000);
    return val;
  }

  else if (reading <= 449){
    Serial.write("Plant has been watered reciently");
    Serial.write('\n');
    val = -1;
    delay(1000);
    return val;
  }
}
