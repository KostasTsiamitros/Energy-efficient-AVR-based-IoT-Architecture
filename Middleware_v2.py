import serial
import requests
from decimal import Decimal
import json
import datetime
import time

def read_value(sensor_val):
    global bat, lin, shm
    print(sensor_val)
    if b'LIN' in sensor_val:
        #li = Decimal.from_float(float(sensor_val))
        lin = float(sensor_val[4:-2])
        measurements[0] = lin
    elif b'SHM' in sensor_val:
        #s_temp = Decimal.from_float(float(sensor_val))
        shm = float(sensor_val[4:-2])
        measurements[1] = shm
    elif b'BAT' in sensor_val:
        #hum = Decimal.from_float(float(sensor_val))
        bat = float(sensor_val[4:-2])
        measurements[2] = bat

bat, lin, shm = 0, 0, 0
ser=serial.Serial(port="/dev/ttyACM0",baudrate=9600, timeout=2           )  #change ACM number as found from ls /dev/tty/ACM*
#ser.baudrate=9600


measurements = [5000, 5000, 5000]

while True:

    
    time.sleep(5)
    
    #while True:
    ser.write(b'A')
    
    #
    
    #while ser.in_waiting:
    sensor_val=ser.readline()
    #print(read_ser)
    read_value(sensor_val)
    
    sensor_val=ser.readline()
    #print(read_ser)
    read_value(sensor_val)
    
    sensor_val=ser.readline()
    #print(read_ser)
    read_value(sensor_val)
    #if b"Acknowledge data" in read_ser:
    #sensor_val = ser.readline()
    #index = ser.readline()
    #print(index, " ", sensor_val)
    

    print(sum(measurements))
    if sum(measurements) < 5000:
        # The API endpoint
        url = "http://localhost:8000/my_server/store_measurement_inefficient/"

        # Data to be sent
        data = {
            "battery_level": bat,
            "light_intensity": lin,
            "soil_humidity": shm,
        }

        # A POST request to the API
        response = requests.post(url, json=data)

        # Print the response
        print(response)#.json())
        
        measurements = [5000, 5000, 5000]
        #break
        #print(datetime.datetime.now().hour)
        now = datetime.datetime.now()
        next_measurement = now
        next_measurement += datetime.timedelta(minutes=15)
        interval = (next_measurement-now).total_seconds()
        print("Sleeping for: " + str(interval) + " seconds.")
        print("Now: " + str(now))
        print("Sleeping until: " + str(next_measurement))
        time.sleep(interval)
