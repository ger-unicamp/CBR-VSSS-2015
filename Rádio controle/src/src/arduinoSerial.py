import serial
import string

ser = serial.Serial('/dev/cu.usbmodem1421', 9600, timeout = 1)

def sendString(robo, dirR, speedR, dirL, speedL):
	ser.write('S' + str(robo) + "#" + str(dirR) + '#' + str(speedR) + '#' + str(dirL) + '#' + str(speedL) + 'E')
	return ser.readline()

def closeSerial():
	ser.close()
