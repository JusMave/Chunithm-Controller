
# -*- coding: utf-8 -*-

import time 
import serial

ser = serial.Serial(2) # 注意选择串口号， 从 0 开始计数， 我的是 COM3 ，所以参数是 2 
line = ser.readline() 
while line: 
    print(time.strftime(“%Y-%m-%d %X\t”) + line.strip()) 
    line = ser.readline()

# 每 10 秒向窗口写当前计算机时间
sep  = int(time.strftime("%S")) % 10
if sep == 0:
    ser.write("hello, I am hick, the time is : " + time.strftime("%Y-%m-%d %X\n"))      # write a string
ser.close()