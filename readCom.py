from time import sleep
import serial
ser = serial.Serial('COM25', 115200)
ser.flush()
while True:
    s = ser.read(1) 
    print(s)