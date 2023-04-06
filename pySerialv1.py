import serial
from time import localtime, strftime

ser = serial.Serial(port='COM5', baudrate=9600, bytesize=8, parity='N', stopbits=1, timeout=2, xonxoff=0, rtscts=0)

temp_file = open('readings.txt', 'w', encoding = 'utf-8')

while(True):
    line = ser.readline()
    line_dec = line.decode('utf-8').rstrip()
    readd = line_dec.split()
    # print(readd)
    if len(readd)==2 : print("Volt: ",readd[1]," Current: ",readd[0])

    # temp_file.write(strftime("%d %b %Y %H%M%S ", localtime()))
    if(line_dec=="xxxx") : break
    temp_file.write(line.decode())
    # temp_file.write('\n')
