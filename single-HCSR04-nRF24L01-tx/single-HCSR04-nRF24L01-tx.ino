

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


/* RADIO PINS + RADIO CHANNEL */

RF24 radio(9, 10); // CE, CSN         
const byte address[6] = "00001";     //Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.


/* PINS DECLARATION */

const int trigPinL = 5;
const int echoPinL = 8;


/* CROSSING STATE VARIABLES */
float durationL, distanceL;
boolean crossed = 0;




/* SETTABLE PARAMETERS */

const int minDistance = 80;   // threshold for crossing
const int minReadTime = 100;  // minimum time for a single sensor detection to read a single crossing
const int maxWaitRead = 100;  // max time after which a cross has finished





void setup() {
  
/* DISTANCE SENSORS SETUP */

pinMode(trigPinL, OUTPUT);
pinMode(echoPinL, INPUT);


/* RADIO TRANSMITTER SETUP */
radio.begin();                  //Starting the Wireless communication
  radio.setDataRate( RF24_250KBPS );
  radio.setChannel( 90 );
radio.openWritingPipe(address); //Setting the address where we will send the data
radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distanceL between the transmitter and receiver.
radio.stopListening();          //This sets the module as transmitter
Serial.begin(9600);

}



void loop() {

  /* CALCULATE ULTRASOUND TIME OF RETURN LEFT */
  
  digitalWrite(trigPinL, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinL, LOW);
  durationL = pulseIn(echoPinL, HIGH);
  distanceL = (durationL * .0343) / 2;
  delay(10);


/* CROSSING SENSOR */

if (distanceL < minDistance)
    {

      if (crossed == 0) {
      crossed = 1;
      radio.write(&crossed, sizeof(crossed));
      Serial.println(crossed);
       delay(100);
      }
    
    }
else
    {
      if (crossed == 1) {
      crossed = 0;
      radio.write(&crossed, sizeof(crossed));
      Serial.println(crossed);
      delay(100);
      }
     
    }





}
