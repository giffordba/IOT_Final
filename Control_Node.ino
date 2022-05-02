#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

int data = -1;
int pump = D10;
int warning = D8;
int confirm = D6;

BLEService sensorService(deviceServiceUuid); 
BLEByteCharacteristic sensorCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);


void setup() {
  pinMode(pump, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial);  
  
  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName("Control Node");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorCharacteristic);
  BLE.addService(sensorService);
  sensorCharacteristic.writeValue(-1);
  BLE.advertise();

  Serial.println("Nano 33 BLE (Control Node)");
  Serial.println(" ");
}

void loop() {
  BLEDevice central = BLE.central();
  Serial.println("- Discovering sensor device...");
  delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      if (sensorCharacteristic.written()) {
         data = sensorCharacteristic.value();
         writeControl(data);
       }
    }
    
    Serial.println("* Disconnected to central device!");
  }
}

void writeControl(int val) {
  if (val == 2){
    digitalWrite(pump, HIGH);
    digitalWrite(warning, LOW);
    digitalWrite(confirm, LOW);
    delay(15000);
    digitalWrite(pump, LOW);
  }
  if (val == 1){
    digitalWrite(pump, LOW);
    digitalWrite(warning, HIGH);
    digitalWrite(confirm, LOW);
  }
  else{
    digitalWrite(pump, LOW);
    digitalWrite(warning, LOW);
    digitalWrite(confirm, HIGH);
  }

    }      
