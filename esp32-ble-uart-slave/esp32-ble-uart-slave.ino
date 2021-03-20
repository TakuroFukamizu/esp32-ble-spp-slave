#include <M5StickCPlus.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define DEVICE_NAME "BT_SPP_SLAVE"
#define UUID_VSP_SERVICE  "569a1101-b87f-490c-92cb-11ba5ea5167c" //VSP
#define UUID_RX "569a2001-b87f-490c-92cb-11ba5ea5167c" //RX
#define UUID_TX "569a2000-b87f-490c-92cb-11ba5ea5167c" //TX

//----------------------------

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pRxCharacteristic;
BLECharacteristic *pTxCharacteristic;

bool deviceConnected = false;

//----------------------------

void sendTx();


class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    M5.Lcd.println("connected!");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
        M5.Lcd.print(rxValue[i]);
      }
      Serial.println();
      Serial.println("*********");
    }
  }
};



//----------------------------

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 0, 2);

  // setup BLE serial
  BLEDevice::init(DEVICE_NAME); //Bluetooth device name
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  pService = pServer->createService(UUID_VSP_SERVICE);
  pRxCharacteristic = pService->createCharacteristic(
                                UUID_RX,
                                BLECharacteristic::PROPERTY_WRITE
                              );
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pTxCharacteristic = pService->createCharacteristic(
                                UUID_TX,
                                BLECharacteristic::PROPERTY_NOTIFY
                              );
  pTxCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(UUID_VSP_SERVICE);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Initialized");
  M5.Lcd.println("Initialized! wait central...");
}

void loop() {
  M5.update();
  
  delay(20);
}

//----------------------------

void sendTx(char value) {
  pTxCharacteristic->setValue((uint8_t*)&value, 1);
  pTxCharacteristic->notify();
}

//----------------------------
