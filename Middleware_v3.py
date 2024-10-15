import serial
import requests
from decimal import Decimal
import json
import datetime
import time

def read_values():
    
    #read the bytes from USB
    sensor_meas = []
    while ser.in_waiting:
        # and store them in the list as integers
        sensor_meas.append(int((ser.readline()).strip()))
    
    #print(sensor_meas)
    for i in range(0, len(sensor_meas), 4):
        
        meas = sensor_meas[i:i+4]
        #print(meas)
        
        # "decimal point" detected
        if meas[2] == 255:
            print(str(i) + " to " + str(i+3) + " bytes received CORRECTLY")
        else:
            print(str(i) + " to " + str(i+3) + " bytes received INCORRECTLY")

        _id = meas[0]
        _val = float(str(meas[1]) + "." + str(meas[3]))
        print("Measurement id: " + str(_id) + " Value: " + str(_val))
    
        #Update the measurements array
        measurements[_id] = _val
        
if __name__ == "__main__":

    #open the serial port
    ser=serial.Serial(port="/dev/ttyACM0",baudrate=9600, timeout=2)

    # Initialize the measurements array
    # Here ther collected measurements will be stored
    # ---- Let:
    # index 0 -> Battery voltage measurement
    # index 1 -> Soil Humidity measurement
    # index 2 -> Light Intensity Measurement
    measurements = [5000, 5000, 5000]
    
    # Interval between sensor measurements
    MINS = 15 # how many minutes to sleep
    # IMPORTANT, please allow adequate time
    # for collection and sending of the data
    # 5 seconds should be more than enough
    SECS = 0 # how many seconds to sleep
    
    
    # In Seconds
    RETRY_DELAY = 5

    while True:
        print("Sleeping for " + str(RETRY_DELAY) + " seconds...")
        time.sleep(RETRY_DELAY)
        
        # Initiate a sensor data collection
        ser.write(b'A')

        # Read the USB port, parse and collect the measurtements
        read_values()    

        print(sum(measurements))
        if sum(measurements) < 5000:
            # The API endpoint
            url = "http://localhost:8000/my_server/store_measurement_v3/"

            # Data to be sent
            data = {
                "battery_level": measurements[0],
                "light_intensity": measurements[1],
                "soil_humidity": measurements[2],
            }

            # A POST request to the API
            response = requests.post(url, json=data)

            # Print the response
            print(response)#.json())
            
            # Clear the measurements array
            measurements = [5000, 5000, 5000]

            # Schedule the next "sleeping" interval
            now = datetime.datetime.now()
            next_measurement = now
            next_measurement += datetime.timedelta(minutes=MINS, seconds=SECS)
            interval = (next_measurement-now).total_seconds()
            print("Sleeping for: " + str(interval) + " seconds.")
            print("Now: " + str(now))
            print("Sleeping until: " + str(next_measurement))
            time.sleep(interval)

