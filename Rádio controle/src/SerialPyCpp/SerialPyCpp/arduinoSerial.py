import serial
import time
import string

ser = serial.Serial('/dev/cu.usbmodem1421', 9600)
ser.dsrdtr=False
ser.setDTR(level=False)
time.sleep(2)
def sendString(robo, dirR, speedR, dirL, speedL):
	ser.write('S' + str(robo) + "#" + str(dirR) + '#' + str(speedR) + '#' + str(dirL) + '#' + str(speedL) + 'E')
	return ser.readline()

def closeSerial():
	ser.close()
