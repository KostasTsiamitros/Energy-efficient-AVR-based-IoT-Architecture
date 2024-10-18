#include <avr/sleep.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "LowPower.h"

// PINS:
// 0 - RX
// 1 - TX
#define nRF_IRQ_PIN 2
#define heartbeat 3
#define NPN_TRANSISTOR_PIN 4
// 5, 6 - UNUSED
RF24 radio(7, 8); // CE, CSN
// A0 - battery monitoring sensor
// A1 - Soil Humidity sensor
// A2 - Light Intensity (LDR) sensor
const byte UNUSED_PINS[7] = {5, 6, 9, 10, A3, A4, A5};

#define NUM_SENSORS 3

// TODO: read this from a hardware switch at startup
#define DEBUG LOW 

const byte addresses[][10] = {"ADDRESS01","ADDRESS02"};

// First dimension: <NUM_SENSORS>
// ----- how many sensors are connected
float measurements[NUM_SENSORS];

void wake ()
{
  // cancel sleep as a precaution
  sleep_disable();
  
  // precautionary while we do other stuff
  detachInterrupt (digitalPinToInterrupt(nRF_IRQ_PIN));
  
  // Read and Clear the flags
  bool tx_ds, tx_df, rx_dr;
  radio.whatHappened(tx_ds, tx_df, rx_dr);
}

void setup() {

  // Initialize the NPN Base pin to OUTPUT and disable power to it
  pinMode(NPN_TRANSISTOR_PIN, OUTPUT);
  digitalWrite(NPN_TRANSISTOR_PIN, LOW);
  
  // Initialize the heartbeat LED pin
  pinMode(heartbeat, OUTPUT);
  
  // "Disable" Unused pins
  for (byte i = 0; i < sizeof(UNUSED_PINS); i++)
  {
    pinMode (UNUSED_PINS[i], OUTPUT);
    digitalWrite (UNUSED_PINS[i], LOW); 
  }

  // 1st Blink
  digitalWrite(heartbeat, HIGH);
  delay(1000);
  digitalWrite(heartbeat, LOW);
  delay(1000);

  if (DEBUG == HIGH){
    // Start Serial Communication
    Serial.begin(9600);

    // 2nd Blink
    digitalWrite(heartbeat, HIGH);
    delay(1000);
    digitalWrite(heartbeat, LOW);
    delay(1000);  
  }

  // Configure and start the nRF24L01
  radio.begin();  
  radio.openWritingPipe(addresses[1]);    // ADDRESS02
  radio.openReadingPipe(1, addresses[0]); // ADDRESS01
  radio.setPALevel(RF24_PA_HIGH);
  radio.setPayloadSize(12);
  radio.maskIRQ(1, 1, 0);
  
  // Attach the IRQ pin to the <wake> interrupt service routine
  pinMode(digitalPinToInterrupt(nRF_IRQ_PIN), INPUT);
  attachInterrupt (digitalPinToInterrupt(nRF_IRQ_PIN), wake, FALLING);

  // 2nd Blink (or 3rd if DEBUG == HIGH)
  digitalWrite(heartbeat, HIGH);
  delay(1000);
  digitalWrite(heartbeat, LOW);
  delay(1000);  

  // begin listening for transmissions
  radio.startListening();
};

void loop() {
  radio.stopListening();
  radio.flush_rx();

  if (DEBUG == HIGH)
  {
    digitalWrite(heartbeat, HIGH);
    delay(100);
    digitalWrite(heartbeat, LOW);
    delay(100);
  }

  digitalWrite(NPN_TRANSISTOR_PIN, HIGH);
  // Battery voltage level (in volts)
  measurements[0] = analogRead(A0) * (5.0 / 1024.0);
  // Light Intensity (in percentage)
  measurements[1] = analogRead(A2) * (100.0 / 1024.0);
  delay(750);
  // Soil Humidity (in percentage)
  measurements[2] = analogRead(A1) * (100.0 / 1024.0);
  digitalWrite(NPN_TRANSISTOR_PIN, LOW);

  /* Dummy payload --- for testing only
  byte payload[12] = {
    0, 100, 255, 99, 
    1, 100, 255, 99,
    2, 100, 255, 99,
  };
  */

// Payload: 
// ----- 4 bytes are needed
// ----- sensor_id, whole part, decimal point, decimal part
// ----- e.g. sensor_id = 0, value = 100.99
// ----------  payload[0] = 0
// ----------  payload[1] = 100
// ----------  payload[2] = 255 (the decimal point -- used as "parity check")
// ----------  payload[3] = 99

  byte payload[12];
  byte i;
  byte _id = 0;
  for(i = 0; i < sizeof(payload) ; i += 4){
    payload[i] = _id;
    payload[i + 1] = (int)measurements[_id]; // integer part
    payload[i + 2] = 255; // the decimal point -- used as "parity check"
    payload[i + 3] = 100 * (measurements[_id] - payload[i + 1]);

    _id++;
  }

  // Transmit the payload
  radio.write(&payload, sizeof(payload));

  if (DEBUG == HIGH)
  {
    Serial.print("PAYLOAD TO TRANSMIT: ");
    for(i = 0; i < sizeof(payload); i++){
      Serial.print(payload[i]);
      Serial.print(" ");
    }
    
    Serial.print("WITH SIZE: ");
    Serial.println(sizeof(payload));

    digitalWrite(heartbeat, HIGH);
    delay(100);
    digitalWrite(heartbeat, LOW);
    delay(100);
    
    // this delay exists only to give enough time to the serial port
    // to transmit to the PC
    delay(500);
  }

  radio.startListening();

  attachInterrupt (digitalPinToInterrupt(nRF_IRQ_PIN), wake, FALLING);  
  LowPower.powerDown(SLEEP_FOREVER, ADC_ON, BOD_ON); 
};
