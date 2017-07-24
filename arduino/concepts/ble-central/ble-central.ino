/**
 * Curie Libraries
 */
#include <CurieBLE.h>

/**
 * Ethernet UDP Libraries
 */
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// MAC address and IP address for ethernet controller
byte eth_mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDD };
IPAddress eth_ip(192, 168, 188, 165);
unsigned int eth_port = 6553;

// Set IP and port of remote UDP host
IPAddress dest_ip(192, 168, 188, 83);
unsigned int dest_port = 8888;

// Ethernet UDP instance (send/receive)
EthernetUDP Udp;


void setup() {

  // Begin Ethernet service
  Ethernet.begin(eth_mac, eth_ip);

  // initialize the BLE hardware
  BLE.begin();

  // start scanning for peripherals
  BLE.scan();

}

void loop() {
  
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, check its one we care about

    if (peripheral.localName() == "Pixel") {

      // Begin exploring the peripheral
      explorerPeripheral(peripheral);
    }
  }
}

void explorerPeripheral(BLEDevice peripheral) {
  
  // connect to the peripheral
  if (peripheral.connect()) {
    // peripheral connects
  } 
  else {
    // peripheral failed to connects
    return;
  }

  // discover peripheral attributes
  if (peripheral.discoverAttributes()) {
    // peripheral contains attributes
  } else {
    // peripheral has no attributes
    peripheral.disconnect();
    return;
  }

  // read and print device name of peripheral
  peripheral.deviceName();

  // loop the services of the peripheral and explore each
  for (int i = 0; i < peripheral.serviceCount(); i++) {
    BLEService service = peripheral.service(i);
    exploreService(service);
  }
  peripheral.disconnect();
}

void exploreService(BLEService service) {
  // UUID of the service
  service.uuid();

  // loop the characteristics of the service and explore each
  for (int i = 0; i < service.characteristicCount(); i++) {
    BLECharacteristic characteristic = service.characteristic(i);
    exploreCharacteristic(characteristic);
  }
}

void exploreCharacteristic(BLECharacteristic characteristic) {
  // UUID and properies of the characteristic
  characteristic.uuid();
  characteristic.properties();

  // check if the characteristic is readable
  if (characteristic.canRead()) {
    // read the characteristic value
    characteristic.read();

    if (characteristic.valueLength() > 0)
    {
      // value of the characteristic
      characteristic.value(), characteristic.valueLength();
    }
  }
  
  // loop the descriptors of the characteristic and explore each
  for (int i = 0; i < characteristic.descriptorCount(); i++) {
    BLEDescriptor descriptor = characteristic.descriptor(i);
    exploreDescriptor(descriptor);
  }
}

void exploreDescriptor(BLEDescriptor descriptor) {
  // UUID of the descriptor
  descriptor.uuid();
  
  // read the descriptor value
  descriptor.read();
  descriptor.value(), descriptor.valueLength();
}

void sendDataUDP(char[] contents) {
  Udp.beginPacket(dest_ip, dest_port);
  Udp.write(contents);
  Udp.endPacket();
}

