import socket
import binascii
import struct

UDP_IP = ""
UDP_PORT = 8888

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

def unpack_helper(fmt, data):
	size = struct.calcsize(fmt)
	return struct.unpack(fmt, data[:size]), data[size:]


while True:
	data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
	print "received message:", data
