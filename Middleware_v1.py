import serial
import requests
import json
import datetime
import time

ser=serial.Serial(port="/dev/ttyACM0",baudrate=9600)
measurements = [5000, 5000, 5000]

while True:
    now = datetime.datetime.now()
    next_measurement = now
    next_measurement += datetime.timedelta(seconds=5)
    interval = (next_measurement-now).total_seconds()
    print("Sleeping for: " + str(interval) + " seconds.")
    time.sleep(interval)

    while True:
        ser.write(b'A')
        read_ser=ser.readline()
        #print(read_ser)
        
        if b"Acknowledge data" in read_ser:
            sensor_val = ser.readline()
            index = ser.readline()
            print(index, " ", sensor_val)
            if b'0.00' in index:
                li = float(sensor_val)
                measurements[0] = li
            elif b'1.00' in index:
                s_temp = float(sensor_val)
                measurements[1] = s_temp
            elif b'2.00' in index:
                hum = float(sensor_val)
                measurements[2] = hum
        
            print(sum(measurements))
            if sum(measurements) < 5000:
                # The API endpoint
                url = "http://localhost:8000/my_server/store_measurement/"

                # Data to be sent
                data = {
                    "humidity": hum,
                    "light_intensity": li,
                    "soil_temperature": s_temp,
                }

                # A POST request to the API
                response = requests.post(url, json=data)

                # Print the response
                print(response)
                
                measurements = [5000, 5000, 5000]
                break
            else:
                print("Request not ready yet.")
