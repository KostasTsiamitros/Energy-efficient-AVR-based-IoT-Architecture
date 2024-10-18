/**
  The Arduino Uno code used in the evaluations.

  Improvements can be made. See Future Work.

  author: Kostantinos Tsiamitros
  email: ktsiamitros@metamind.gr
*/


#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define heartbeat 13

RF24 radio(7, 8); // CE, CSN
const byte addresses[][10] = {"ADDRESS01","ADDRESS02"};


void setup() {

  pinMode(heartbeat, OUTPUT);
  
  digitalWrite(heartbeat, HIGH);
  delay(1000);
  digitalWrite(heartbeat, LOW);
  delay(1000);

  Serial.begin(9600);

  digitalWrite(heartbeat, HIGH);
  delay(1000);
  digitalWrite(heartbeat, LOW);
  delay(1000);  


  radio.begin();  
  radio.openWritingPipe(addresses[1]);    // ADDRESS02
  radio.openReadingPipe(1, addresses[0]); // ADDRESS01
  radio.setPALevel(RF24_PA_HIGH);
  radio.maskIRQ(1, 1, 1);

  digitalWrite(heartbeat, HIGH);
  delay(1000);
  digitalWrite(heartbeat, LOW);
  delay(1000);

  radio.startListening();
};

void transmit(char id[], char val[]){
  // Transmit
  char txt_sent[15] = {id[0], id[1], id[2], id[3], val[0], val[1], val[2], val[3], val[4], val[5]};
  //const char txt_sent[]  = "Sender 2";
  //Serial.println(txt_sent);
  radio.write(&txt_sent, sizeof(txt_sent));
  //Serial.println(sizeof(txt_sent));  
  //Serial.print("Sent to: ");
  //Serial.println(txt_sent);
}

void receive(){
  if (radio.available()) {                // Receive
    char txt_received[15];
    radio.read(&txt_received, sizeof(txt_received)); 
    //Serial.print("Received from: ");
    //Serial.println(txt_received);
    digitalWrite(heartbeat, HIGH);
    delay(100);
    digitalWrite(heartbeat, LOW);
    delay(100);

  float battery_level = analogRead(A0) * (5.0 / 1024.0);
  //delay(100);
  //Serial.println("BAT: " + String(battery_level ));
  char val[6];
  dtostrf(battery_level, 2, 2, val);
  //Serial.println(val);
  //Serial.println(sizeof(val));
  char id[4] = {'B', 'A', 'T', ':'};
  radio.stopListening();
  //delay(500);
  transmit(id, val);
  //delay(500);
  radio.startListening();

  delay(750);
  float soil_humidity = analogRead(A1) * (100.0 / 1024.0);
  //Serial.println("SH: " + String(soil_humidity));
  //text = "SH: " + char(soil_humidity);
  dtostrf(soil_humidity, 2, 2, val);
  //Serial.println(val);
  //Serial.println(sizeof(val));
  id[0] = 'S';
  id[1] = 'H';
  id[2] = 'M';
  id[3] = ':';
  radio.stopListening();
  //delay(500);
  transmit(id, val);
  //delay(500);
  radio.startListening();

  
  float light_intensity = analogRead(A2) * (100.0 / 1024.0);
  //Serial.println("LI: " + String(light_intensity));
  //text = "LI: " + char(light_intensity);
  dtostrf(light_intensity, 2, 2, val);
  //Serial.println(val);
  //Serial.println(sizeof(val));
  id[0] = 'L';
  id[1] = 'I';
  id[2] = 'N';
  id[3] = ':';
  radio.stopListening();
  //delay(500);
  transmit(id, val);
  //delay(500);
  radio.startListening();
  }
}

void loop() {
  receive();
};
