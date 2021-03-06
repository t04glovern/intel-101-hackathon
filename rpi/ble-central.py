import websocket
import thread
from bluepy import btle
import time
import random
from datetime import datetime
import struct
import json

def large_magnitude(x, y, z):
	return (x ** 2 + y ** 2 + z ** 2) ** 0.5


def send_ws_message(msg):
	ws.connect("ws://nathanglover.com:3000/ws/bledata")
	ws.send(msg)
	ws.close()


# Web socket hook
ws = websocket.WebSocket(sslopt={"check_hostname": False})

print "Connecting..."
# This mac address is the one used by the Arduino 101
dev = btle.Peripheral("98:4F:EE:0D:02:84")

''' Debugging
print "Services..."
# Prints all the services available for the Arduino 101
for svc in dev.services:
    print str(svc)
'''

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
	time.sleep(0.05)
	accVal = arduinoAccValue.read()
	gyroVal = arduinoGyroValue.read()

	# Unpack the float arrays
	accVal_unpacked = struct.unpack('3f', accVal)
	gyroVal_unpacked = struct.unpack('3f', gyroVal)		

	# Save variables
	accX, accY, accZ = accVal_unpacked
	gyroX, gyroY, gyroZ = gyroVal_unpacked

	# Normalize
	accMag = large_magnitude(accX, accY, accZ)

	# Testing by sending random limbs
	options = ["right-arm", "left-arm", "right-leg", "left-leg"]
	data = { "sensorName" : random.choice(options), "timestamp" : str(datetime.utcnow()), "accX" : accX, "accY" : accY, "accZ" : accZ, "accMag" : accMag, "gyroX" : gyroX, "gyroY" : gyroY, "gyroZ" : gyroZ }
	json_data = json.dumps(data)
	send_ws_message(json_data)
