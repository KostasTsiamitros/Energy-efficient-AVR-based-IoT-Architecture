# Energy-efficient AVR-based IoT Architecture with Separate Battery Management System for Edge Computing

Contact info
email: ktsiamitros@metamind.gr

This GitHub repository is accompanying the homonymous publication.

~~~~~~~~~~~ HARDWARE CONFIGURATION ~~~~~~~~~~~~~
Towards the purposes of a functional demo,
the coordinator has been set up as an Arduino DUE, 
connected via USB to a Raspberry Pi 3 Model B.

~~~~~~~~~~~ SOFTWARE CONFIGURATION ~~~~~~~~~~~~~
On the Raspberry Pi, we installed the latest version of:
----> Rasbian OS
----> Django Web Framework + ChartJS
----> Python 3 (PySerial)

~~~~~~~~~~~ DJANGO WEB FRAMEWORK ~~~~~~~~~~~~~
Utilizing the Django Web Framework we constructed a dummy web page
which ran on localhost port 8000 (the default settings of Django's
webserver). On this webpage we created some models and views in
order to facilitate two endpoints. One for POST-ing the collected
sensor data to the database, and another one for visualizing the results
(with ChartJS).

~~~~~~~~~~~ PYTHON MIDDLEWARE ~~~~~~~~~~~~~
On the Raspberry Pi, a middleware python program was ran in order to read 
the Serial port on which the DUE writtes on specific time intervals 
(15-minute intervals) the collected sensor values.

~~~~~~~~~~~ SCHEDULING & SENSOR READING PROCESS ~~~~~~~~~~~~~
In order to poll the sensors for their readings on specific intervals,
the scheduling happens on the Pi's middleware. Whenever the time interval 
elapses, the middleware writes the character 'A' to the DUE's serial port.
In turn, the DUE sends a message through the nRF24L01 radio to our proposed
IoT Architecture instance node. Afterwards the ATMega328P:
1. is woken up
2. reads the sensors
3. compiles and transmits a message with the sensor readings over the radio
Finally, the DUE echoes the received message to the Middleware, which in turn 
decodes and POSTs it to a specific endpoint of the webpage that we created.
When the sensor values reach the webpages, they are stored in an SQL Databse.

~~~~~~~~~~~ PROBLEMS IDENTIFIED ~~~~~~~~~~~~~
The main power consumer on the IoT Architecture is the nRF24L01 radio which
is powered on constantly. This problem will be tackled in the future to
further reduce the total power consumption of the board.

