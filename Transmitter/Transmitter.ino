/* *

  author: Kostantinos Tsiamitros
  email: ktsiamitros@metamind.gr
  
              The Coordinator code.
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
