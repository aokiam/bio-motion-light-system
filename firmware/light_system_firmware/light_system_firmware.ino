// Library needed: "ESP32 BLE Arduino" (usually bundled with the ESP32
// board package already - if not, install via Library Manager).

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BRIGHTNESS_CHAR_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ANIM_MODE_CHAR_UUID    "0a3b0f1e-6b0a-4a6a-9a3a-3e6b1a2c4d5e"
#define HEART_RATE_CHAR_UUID   "6b3f1a4c-2d5e-4f6a-8b9c-1a2b3c4d5e6f"

BLECharacteristic *brightnessChar;
BLECharacteristic *animModeChar;
BLECharacteristic *heartRateChar;

bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) {
    deviceConnected = true;
    Serial.println("Phone connected.");
  }
  void onDisconnect(BLEServer *server) {
    deviceConnected = false;
    Serial.println("Phone disconnected, restarting advertising...");
    BLEDevice::startAdvertising(); // so it can be found again
  }
};

// Called whenever the phone writes to the brightness characteristic
class BrightnessCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      uint8_t brightness = (uint8_t)value[0];
      Serial.print("Brightness set to: ");
      Serial.println(brightness);
      // TODO: apply it to actual LED strip
    }
  }
};

// Called whenever the phone writes to the animation mode characteristic
class AnimModeCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Animation mode set to: ");
      Serial.println(value);
      // TODO: switch your state machine's mode
    }
  }
};

void setup() {
  Serial.begin(9600l);
  delay(1000);

  BLEDevice::init("CosplayProp"); // name shown when scanning
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService *service = server->createService(SERVICE_UUID);

  // Brightness: phone can read current value and write a new one
  brightnessChar = service->createCharacteristic(
    BRIGHTNESS_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  brightnessChar->setCallbacks(new BrightnessCallbacks());
  uint8_t initialBrightness = 128;
  brightnessChar->setValue(&initialBrightness, 1);

  // Animation mode: phone can read/write a short string
  animModeChar = service->createCharacteristic(
    ANIM_MODE_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  animModeChar->setCallbacks(new AnimModeCallbacks());
  animModeChar->setValue("pulse");

  // Heart rate: phone subscribes to get pushed updates (notify)
  heartRateChar = service->createCharacteristic(
    HEART_RATE_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  heartRateChar->addDescriptor(new BLE2902()); // required for notify to work
  uint8_t initialBpm = 0;
  heartRateChar->setValue(&initialBpm, 1);

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE advertising started. Look for 'CosplayProp' in your scanner app.");
}

void loop() {
  // Simulated heart rate notify, once per second, only while connected.
  // Replace this fake value with your real MAX30102 reading later.
  static unsigned long lastUpdate = 0;
  static uint8_t fakeBpm = 70;

  if (deviceConnected && millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    fakeBpm = 60 + (millis() / 1000) % 40; // slowly climbs 60-99 for testing
    heartRateChar->setValue(&fakeBpm, 1);
    heartRateChar->notify();
    Serial.print("Notified heart rate: ");
    Serial.println(fakeBpm);
  }

  delay(10);
}

// How to test with nRF Connect for Mobile:
//   1. Flash this sketch, open Serial Monitor to confirm "BLE advertising
//      started" prints.
//   2. Open nRF Connect on your phone, tap Scan, look for "CosplayProp".
//   3. Tap Connect. You should see one service with three characteristics.
//   4. Tap the brightness characteristic, write a single byte value (like
//      200) - Serial Monitor should print "Brightness set to: 200".
//   5. Tap the animation mode characteristic, write a text value like
//      "slash" - Serial Monitor should print it back.
//   6. Tap the heart rate characteristic's notify icon (usually a
//      down-arrow or bell) to subscribe - you should see values update
//      once per second without you doing anything further.
