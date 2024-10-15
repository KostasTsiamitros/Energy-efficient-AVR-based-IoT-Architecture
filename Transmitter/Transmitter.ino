/* *

  author: Kostantinos Tsiamitros
  email: ktsiamitros@metamind.gr
  
              The Coordinator code.

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

*/

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(7, 8); // CE, CSN

const byte addresses[][10] = {"ADDRESS01","ADDRESS02"};
byte wake_sig[12];

void setup() {
  // Initialize the Serial Communication
  Serial.begin(9600);

  // Initialize the radio
  radio.begin();
  radio.openWritingPipe(addresses[0]);    // ADDRESS01
  radio.openReadingPipe(1, addresses[1]); // ADDRESS02
  radio.setPALevel(RF24_PA_HIGH);
  radio.setPayloadSize(sizeof(wake_sig));  // 2x int datatype occupy 8 bytes
  radio.maskIRQ(1, 1, 1);
  radio.startListening();

};

void transmit(){
  // Send a dummy value to the receiver
  wake_sig[0] = 255;
  radio.write(&wake_sig, sizeof(wake_sig));  
}


void receive(){
  if (radio.available()) {
    byte _received[12];
    radio.read(&_received, sizeof(_received)); 

    for(byte i = 0; i < sizeof(_received); i++){
      Serial.println(_received[i]);
    }
  };
}

void loop() {
  if(Serial.available()){
    if(Serial.read() == 'A'){
      radio.stopListening(); 
      transmit();
      radio.startListening();
    }
  }
  receive();
};
