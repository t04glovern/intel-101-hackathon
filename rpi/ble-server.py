from bluepy import btle
import time
import struct

print "Connecting..."
# This mac address is the one used by the Arduino 101
dev = btle.Peripheral("98:4F:EE:0D:02:84")

print "Services..."
# Prints all the services available for the Arduino 101
for svc in dev.services:
    print str(svc)

# Arduino 101 imuService UUID
arduinoDevice = btle.UUID("917649A0-D98E-11E5-9EEC-0002A5D5C51B")
arduinoService = dev.getServiceByUUID(arduinoDevice)

# Arduino 101 Accelerometer & Gyroscope Characteristics
imuAccUUID = btle.UUID("917649A1-D98E-11E5-9EEC-0002A5D5C51B")
imuGyroUUID = btle.UUID("917649A2-D98E-11E5-9EEC-0002A5D5C51B")

''' 
Data for the two characteristics are stored like this

union
{
  float a[3];
  unsigned char bytes[12];
} accData;

union
{
  float g[3];
  unsigned char bytes[12];
} gyroData;

'''

# Accelerometer & Gyroscope Characteristic mapping
# Index zero denotes the float field in each union
arduinoAccValue = arduinoService.getCharacteristics(imuAccUUID)[0]
arduinoGyroValue = arduinoService.getCharacteristics(imuGyroUUID)[0]

while (1):
    time.sleep(1.0)
    accVal = arduinoAccValue.read()
    gyroVal = arduinoGyroValue.read()

    # Unpack the float arrays
    accVal_unpacked = struct.unpack('3f', accVal)
    gyroVal_unpacked = struct.unpack('3f', gyroVal)

    print accVal_unpacked
    print gyroVal_unpacked
