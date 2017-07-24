from bluepy.btle import Scanner, DefaultDelegate, UUID, Peripheral
from datetime import datetime

import time
import binascii

# Bluetooth LE Settings
deviceName = "rpi-central-01"

# Extra
today = datetime.today()

# Defines how often to poll for device if data is found/not found
sleepIfFound = 20  # secs
sleepIfNotFound = 10  # secs

# Define Bluetooth LE UUIDs
validUUIDs = []

validUUIDs.append([])
validUUIDs[0].append("b43521f5-16ec-4936-849c-6c218e7bdab5")
validUUIDs[0].append("timestamp")

validUUIDs.append([])
validUUIDs[1].append("beb203a6-bdc3-4111-9569-b2a3aa121f00")
validUUIDs[1].append("data_point_1")

validUUIDs.append([])
validUUIDs[2].append("0d41699d-cae3-41d4-8504-86fd0072b31a")
validUUIDs[2].append("data_point_2")

validUUIDs.append([])
validUUIDs[3].append("8cd25b99-1690-47bc-b3ac-de594fb313de")
validUUIDs[3].append("data_point_3")

validUUIDs.append([])
validUUIDs[4].append("0b86e58a-1456-410c-a69b-601aaeaa2a46")
validUUIDs[4].append("data_point_4")

uuidCount = len(validUUIDs)


# Write data to a date store
def add_sensor_data(timestamp, datePoint1, datePoint2, datePoint3, datePoint4):
    time_stamp = datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')
    data = "Timestamp: {} | Data 1: {} | Data 2: {} | Data 3: {} | Data 4: {} | ".format(time_stamp, datePoint1, datePoint2, datePoint3, datePoint4)
    print data
    return data


# Function to determine BTLE devices available
class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            print "Discovered device", dev.addr
        elif isNewData:
            print "Received new data from", dev.addr


def connect_to_device(mac_address):
    # Get a reference to the connect device peripheral
    arduino_peripheral = Peripheral(mac_address)

    timestamp, data_point_1, data_point_2, data_point_3, data_point_4 = "blank"

    # Iterate over all the known UUIDs
    for x in range(0, uuidCount):
        try:
            uuid_val = validUUIDs[x][0]
            arduino_characteristics = arduino_peripheral.getCharacteristics(uuid=uuid_val)[0]
            if arduino_characteristics.supportsRead():
                val = binascii.b2a_hex(arduino_characteristics.read())
                val = binascii.unhexlify(val)
                val = str(val).strip()
                val = int(val)

                #print validUUIDs[x][1] + ": " + str(val)

                if validUUIDs[x][1] == "timestamp":
                    timestamp = val
                elif validUUIDs[x][1] == "data_point_1":
                    data_point_1 = val
                elif validUUIDs[x][1] == "data_point_2":
                    data_point_2 = val
                elif validUUIDs[x][1] == "data_point_3":
                    data_point_3 = val
                elif validUUIDs[x][1] == "data_point_4":
                    data_point_4 = val
        finally:
            pass

     # Add sensor data to the datastore
    add_sensor_data(timestamp, data_point_1, data_point_2, data_point_3, data_point_4)

    # Disconnect from the device peripheral
    arduino_peripheral.disconnect()


def get_scan():
    scanner = Scanner().withDelegate(ScanDelegate())
    devices = scanner.scan(4.0)

    base_device_id = "arduino-node-"
    mac_addr = "empty"

    for dev in devices:
        for (adtype, desc, value) in dev.getScanData():
            if desc == "Complete Local Name" and base_device_id in value:
                print "Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi)
                print "  %s = %s" % (desc, value)
                mac_addr = dev.addr
    return mac_addr


def get_mac(val_name):
    scanner = Scanner().withDelegate(ScanDelegate())
    devices = scanner.scan(4.0)

    mac_addr = "empty"

    for dev in devices:
        for (adtype, desc, value) in dev.getScanData():
            if desc == "Complete Local Name" and value == val_name:
                print "Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi)
                print "  %s = %s" % (desc, value)
                mac_addr = dev.addr
    return mac_addr


# Enable bluetooth
time.sleep(3)

# Used to ensure we only check every 30 seconds
check_time = 0

while 1:
    while check_time < time.time():
        # Check if arduino node is available
        #arduino_mac = get_mac("arduino-node-01")
        arduino_mac = get_scan()

        if arduino_mac != "empty":
            try:
                connect_to_device(arduino_mac)
                check_time = time.time() + sleepIfFound
                arduino_mac = "empty"
                print "Reading obtained, waiting " + str(sleepIfFound) + " seconds"
            except:
                arduino_mac = "empty"
        else:
            print "No device found, searching again in " + str(sleepIfNotFound) + " seconds"
            check_time = time.time() + sleepIfNotFound
